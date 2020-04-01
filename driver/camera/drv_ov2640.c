#include "drv_ov2640.h"
#include <stdio.h>
#include <unistd.h>
#include "dvp.h"
#include "ov2640.h"
#include "plic.h"

#define LOG_TAG "ov2640"
#include <ulog.h>

#define CAMERA_SIZE (320 * 240)

static uint32_t g_camera_gram0[CAMERA_SIZE / 2] __attribute__((aligned(64)));
static uint32_t g_camera_gram1[CAMERA_SIZE / 2] __attribute__((aligned(64)));

volatile uint8_t g_ram_mux;
volatile uint8_t g_dvp_finish_flag;

uint16_t *camera_get_buff(void)
{
    uint16_t *ptr;

    while (g_dvp_finish_flag == 0)
    {
        rt_thread_delay(1);
    }
    g_dvp_finish_flag = 0;
    /* display pic*/
    g_ram_mux ^= 0x01;
    ptr = (uint16_t *)(g_ram_mux ? g_camera_gram0 : g_camera_gram1);

    return ptr;
}

static int on_irq_dvp(void *ctx)
{
    if (dvp_get_interrupt(DVP_STS_FRAME_FINISH))
    {
        /* switch gram */
        dvp_set_display_addr(g_ram_mux ? (uint32_t)g_camera_gram0 : (uint32_t)g_camera_gram1);

        // dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
        dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
        g_dvp_finish_flag = 1;
    }
    else
    {
        if (g_dvp_finish_flag == 0) dvp_start_convert();
        dvp_clear_interrupt(DVP_STS_FRAME_START);
    }

    return 0;
}

static int hw_ov2640_init(void)
{
    dvp_init(8);
    dvp_set_xclk_rate(24000000);
    dvp_enable_burst();
    dvp_set_output_enable(0, 1);
    dvp_set_output_enable(1, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    dvp_set_image_size(320, 240);
    ov2640_init();

    dvp_set_ai_addr((uint32_t)0x40600000, (uint32_t)0x40612C00, (uint32_t)0x40625800);
    dvp_set_display_addr((uint32_t)g_camera_gram0);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_disable_auto();

    /* DVP interrupt config */
    printf("DVP interrupt config\r\n");
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, on_irq_dvp, NULL);
    plic_irq_enable(IRQN_DVP_INTERRUPT);

    g_ram_mux         = 0;
    g_dvp_finish_flag = 0;
    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

    return 0;
}
INIT_DEVICE_EXPORT(hw_ov2640_init);
