/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/09/30     Bernard      The first version
 */

#include <rtdevice.h>
#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ulog.h>

#include <fpioa.h>
#include <gpio.h>
#include <gpiohs.h>

#define R_LED (6)
#define G_LED (5)
#define B_LED (4)

int main(void)
{
    gpio_init();

    gpio_set_drive_mode(R_LED, GPIO_DM_OUTPUT);
    fpioa_set_function(14, FUNC_GPIO0 + R_LED);
    gpio_set_drive_mode(G_LED, GPIO_DM_OUTPUT);
    fpioa_set_function(13, FUNC_GPIO0 + G_LED);
    gpio_set_drive_mode(B_LED, GPIO_DM_OUTPUT);
    fpioa_set_function(12, FUNC_GPIO0 + B_LED);

    while (1)
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                gpio_set_pin(R_LED - j, (i & (1 << j)) >> j);
            }
            rt_thread_delay(RT_TICK_PER_SECOND / 2);
        }
    }

    return 0;
}
