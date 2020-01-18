#include <rtdevice.h>
#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <fpioa.h>
#include <gpio.h>
#include <gpiohs.h>
#include "drv_tft.h"

#define LOG_TAG "app.cmd"
#include <ulog.h>

const char pin_func_name[][20] = {
    "FUNC_JTAG_TCLK     ", /*!< JTAG Test Clock */
    "FUNC_JTAG_TDI      ", /*!< JTAG Test Data In */
    "FUNC_JTAG_TMS      ", /*!< JTAG Test Mode Select */
    "FUNC_JTAG_TDO      ", /*!< JTAG Test Data Out */
    "FUNC_SPI0_D0       ", /*!< SPI0 Data 0 */
    "FUNC_SPI0_D1       ", /*!< SPI0 Data 1 */
    "FUNC_SPI0_D2       ", /*!< SPI0 Data 2 */
    "FUNC_SPI0_D3       ", /*!< SPI0 Data 3 */
    "FUNC_SPI0_D4       ", /*!< SPI0 Data 4 */
    "FUNC_SPI0_D5       ", /*!< SPI0 Data 5 */
    "FUNC_SPI0_D6       ", /*!< SPI0 Data 6 */
    "FUNC_SPI0_D7       ", /*!< SPI0 Data 7 */
    "FUNC_SPI0_SS0      ", /*!< SPI0 Chip Select 0 */
    "FUNC_SPI0_SS1      ", /*!< SPI0 Chip Select 1 */
    "FUNC_SPI0_SS2      ", /*!< SPI0 Chip Select 2 */
    "FUNC_SPI0_SS3      ", /*!< SPI0 Chip Select 3 */
    "FUNC_SPI0_ARB      ", /*!< SPI0 Arbitration */
    "FUNC_SPI0_SCLK     ", /*!< SPI0 Serial Clock */
    "FUNC_UARTHS_RX     ", /*!< UART High speed Receiver */
    "FUNC_UARTHS_TX     ", /*!< UART High speed Transmitter */
    "FUNC_RESV6         ", /*!< Reserved function */
    "FUNC_RESV7         ", /*!< Reserved function */
    "FUNC_CLK_SPI1      ", /*!< Clock SPI1 */
    "FUNC_CLK_I2C1      ", /*!< Clock I2C1 */
    "FUNC_GPIOHS0       ", /*!< GPIO High speed 0 */
    "FUNC_GPIOHS1       ", /*!< GPIO High speed 1 */
    "FUNC_GPIOHS2       ", /*!< GPIO High speed 2 */
    "FUNC_GPIOHS3       ", /*!< GPIO High speed 3 */
    "FUNC_GPIOHS4       ", /*!< GPIO High speed 4 */
    "FUNC_GPIOHS5       ", /*!< GPIO High speed 5 */
    "FUNC_GPIOHS6       ", /*!< GPIO High speed 6 */
    "FUNC_GPIOHS7       ", /*!< GPIO High speed 7 */
    "FUNC_GPIOHS8       ", /*!< GPIO High speed 8 */
    "FUNC_GPIOHS9       ", /*!< GPIO High speed 9 */
    "FUNC_GPIOHS10      ", /*!< GPIO High speed 10 */
    "FUNC_GPIOHS11      ", /*!< GPIO High speed 11 */
    "FUNC_GPIOHS12      ", /*!< GPIO High speed 12 */
    "FUNC_GPIOHS13      ", /*!< GPIO High speed 13 */
    "FUNC_GPIOHS14      ", /*!< GPIO High speed 14 */
    "FUNC_GPIOHS15      ", /*!< GPIO High speed 15 */
    "FUNC_GPIOHS16      ", /*!< GPIO High speed 16 */
    "FUNC_GPIOHS17      ", /*!< GPIO High speed 17 */
    "FUNC_GPIOHS18      ", /*!< GPIO High speed 18 */
    "FUNC_GPIOHS19      ", /*!< GPIO High speed 19 */
    "FUNC_GPIOHS20      ", /*!< GPIO High speed 20 */
    "FUNC_GPIOHS21      ", /*!< GPIO High speed 21 */
    "FUNC_GPIOHS22      ", /*!< GPIO High speed 22 */
    "FUNC_GPIOHS23      ", /*!< GPIO High speed 23 */
    "FUNC_GPIOHS24      ", /*!< GPIO High speed 24 */
    "FUNC_GPIOHS25      ", /*!< GPIO High speed 25 */
    "FUNC_GPIOHS26      ", /*!< GPIO High speed 26 */
    "FUNC_GPIOHS27      ", /*!< GPIO High speed 27 */
    "FUNC_GPIOHS28      ", /*!< GPIO High speed 28 */
    "FUNC_GPIOHS29      ", /*!< GPIO High speed 29 */
    "FUNC_GPIOHS30      ", /*!< GPIO High speed 30 */
    "FUNC_GPIOHS31      ", /*!< GPIO High speed 31 */
    "FUNC_GPIO0         ", /*!< GPIO pin 0 */
    "FUNC_GPIO1         ", /*!< GPIO pin 1 */
    "FUNC_GPIO2         ", /*!< GPIO pin 2 */
    "FUNC_GPIO3         ", /*!< GPIO pin 3 */
    "FUNC_GPIO4         ", /*!< GPIO pin 4 */
    "FUNC_GPIO5         ", /*!< GPIO pin 5 */
    "FUNC_GPIO6         ", /*!< GPIO pin 6 */
    "FUNC_GPIO7         ", /*!< GPIO pin 7 */
    "FUNC_UART1_RX      ", /*!< UART1 Receiver */
    "FUNC_UART1_TX      ", /*!< UART1 Transmitter */
    "FUNC_UART2_RX      ", /*!< UART2 Receiver */
    "FUNC_UART2_TX      ", /*!< UART2 Transmitter */
    "FUNC_UART3_RX      ", /*!< UART3 Receiver */
    "FUNC_UART3_TX      ", /*!< UART3 Transmitter */
    "FUNC_SPI1_D0       ", /*!< SPI1 Data 0 */
    "FUNC_SPI1_D1       ", /*!< SPI1 Data 1 */
    "FUNC_SPI1_D2       ", /*!< SPI1 Data 2 */
    "FUNC_SPI1_D3       ", /*!< SPI1 Data 3 */
    "FUNC_SPI1_D4       ", /*!< SPI1 Data 4 */
    "FUNC_SPI1_D5       ", /*!< SPI1 Data 5 */
    "FUNC_SPI1_D6       ", /*!< SPI1 Data 6 */
    "FUNC_SPI1_D7       ", /*!< SPI1 Data 7 */
    "FUNC_SPI1_SS0      ", /*!< SPI1 Chip Select 0 */
    "FUNC_SPI1_SS1      ", /*!< SPI1 Chip Select 1 */
    "FUNC_SPI1_SS2      ", /*!< SPI1 Chip Select 2 */
    "FUNC_SPI1_SS3      ", /*!< SPI1 Chip Select 3 */
    "FUNC_SPI1_ARB      ", /*!< SPI1 Arbitration */
    "FUNC_SPI1_SCLK     ", /*!< SPI1 Serial Clock */
    "FUNC_SPI_SLAVE_D0  ", /*!< SPI Slave Data 0 */
    "FUNC_SPI_SLAVE_SS  ", /*!< SPI Slave Select */
    "FUNC_SPI_SLAVE_SCLK", /*!< SPI Slave Serial Clock */
    "FUNC_I2S0_MCLK     ", /*!< I2S0 Master Clock */
    "FUNC_I2S0_SCLK     ", /*!< I2S0 Serial Clock(BCLK) */
    "FUNC_I2S0_WS       ", /*!< I2S0 Word Select(LRCLK) */
    "FUNC_I2S0_IN_D0    ", /*!< I2S0 Serial Data Input 0 */
    "FUNC_I2S0_IN_D1    ", /*!< I2S0 Serial Data Input 1 */
    "FUNC_I2S0_IN_D2    ", /*!< I2S0 Serial Data Input 2 */
    "FUNC_I2S0_IN_D3    ", /*!< I2S0 Serial Data Input 3 */
    "FUNC_I2S0_OUT_D0   ", /*!< I2S0 Serial Data Output 0 */
    "FUNC_I2S0_OUT_D1   ", /*!< I2S0 Serial Data Output 1 */
    "FUNC_I2S0_OUT_D2   ", /*!< I2S0 Serial Data Output 2 */
    "FUNC_I2S0_OUT_D3   ", /*!< I2S0 Serial Data Output 3 */
    "FUNC_I2S1_MCLK     ", /*!< I2S1 Master Clock */
    "FUNC_I2S1_SCLK     ", /*!< I2S1 Serial Clock(BCLK) */
    "FUNC_I2S1_WS       ", /*!< I2S1 Word Select(LRCLK) */
    "FUNC_I2S1_IN_D0    ", /*!< I2S1 Serial Data Input 0 */
    "FUNC_I2S1_IN_D1    ", /*!< I2S1 Serial Data Input 1 */
    "FUNC_I2S1_IN_D2    ", /*!< I2S1 Serial Data Input 2 */
    "FUNC_I2S1_IN_D3    ", /*!< I2S1 Serial Data Input 3 */
    "FUNC_I2S1_OUT_D0   ", /*!< I2S1 Serial Data Output 0 */
    "FUNC_I2S1_OUT_D1   ", /*!< I2S1 Serial Data Output 1 */
    "FUNC_I2S1_OUT_D2   ", /*!< I2S1 Serial Data Output 2 */
    "FUNC_I2S1_OUT_D3   ", /*!< I2S1 Serial Data Output 3 */
    "FUNC_I2S2_MCLK     ", /*!< I2S2 Master Clock */
    "FUNC_I2S2_SCLK     ", /*!< I2S2 Serial Clock(BCLK) */
    "FUNC_I2S2_WS       ", /*!< I2S2 Word Select(LRCLK) */
    "FUNC_I2S2_IN_D0    ", /*!< I2S2 Serial Data Input 0 */
    "FUNC_I2S2_IN_D1    ", /*!< I2S2 Serial Data Input 1 */
    "FUNC_I2S2_IN_D2    ", /*!< I2S2 Serial Data Input 2 */
    "FUNC_I2S2_IN_D3    ", /*!< I2S2 Serial Data Input 3 */
    "FUNC_I2S2_OUT_D0   ", /*!< I2S2 Serial Data Output 0 */
    "FUNC_I2S2_OUT_D1   ", /*!< I2S2 Serial Data Output 1 */
    "FUNC_I2S2_OUT_D2   ", /*!< I2S2 Serial Data Output 2 */
    "FUNC_I2S2_OUT_D3   ", /*!< I2S2 Serial Data Output 3 */
    "FUNC_RESV0         ", /*!< Reserved function */
    "FUNC_RESV1         ", /*!< Reserved function */
    "FUNC_RESV2         ", /*!< Reserved function */
    "FUNC_RESV3         ", /*!< Reserved function */
    "FUNC_RESV4         ", /*!< Reserved function */
    "FUNC_RESV5         ", /*!< Reserved function */
    "FUNC_I2C0_SCLK     ", /*!< I2C0 Serial Clock */
    "FUNC_I2C0_SDA      ", /*!< I2C0 Serial Data */
    "FUNC_I2C1_SCLK     ", /*!< I2C1 Serial Clock */
    "FUNC_I2C1_SDA      ", /*!< I2C1 Serial Data */
    "FUNC_I2C2_SCLK     ", /*!< I2C2 Serial Clock */
    "FUNC_I2C2_SDA      ", /*!< I2C2 Serial Data */
    "FUNC_CMOS_XCLK     ", /*!< DVP System Clock */
    "FUNC_CMOS_RST      ", /*!< DVP System Reset */
    "FUNC_CMOS_PWDN     ", /*!< DVP Power Down Mode */
    "FUNC_CMOS_VSYNC    ", /*!< DVP Vertical Sync */
    "FUNC_CMOS_HREF     ", /*!< DVP Horizontal Reference output */
    "FUNC_CMOS_PCLK     ", /*!< Pixel Clock */
    "FUNC_CMOS_D0       ", /*!< Data Bit 0 */
    "FUNC_CMOS_D1       ", /*!< Data Bit 1 */
    "FUNC_CMOS_D2       ", /*!< Data Bit 2 */
    "FUNC_CMOS_D3       ", /*!< Data Bit 3 */
    "FUNC_CMOS_D4       ", /*!< Data Bit 4 */
    "FUNC_CMOS_D5       ", /*!< Data Bit 5 */
    "FUNC_CMOS_D6       ", /*!< Data Bit 6 */
    "FUNC_CMOS_D7       ", /*!< Data Bit 7 */
    "FUNC_SCCB_SCLK     ", /*!< SCCB Serial Clock */
    "FUNC_SCCB_SDA      ", /*!< SCCB Serial Data */
    "FUNC_UART1_CTS     ", /*!< UART1 Clear To Send */
    "FUNC_UART1_DSR     ", /*!< UART1 Data Set Ready */
    "FUNC_UART1_DCD     ", /*!< UART1 Data Carrier Detect */
    "FUNC_UART1_RI      ", /*!< UART1 Ring Indicator */
    "FUNC_UART1_SIR_IN  ", /*!< UART1 Serial Infrared Input */
    "FUNC_UART1_DTR     ", /*!< UART1 Data Terminal Ready */
    "FUNC_UART1_RTS     ", /*!< UART1 Request To Send */
    "FUNC_UART1_OUT2    ", /*!< UART1 User-designated Output 2 */
    "FUNC_UART1_OUT1    ", /*!< UART1 User-designated Output 1 */
    "FUNC_UART1_SIR_OUT ", /*!< UART1 Serial Infrared Output */
    "FUNC_UART1_BAUD    ", /*!< UART1 Transmit Clock Output */
    "FUNC_UART1_RE      ", /*!< UART1 Receiver Output Enable */
    "FUNC_UART1_DE      ", /*!< UART1 Driver Output Enable */
    "FUNC_UART1_RS485_EN", /*!< UART1 RS485 Enable */
    "FUNC_UART2_CTS     ", /*!< UART2 Clear To Send */
    "FUNC_UART2_DSR     ", /*!< UART2 Data Set Ready */
    "FUNC_UART2_DCD     ", /*!< UART2 Data Carrier Detect */
    "FUNC_UART2_RI      ", /*!< UART2 Ring Indicator */
    "FUNC_UART2_SIR_IN  ", /*!< UART2 Serial Infrared Input */
    "FUNC_UART2_DTR     ", /*!< UART2 Data Terminal Ready */
    "FUNC_UART2_RTS     ", /*!< UART2 Request To Send */
    "FUNC_UART2_OUT2    ", /*!< UART2 User-designated Output 2 */
    "FUNC_UART2_OUT1    ", /*!< UART2 User-designated Output 1 */
    "FUNC_UART2_SIR_OUT ", /*!< UART2 Serial Infrared Output */
    "FUNC_UART2_BAUD    ", /*!< UART2 Transmit Clock Output */
    "FUNC_UART2_RE      ", /*!< UART2 Receiver Output Enable */
    "FUNC_UART2_DE      ", /*!< UART2 Driver Output Enable */
    "FUNC_UART2_RS485_EN", /*!< UART2 RS485 Enable */
    "FUNC_UART3_CTS     ", /*!< UART3 Clear To Send */
    "FUNC_UART3_DSR     ", /*!< UART3 Data Set Ready */
    "FUNC_UART3_DCD     ", /*!< UART3 Data Carrier Detect */
    "FUNC_UART3_RI      ", /*!< UART3 Ring Indicator */
    "FUNC_UART3_SIR_IN  ", /*!< UART3 Serial Infrared Input */
    "FUNC_UART3_DTR     ", /*!< UART3 Data Terminal Ready */
    "FUNC_UART3_RTS     ", /*!< UART3 Request To Send */
    "FUNC_UART3_OUT2    ", /*!< UART3 User-designated Output 2 */
    "FUNC_UART3_OUT1    ", /*!< UART3 User-designated Output 1 */
    "FUNC_UART3_SIR_OUT ", /*!< UART3 Serial Infrared Output */
    "FUNC_UART3_BAUD    ", /*!< UART3 Transmit Clock Output */
    "FUNC_UART3_RE      ", /*!< UART3 Receiver Output Enable */
    "FUNC_UART3_DE      ", /*!< UART3 Driver Output Enable */
    "FUNC_UART3_RS485_EN", /*!< UART3 RS485 Enable */
    "FUNC_TIMER0_TOGGLE1", /*!< TIMER0 Toggle Output 1 */
    "FUNC_TIMER0_TOGGLE2", /*!< TIMER0 Toggle Output 2 */
    "FUNC_TIMER0_TOGGLE3", /*!< TIMER0 Toggle Output 3 */
    "FUNC_TIMER0_TOGGLE4", /*!< TIMER0 Toggle Output 4 */
    "FUNC_TIMER1_TOGGLE1", /*!< TIMER1 Toggle Output 1 */
    "FUNC_TIMER1_TOGGLE2", /*!< TIMER1 Toggle Output 2 */
    "FUNC_TIMER1_TOGGLE3", /*!< TIMER1 Toggle Output 3 */
    "FUNC_TIMER1_TOGGLE4", /*!< TIMER1 Toggle Output 4 */
    "FUNC_TIMER2_TOGGLE1", /*!< TIMER2 Toggle Output 1 */
    "FUNC_TIMER2_TOGGLE2", /*!< TIMER2 Toggle Output 2 */
    "FUNC_TIMER2_TOGGLE3", /*!< TIMER2 Toggle Output 3 */
    "FUNC_TIMER2_TOGGLE4", /*!< TIMER2 Toggle Output 4 */
    "FUNC_CLK_SPI2      ", /*!< Clock SPI2 */
    "FUNC_CLK_I2C2      ", /*!< Clock I2C2 */
    "FUNC_INTERNAL0     ", /*!< Internal function signal 0 */
    "FUNC_INTERNAL1     ", /*!< Internal function signal 1 */
    "FUNC_INTERNAL2     ", /*!< Internal function signal 2 */
    "FUNC_INTERNAL3     ", /*!< Internal function signal 3 */
    "FUNC_INTERNAL4     ", /*!< Internal function signal 4 */
    "FUNC_INTERNAL5     ", /*!< Internal function signal 5 */
    "FUNC_INTERNAL6     ", /*!< Internal function signal 6 */
    "FUNC_INTERNAL7     ", /*!< Internal function signal 7 */
    "FUNC_INTERNAL8     ", /*!< Internal function signal 8 */
    "FUNC_INTERNAL9     ", /*!< Internal function signal 9 */
    "FUNC_INTERNAL10    ", /*!< Internal function signal 10 */
    "FUNC_INTERNAL11    ", /*!< Internal function signal 11 */
    "FUNC_INTERNAL12    ", /*!< Internal function signal 12 */
    "FUNC_INTERNAL13    ", /*!< Internal function signal 13 */
    "FUNC_INTERNAL14    ", /*!< Internal function signal 14 */
    "FUNC_INTERNAL15    ", /*!< Internal function signal 15 */
    "FUNC_INTERNAL16    ", /*!< Internal function signal 16 */
    "FUNC_INTERNAL17    ", /*!< Internal function signal 17 */
    "FUNC_CONSTANT      ", /*!< Constant function */
    "FUNC_INTERNAL18    ", /*!< Internal function signal 18 */
    "FUNC_DEBUG0        ", /*!< Debug function 0 */
    "FUNC_DEBUG1        ", /*!< Debug function 1 */
    "FUNC_DEBUG2        ", /*!< Debug function 2 */
    "FUNC_DEBUG3        ", /*!< Debug function 3 */
    "FUNC_DEBUG4        ", /*!< Debug function 4 */
    "FUNC_DEBUG5        ", /*!< Debug function 5 */
    "FUNC_DEBUG6        ", /*!< Debug function 6 */
    "FUNC_DEBUG7        ", /*!< Debug function 7 */
    "FUNC_DEBUG8        ", /*!< Debug function 8 */
    "FUNC_DEBUG9        ", /*!< Debug function 9 */
    "FUNC_DEBUG10       ", /*!< Debug function 10 */
    "FUNC_DEBUG11       ", /*!< Debug function 11 */
    "FUNC_DEBUG12       ", /*!< Debug function 12 */
    "FUNC_DEBUG13       ", /*!< Debug function 13 */
    "FUNC_DEBUG14       ", /*!< Debug function 14 */
    "FUNC_DEBUG15       ", /*!< Debug function 15 */
    "FUNC_DEBUG16       ", /*!< Debug function 16 */
    "FUNC_DEBUG17       ", /*!< Debug function 17 */
    "FUNC_DEBUG18       ", /*!< Debug function 18 */
    "FUNC_DEBUG19       ", /*!< Debug function 19 */
    "FUNC_DEBUG20       ", /*!< Debug function 20 */
    "FUNC_DEBUG21       ", /*!< Debug function 21 */
    "FUNC_DEBUG22       ", /*!< Debug function 22 */
    "FUNC_DEBUG23       ", /*!< Debug function 23 */
    "FUNC_DEBUG24       ", /*!< Debug function 24 */
    "FUNC_DEBUG25       ", /*!< Debug function 25 */
    "FUNC_DEBUG26       ", /*!< Debug function 26 */
    "FUNC_DEBUG27       ", /*!< Debug function 27 */
    "FUNC_DEBUG28       ", /*!< Debug function 28 */
    "FUNC_DEBUG29       ", /*!< Debug function 29 */
    "FUNC_DEBUG30       ", /*!< Debug function 30 */
    "FUNC_DEBUG31       ", /*!< Debug function 31 */
};

