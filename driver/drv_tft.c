#include <fpioa.h>
#include <gpiohs.h>
#include <rtthread.h>
#include <spi.h>

/* clang-format off */
#define NO_OPERATION            0x00
#define SOFTWARE_RESET          0x01
// #define READ_ID                 0x04
#define READ_STATUS             0x09
#define READ_POWER_MODE         0x0A
#define READ_MADCTL             0x0B
#define READ_PIXEL_FORMAT       0x0C
#define READ_IMAGE_FORMAT       0x0D
#define READ_SIGNAL_MODE        0x0E
#define READ_SELT_DIAG_RESULT   0x0F
#define SLEEP_ON                0x10
#define SLEEP_OFF               0x11
#define PARTIAL_DISPALY_ON      0x12
#define NORMAL_DISPALY_ON       0x13
#define INVERSION_DISPALY_OFF   0x20
#define INVERSION_DISPALY_ON    0x21
#define GAMMA_SET               0x26
#define DISPALY_OFF             0x28
#define DISPALY_ON              0x29
#define HORIZONTAL_ADDRESS_SET  0x2A
#define VERTICAL_ADDRESS_SET    0x2B
#define MEMORY_WRITE            0x2C
#define COLOR_SET               0x2D
#define MEMORY_READ             0x2E
#define PARTIAL_AREA            0x30
#define VERTICAL_SCROL_DEFINE   0x33
#define TEAR_EFFECT_LINE_OFF    0x34
#define TEAR_EFFECT_LINE_ON     0x35
#define MEMORY_ACCESS_CTL       0x36
#define VERTICAL_SCROL_S_ADD    0x37
#define IDLE_MODE_OFF           0x38
#define IDLE_MODE_ON            0x39
#define PIXEL_FORMAT_SET        0x3A
#define WRITE_MEMORY_CONTINUE   0x3C
#define READ_MEMORY_CONTINUE    0x3E
#define SET_TEAR_SCANLINE       0x44
#define GET_SCANLINE            0x45
#define WRITE_BRIGHTNESS        0x51
#define READ_BRIGHTNESS         0x52
#define WRITE_CTRL_DISPALY      0x53
#define READ_CTRL_DISPALY       0x54
#define WRITE_BRIGHTNESS_CTL    0x55
#define READ_BRIGHTNESS_CTL     0x56
#define WRITE_MIN_BRIGHTNESS    0x5E
#define READ_MIN_BRIGHTNESS     0x5F
#define READ_ID1                0xDA
#define READ_ID2                0xDB
#define READ_ID3                0xDC
#define RGB_IF_SIGNAL_CTL       0xB0
#define NORMAL_FRAME_CTL        0xB1
#define IDLE_FRAME_CTL          0xB2
#define PARTIAL_FRAME_CTL       0xB3
#define INVERSION_CTL           0xB4
#define BLANK_PORCH_CTL         0xB5
#define DISPALY_FUNCTION_CTL    0xB6
#define ENTRY_MODE_SET          0xB7
#define BACKLIGHT_CTL1          0xB8
#define BACKLIGHT_CTL2          0xB9
#define BACKLIGHT_CTL3          0xBA
#define BACKLIGHT_CTL4          0xBB
#define BACKLIGHT_CTL5          0xBC
#define BACKLIGHT_CTL7          0xBE
#define BACKLIGHT_CTL8          0xBF
#define POWER_CTL1              0xC0
#define POWER_CTL2              0xC1
#define VCOM_CTL1               0xC5
#define VCOM_CTL2               0xC7
#define NV_MEMORY_WRITE         0xD0
#define NV_MEMORY_PROTECT_KEY   0xD1
#define NV_MEMORY_STATUS_READ   0xD2
#define READ_ID4                0xD3
#define POSITIVE_GAMMA_CORRECT  0xE0
#define NEGATIVE_GAMMA_CORRECT  0xE1
#define DIGITAL_GAMMA_CTL1      0xE2
#define DIGITAL_GAMMA_CTL2      0xE3
#define INTERFACE_CTL           0xF6

struct tft_hw
{
    spi_device_num_t spi_channel;
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
    gpiohs_set_drive_mode(tft.hw.dc, GPIO_DM_OUTPUT);
    gpiohs_set_pin(tft.hw.dc, GPIO_PV_HIGH);
}

static void set_dcx_control(void)
{
    gpiohs_set_pin(tft.hw.dc, GPIO_PV_LOW);
}

static void set_dcx_data(void)
{
    gpiohs_set_pin(tft.hw.dc, GPIO_PV_HIGH);
}

static void init_rst(void)
{
    gpiohs_set_drive_mode(tft.hw.rst, GPIO_DM_OUTPUT);
    gpiohs_set_pin(tft.hw.rst, GPIO_PV_HIGH);
}

static void set_rst(uint8_t val)
{
    gpiohs_set_pin(tft.hw.rst, val);
}

void tft_set_clk_freq(uint32_t freq)
{
    spi_set_clk_rate(tft.hw.spi_channel, freq);
}

rt_err_t tft_hw_init(rt_device_t dev)
{    
    init_dcx();
    init_rst();

    spi_init(tft.hw.spi_channel, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    
    tft_set_clk_freq(15000000);
    rt_thread_mdelay(50);
    set_rst(1);
    rt_thread_mdelay(50);

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
    tft.info.framebuffer    = rt_malloc(tft.info.width * tft.info.height * tft.info.bits_per_pixel / 8);

    tft.parent.user_data = (void *)&tft.info;

    tft.hw = (struct tft_hw){
        .spi_channel = 0,
        .dma_channel = DMAC_CHANNEL1,
        .cs = 36,
        .dc = 38,
        .clk = 39,
        .rst = 37,
    };

    ret = rt_device_register(&tft.parent, "tft", RT_DEVICE_FLAG_RDWR);

    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_tft_init);
