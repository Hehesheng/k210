#include <fpioa.h>
#include <gpiohs.h>
#include <rtthread.h>
#include <spi.h>
#include <sysctl.h>

#include "drv_tft.h"

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
};

static struct k210_tft tft;

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
    // tft.info.framebuffer    = rt_malloc(tft.info.width * tft.info.height * tft.info.bits_per_pixel / 8);

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
    tft_fill_data(&data, tft.info.width * tft.info.height /2);
}