static void get_pin_config(int argc, char** argv)
{
    fpioa_io_config_t config;

    rt_kprintf("┌─────┬──────┬────────────────────┐\n");
    rt_kprintf("│ pin │ func │        info        │\n");
    rt_kprintf("│-----│------│--------------------│\n");
    for (int i = 0; i < FPIOA_NUM_IO; i++)
    {
        fpioa_get_io(i, &config);
        rt_kprintf("│ %3d │ %4d │%20s│\n", i, config.ch_sel, pin_func_name[config.ch_sel]);
    }
    rt_kprintf("└─────┴──────┴────────────────────┘\n");
}
MSH_CMD_EXPORT(get_pin_config, get pin config);

static void tft_thread(void* param)
{
    uint32_t t = *((uint32_t*)param);
    int8_t r = 0, g = 0, b = 1;
    int8_t r_d = 0, g_d = 0, b_d = 1;
    uint16_t color = 0;
    rt_tick_t start, end;
    double fps;

    tft_clear(0);

    while (1)
    {
        start = rt_tick_get();
        b += b_d;
        g += g_d;
        r += r_d;

        if (b >= 31)
        {
            b_d = -1;
            g_d = 2;
            r_d = 0;
        }
        if (g >= 62)
        {
            b_d = 0;
            g_d = -2;
            r_d = 1;
        }
        if (r >= 31)
        {
            b_d = 1;
            g_d = 0;
            r_d = -1;
        }

        color = (((0x1F & r) << 11) | ((0x3F & g) << 5) | ((0x1F & b) << 0));
        tft_clear(color);
        rt_thread_mdelay(t);
        end = rt_tick_get();
        fps = 1000.0 / (end - start);
    }
}

static void try_tft(int argc, char** argv)
{
    rt_device_t tft;
    rt_thread_t tid;
    uint32_t param = 100;

    tft = rt_device_find("tft");
    if (tft)
    {
        if (tft->flag & RT_DEVICE_FLAG_ACTIVATED) return;
        rt_device_init(tft);
    }
    if (argc > 1) param = atoi(argv[1]);
    tid = rt_thread_create("tft", tft_thread, &param, 4096, 10, 50);
    rt_thread_startup(tid);
}
MSH_CMD_EXPORT(try_tft, tft test);
