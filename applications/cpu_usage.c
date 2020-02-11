#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu_usage.h"

#include <sysctl.h>
#include <timer.h>

#define LOG_TAG "app.cpu.usage"
#include <ulog.h>

#define TIMER_NUM TIMER_DEVICE_0
#define TIMER_CHANNEL TIMER_CHANNEL_0

#define get_timer_value() \
    ((timer_tick + 1) * timer_reload_num - (rt_uint64_t)timer[TIMER_NUM]->channel[TIMER_CHANNEL].current_value)
#define get_count_time(t, core) (t->leave_tick[core] - t->enter_tick[core])

static rt_uint64_t timer_reload_num;
static volatile rt_uint64_t timer_tick;

static void clean_up_call(rt_thread_t tid)
{
    thread_usage_t info;

    if (tid != RT_NULL && tid->user_data != RT_NULL)
    {
        info = (thread_usage_t)tid->user_data;
        if (info->magic == MAGIC_NUM)
        {
            rt_free(info);
        }
    }
}

static void schedule_hook(rt_thread_t from, rt_thread_t to)
{
    thread_usage_t from_info, to_info;
    int core = rt_hw_cpu_id();
    rt_uint64_t tick;

    tick = get_timer_value();
    /* check from data illegal */
    from_info = (thread_usage_t)from->user_data;
    if (from_info == RT_NULL && from->cleanup == RT_NULL)
    {
        from_info = rt_malloc(sizeof(struct thread_usage));
        if (from_info == RT_NULL)
        {
            log_w("No memory");
        }
        else
        {
            from_info->magic            = MAGIC_NUM;
            from_info->enter_tick[core] = tick;
            from->user_data             = from_info;
            from->cleanup               = clean_up_call;
        }
    }
    /* user data update */
    if (from_info->magic == MAGIC_NUM)
    {
        from_info->leave_tick[core] = tick;
        from_info->count_tick[core] += get_count_time(from_info, core);
    }

    /* check to data illegal */
    to_info = (thread_usage_t)to->user_data;
    if (to_info == RT_NULL && to->cleanup == RT_NULL)
    {
        to_info = rt_malloc(sizeof(struct thread_usage));
        if (to_info == RT_NULL)
        {
            log_w("No memory");
        }
        else
        {
            to_info->magic = MAGIC_NUM;
            to->user_data  = to_info;
            to->cleanup    = clean_up_call;
        }
    }
    /* user data update */
    if (to_info->magic == MAGIC_NUM)
    {
        to_info->enter_tick[core] = tick;
    }
}

static int timer_irq(void* param)
{
    struct rt_object_information* info = (struct rt_object_information*)param;
    thread_usage_t data;
    rt_thread_t tid;
    rt_list_t* node;
    rt_uint64_t tick;

    timer_tick++;

    tick = get_timer_value();
    for (node = info->object_list.next; node != &(info->object_list); node = node->next)
    {
        tid  = rt_list_entry(node, struct rt_thread, list);
        data = (thread_usage_t)tid->user_data;
        if (data == RT_NULL || data->magic != MAGIC_NUM)
        {
            continue;
        }
        for (int i = 0; i < 2; i++)
        {
            if (data->enter_tick[i] > data->leave_tick[i])
            {
                data->leave_tick[i] = tick;
                data->count_tick[i] += get_count_time(data, i);
                data->enter_tick[i] = tick;
            }
            data->cost_tick[i]  = data->count_tick[i];
            data->count_tick[i] = 0;

            if (data->cost_tick[i] <= timer_reload_num)
            {
                data->major[i] = data->cost_tick[i] * 100 / timer_reload_num;
                data->minor[i] = (data->cost_tick[i] * 100 % timer_reload_num) * 100 / timer_reload_num;
            }
            else
            {
                data->major[i] = 100;
                data->minor[i] = 0;
            }
        }
    }

    return 0;
}

static int init_schedule_hook(void)
{
    static struct rt_object_information* info;
    rt_thread_t tid;

    info = rt_object_get_information(RT_Object_Class_Thread);
    timer_init(TIMER_NUM);
    timer_set_interval(TIMER_NUM, TIMER_CHANNEL, 1000000000);
    timer_reload_num = timer[TIMER_DEVICE_0]->channel[TIMER_CHANNEL_0].load_count;
    timer_irq_register(TIMER_NUM, TIMER_CHANNEL, 0, 1, timer_irq, info);
    timer_set_enable(TIMER_NUM, TIMER_CHANNEL, 1);

    rt_scheduler_sethook(schedule_hook);

    return 0;
}
INIT_PREV_EXPORT(init_schedule_hook);

static void get_cpu_usage(void)
{
    struct rt_object_information* info = rt_object_get_information(RT_Object_Class_Thread);
    thread_usage_t data;
    rt_thread_t tid;
    rt_list_t* node;

    rt_kprintf("%-*.s  cpu0    cpu1\n", RT_NAME_MAX, "thread");
    for (int i = 0; i < RT_NAME_MAX; i++) rt_kprintf("-");
    rt_kprintf(" ------- -------\n");
    for (node = info->object_list.next; node != &(info->object_list); node = node->next)
    {
        tid  = rt_list_entry(node, struct rt_thread, list);
        data = (thread_usage_t)tid->user_data;
        if (data != RT_NULL && data->magic == MAGIC_NUM)
        {
            rt_kprintf("%-*.*s %3d.%02d%% %3d.%02d%%\n", RT_NAME_MAX, RT_NAME_MAX, tid->name, data->major[0], data->minor[0],
                       data->major[1], data->minor[1]);
        }
        else
        {
            rt_kprintf("%-*.*s  0.00%%  0.00%%\n", RT_NAME_MAX, RT_NAME_MAX, tid->name);
        }
    }
}
MSH_CMD_EXPORT(get_cpu_usage, get cpu usage);
