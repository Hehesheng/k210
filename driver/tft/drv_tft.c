#include <fpioa.h>
#include <gpiohs.h>
#include <rtthread.h>
#include <spi.h>
#include <stdlib.h>
#include <sysctl.h>

#include "drv_tft.h"

#define LOG_TAG "drv.tft"
#include <ulog.h>

struct tft_hw
{
    spi_device_num_t spi_channel;
    spi_chip_select_t select;
    dmac_channel_number_t dma_channel;
    int cs;
    int dc;
    int clk;
    int rst;
};

struct k210_tft
{
    struct rt_device parent;
    struct tft_hw hw;
    struct rt_device_graphic_info info;
    uint32_t fps;
};

static struct k210_tft tft;

static void tft_thread(void *param);

static void init_dcx(void)
{
    fpioa_set_function(tft.hw.dc, FUNC_GPIOHS22);
    gpiohs_set_drive_mode(22, GPIO_DM_OUTPUT);
    gpiohs_set_pin(22, GPIO_PV_HIGH);
}

static void set_dcx_control(void) { gpiohs_set_pin(22, GPIO_PV_LOW); }

static void set_dcx_data(void) { gpiohs_set_pin(22, GPIO_PV_HIGH); }

static void init_rst(void)
{
    fpioa_set_function(tft.hw.rst, FUNC_GPIOHS21);
    gpiohs_set_drive_mode(21, GPIO_DM_OUTPUT);
    gpiohs_set_pin(21, GPIO_PV_HIGH);
}

static void set_rst(uint8_t val) { gpiohs_set_pin(21, val); }

static void tft_set_clk_freq(uint32_t freq) { spi_set_clk_rate(tft.hw.spi_channel, freq); }

static void init_cs(void) { fpioa_set_function(tft.hw.cs, FUNC_SPI0_SS0 + tft.hw.select); }

static void init_clk(void) { fpioa_set_function(tft.hw.clk, FUNC_SPI0_SCLK); }

