/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/09/30     Bernard      The first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <stdio.h>
#include <msh.h>

int main(void)
{
    rt_kprintf("Hello, world\n");

    rt_thread_delay(RT_TICK_PER_SECOND * 5);

    while (1)
    {
        // msh_exec("list_sem", sizeof("list_sem"));
        rt_thread_delay(RT_TICK_PER_SECOND * 3);
    }
    

    return 0;
}

static void thread(void* parm)
{
    rt_device_t serial, lcd;
    char ch = 'A';
    int count, loop = 0, ret;

    serial = rt_device_find("uarths");
    lcd = rt_device_find("lcd");
    rt_device_init(lcd);
    // ret = rt_device_control(serial, RT_DEVICE_CTRL_SET_INT, RT_NULL);

    while (1)
    {
        // count = rt_device_read(serial, 0, &ch, 1);
        // if (count != 0)
        // {
        //     rt_kprintf("%c", ch);
        // }
        rt_thread_delay(10);
        loop++;
        if (loop % 300 == 0)
        {
            rt_kprintf("loop over:%d, ret:%d, A:%c\n", loop, ret, ch);
        }
    }
    
}

static int test_init(void)
{
    rt_thread_t tid;

    tid = (rt_thread_t)rt_object_find("tshell", RT_Object_Class_Thread);
    rt_thread_control(tid, RT_THREAD_CTRL_BIND_CPU, (void*)0);

    tid = rt_thread_create("test", thread, RT_NULL, 4096, 5, 100);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
// INIT_APP_EXPORT(test_init);
