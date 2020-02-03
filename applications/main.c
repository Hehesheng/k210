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

#define CPU_USAGE_CALC_TICK 100

typedef struct cpu_usage
{
    rt_uint64_t total_count;
    rt_uint64_t count;
    rt_uint8_t cpu_usage_major;
    rt_uint8_t cpu_usage_minor;
} cpu_usage;
static cpu_usage cpus[2] = {0};

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

static void cpu_usage_idle_hook()
{
    rt_tick_t tick;
    rt_uint8_t core = current_coreid();

    if (cpus[core].total_count == 0)
    {
        /* get total count */
        rt_enter_critical();
        tick = rt_tick_get();
        while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
        {
            cpus[core].total_count++;
        }
        rt_exit_critical();
    }

    cpus[core].count = 0;
    /* get CPU usage */
    tick = rt_tick_get();
    while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
    {
        cpus[core].count++;
    }

    /* calculate major and minor */
    if (cpus[core].count < cpus[core].total_count)
    {
        cpus[core].count           = cpus[core].total_count - cpus[core].count;
        cpus[core].cpu_usage_major = (cpus[core].count * 100) / cpus[core].total_count;
        cpus[core].cpu_usage_minor = ((cpus[core].count * 100) % cpus[core].total_count) * 100 / cpus[core].total_count;
    }
    else
    {
        cpus[core].total_count = cpus[core].count;

        /* no CPU usage */
        cpus[core].cpu_usage_major = 0;
        cpus[core].cpu_usage_minor = 0;
    }
}

static void get_cpu_usage(void)
{
    rt_kprintf("cpu  usage     total\n");
    rt_kprintf("---  ------  ---------\n");
    for (int core = 0; core < 2; core++)
    {
        rt_kprintf(" %d   %02d.%02d%%  %8d\n", core, cpus[core].cpu_usage_major, cpus[core].cpu_usage_minor,
                   cpus[core].total_count);
    }
}
MSH_CMD_EXPORT(get_cpu_usage, get cpu usage);

static int cpu_usage_init(void)
{
    /* set idle thread hook */
    rt_thread_idle_sethook(cpu_usage_idle_hook);

    return 0;
}
INIT_PREV_EXPORT(cpu_usage_init);