static void tft_write_command(uint8_t cmd)
{
    set_dcx_control();

    spi_init(tft.hw.spi_channel, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(tft.hw.spi_channel, 8 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_normal_dma(tft.hw.dma_channel, tft.hw.spi_channel, tft.hw.select, (uint8_t *)(&cmd), 1, SPI_TRANS_CHAR);
}

static void tft_write_byte(uint8_t *data_buf, uint32_t length)
{
    set_dcx_data();

    spi_init(tft.hw.spi_channel, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(tft.hw.spi_channel, 0 /*instrction length*/, 8 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_normal_dma(tft.hw.dma_channel, tft.hw.spi_channel, tft.hw.select, data_buf, length, SPI_TRANS_CHAR);
}

static void tft_write_half(uint16_t *data_buf, uint32_t length)
{
    set_dcx_data();

    spi_init(tft.hw.spi_channel, SPI_WORK_MODE_0, SPI_FF_OCTAL, 16, 0);
    spi_init_non_standard(tft.hw.spi_channel, 0 /*instrction length*/, 16 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_normal_dma(tft.hw.dma_channel, tft.hw.spi_channel, tft.hw.select, data_buf, length, SPI_TRANS_SHORT);
}

static void tft_write_word(uint32_t *data_buf, uint32_t length)
{
    set_dcx_data();

    spi_init(tft.hw.spi_channel, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);

    spi_init_non_standard(tft.hw.spi_channel, 0 /*instrction length*/, 32 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_normal_dma(tft.hw.dma_channel, tft.hw.spi_channel, tft.hw.select, data_buf, length, SPI_TRANS_INT);
}

static void tft_fill_data(uint32_t *data_buf, uint32_t length)
{
    set_dcx_data();

    spi_init(tft.hw.spi_channel, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);
    spi_init_non_standard(tft.hw.spi_channel, 0 /*instrction length*/, 32 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_fill_data_dma(tft.hw.dma_channel, tft.hw.spi_channel, tft.hw.select, data_buf, length);
}

rt_err_t tft_hw_init(rt_device_t dev)
{
    uint8_t data;
    rt_thread_t tid;

    init_clk();
    init_cs();
    init_dcx();
    init_rst();
    set_rst(0);

    spi_init(tft.hw.spi_channel, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    tft_set_clk_freq(20000000);

    rt_thread_mdelay(50);
    set_rst(1);
    rt_thread_mdelay(50);

    /*soft reset*/
    tft_write_command(SOFTWARE_RESET);
    rt_thread_mdelay(150);
    /*exit sleep*/
    tft_write_command(SLEEP_OFF);
    rt_thread_mdelay(500);
    /*pixel format*/
    data = 0x55;
    tft_write_command(PIXEL_FORMAT_SET);
    tft_write_byte(&data, 1);
    rt_thread_mdelay(10);

    data = DIR_YX_RLDU;
    tft_write_command(MEMORY_ACCESS_CTL);
    tft_write_byte((uint8_t *)&data, 1);

    tft_write_command(NORMAL_DISPALY_ON);
    rt_thread_mdelay(10);
    /*display on*/
    tft_write_command(DISPALY_ON);
    rt_thread_mdelay(100);
    /* alloc mem */
    tft.info.framebuffer = rt_malloc_align(tft.info.width * tft.info.height * tft.info.bits_per_pixel / 8 * 2, 8);
    rt_memset(tft.info.framebuffer, 0, tft.info.width * tft.info.height * tft.info.bits_per_pixel / 8 * 2);

    tid = rt_thread_create("tft", tft_thread, RT_NULL, 4096, 10, 50);
    rt_thread_startup(tid);

    return RT_EOK;
}

int rt_hw_tft_init(void)
{
    rt_err_t ret;

    tft.parent.type    = RT_Device_Class_Graphic;
    tft.parent.init    = tft_hw_init;
    tft.parent.open    = RT_NULL;
    tft.parent.close   = RT_NULL;
    tft.parent.read    = RT_NULL;
    tft.parent.write   = RT_NULL;
    tft.parent.control = RT_NULL;

    tft.info.bits_per_pixel = 16;
    tft.info.pixel_format   = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    tft.info.width          = 320;
    tft.info.height         = 240;

    tft.parent.user_data = (void *)&tft.info;

    tft.hw = (struct tft_hw){
        .spi_channel = 0,
        .select      = LCD_SPI_SLAVE_SELECT,
        .dma_channel = DMAC_CHANNEL1,
        .cs          = 36,
        .dc          = 38,
        .clk         = 39,
        .rst         = 37,
    };

    sysctl_set_spi0_dvp_data(1);
    ret = rt_device_register(&tft.parent, "tft", RT_DEVICE_FLAG_RDWR);

    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_tft_init);

void tft_set_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint8_t data[4] = {0};

    data[0] = (uint8_t)(x1 >> 8);
    data[1] = (uint8_t)(x1);
    data[2] = (uint8_t)(x2 >> 8);
    data[3] = (uint8_t)(x2);
    tft_write_command(HORIZONTAL_ADDRESS_SET);
    tft_write_byte(data, 4);

    data[0] = (uint8_t)(y1 >> 8);
    data[1] = (uint8_t)(y1);
    data[2] = (uint8_t)(y2 >> 8);
    data[3] = (uint8_t)(y2);
    tft_write_command(VERTICAL_ADDRESS_SET);
    tft_write_byte(data, 4);

    tft_write_command(MEMORY_WRITE);
}

void tft_clear(uint16_t color)
{
    uint32_t data = ((uint32_t)color << 16) | (uint32_t)color;

    tft_set_area(0, 0, tft.info.width - 1, tft.info.height - 1);
    tft_fill_data(&data, tft.info.width * tft.info.height / 2);
}

void tft_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= tft.info.width) x = tft.info.width - 1;
    if (y >= tft.info.height) y = tft.info.height - 1;
    tft_set_area(x, y, x, y);
    tft_write_half(&color, 1);
}

void tft_draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    uint16_t x1, x2, y1, y2;
    uint32_t data = 0;

    if (x >= tft.info.width)
        x1 = tft.info.width - 1;
    else
        x1 = x;
    if (y >= tft.info.height)
        y1 = tft.info.height - 1;
    else
        y1 = y;
    if (x + width >= tft.info.width)
        x2 = tft.info.width - 1;
    else
        x2 = x + width;
    if (y + height >= tft.info.height)
        y2 = tft.info.height - 1;
    else
        y2 = y + height;
    data = ((uint32_t)color << 16 | (uint32_t)color);
    tft_set_area(x1, y1, x2, y2);
    tft_fill_data(&data, (x2 - x1) * (y2 - y1) / 2);
}

void tft_draw_picture(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint16_t *ptr)
{
    tft_set_area(x1, y1, x1 + width, y1 + height);
    tft_write_half(ptr, width * height);
}

void tft_flush(void)
{
    if (tft.info.framebuffer == RT_NULL) return;
    tft_set_area(0, 0, tft.info.width - 1, tft.info.height - 1);

    set_dcx_data();
    spi_init(tft.hw.spi_channel, SPI_WORK_MODE_0, SPI_FF_OCTAL, 16, 0);
    spi_init_non_standard(tft.hw.spi_channel, 0 /*instrction length*/, 16 /*address length*/, 0 /*wait cycles*/,
                          SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    spi_send_data_u32_as_u16(tft.hw.dma_channel, tft.hw.spi_channel, tft.hw.select, tft.info.framebuffer,
                             tft.info.width * tft.info.height);
}

uint32_t *tft_get_frambuffer(void) { return (uint32_t *)tft.info.framebuffer; }

static void tft_thread(void *param)
{
    uint32_t t = 10;
    rt_tick_t start, end;
    struct k210_tft *dev = (struct k210_tft *)rt_device_find("tft");

    tft_clear(0);

    while (1)
    {
        start = rt_tick_get();
        tft_flush();
        rt_thread_mdelay(t);
        end      = rt_tick_get();
        dev->fps = RT_TICK_PER_SECOND * 100 / (end - start);
        /* keep 60fps */
        if (dev->fps > 6000)
            t++;
        else
            t--;
        if (t > RT_TICK_PER_SECOND) t = 0;
    }
}

static void get_fps(int argc, char **argv)
{
    struct k210_tft *dev;

    if (argc < 2)
    {
        return;
    }
    dev = (struct k210_tft *)rt_device_find(argv[1]);
    if (dev)
    {
        rt_kprintf("fps: %d.%02d\n", dev->fps / 100, dev->fps % 100);
    }
}
MSH_CMD_EXPORT(get_fps, show fps);

static void run_tft(int argc, char **argv)
{
    if (tft.parent.flag & RT_DEVICE_FLAG_ACTIVATED) return;
    rt_device_init(&tft.parent);
}
MSH_CMD_EXPORT(run_tft, tft run);
