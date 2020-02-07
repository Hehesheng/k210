#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <sysctl.h>
#include <timer.h>

#define LOG_TAG "app.hook"
#include <ulog.h>

#define HOOK_MAGIC_NUM 0x6e6e56e8

#define CPU_USAGE_CALC_TICK 100
#define CPU_USAGE_LOOP 100

typedef struct cpu_usage
{
    rt_uint32_t total_count;
    rt_uint32_t count;
    rt_uint8_t cpu_usage_major;
    rt_uint8_t cpu_usage_minor;
} cpu_usage;
static cpu_usage cpus[2] = {0};

typedef struct thread_usage
{
    rt_uint32_t magic;
    rt_uint32_t enter_tick;
    rt_uint32_t leave_tick;
    rt_uint32_t count_tick;
    rt_uint32_t cost_tick;
} * thread_usage_t;

static void cpu_usage_idle_hook(void)
{
    rt_tick_t tick;
    rt_uint8_t core = rt_hw_cpu_id();
    volatile rt_uint32_t loop;

    if (cpus[core].total_count == 0)
    {
        /* get total count */
        rt_enter_critical();
        tick = rt_tick_get();
        while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
        {
            cpus[core].total_count++;
            loop = 0;
            while (loop < CPU_USAGE_LOOP) loop++;
        }
        rt_exit_critical();
    }

    cpus[core].count = 0;
    /* get CPU usage */
    tick = rt_tick_get();
    while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
    {
        cpus[core].count++;
        loop = 0;
        while (loop < CPU_USAGE_LOOP) loop++;
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
    struct rt_object_information* info = rt_object_get_information(RT_Object_Class_Thread);
    thread_usage_t data;
    rt_thread_t tid;
    rt_list_t* node;
    rt_tick_t cost;

    rt_kprintf("cpu  usage    total\n");
    rt_kprintf("---  ------  ------\n");
    for (int core = 0; core < 2; core++)
    {
        rt_kprintf(" %d   %2d.%02d%%  %6d\n", core, cpus[core].cpu_usage_major, cpus[core].cpu_usage_minor,
                   cpus[core].total_count);
    }
    rt_kprintf("%-*.s  cost\n", RT_NAME_MAX, "thread");
    for (int i = 0; i < RT_NAME_MAX; i++) rt_kprintf("-");
    rt_kprintf(" ------\n");
    for (node = info->object_list.next; node != &(info->object_list); node = node->next)
    {
        tid  = rt_list_entry(node, struct rt_thread, list);
        data = (thread_usage_t)tid->user_data;
        if (data != RT_NULL && data->magic == HOOK_MAGIC_NUM)
        {
            rt_kprintf("%-*.*s %6d\n", RT_NAME_MAX, RT_NAME_MAX, tid->name, data->cost_tick);
        }
        else
        {
            rt_kprintf("%-*.*s   NULL\n", RT_NAME_MAX, RT_NAME_MAX, tid->name);
        }
    }
}
// MSH_CMD_EXPORT(get_cpu_usage, get cpu usage);

static int cpu_usage_init(void)
{
    /* set idle thread hook */
    rt_thread_idle_sethook(cpu_usage_idle_hook);

    return 0;
}
// INIT_PREV_EXPORT(cpu_usage_init);

static void clean_up_call(rt_thread_t tid)
{
    thread_usage_t info;

    if (tid != RT_NULL && tid->user_data != RT_NULL)
    {
        info = (thread_usage_t)tid->user_data;
        if (info->magic == HOOK_MAGIC_NUM)
        {
            rt_free(info);
        }
    }
}

static void schedule_hook(rt_thread_t from, rt_thread_t to)
{
    thread_usage_t from_info, to_info;
    rt_tick_t tick;

    tick = rt_tick_get();
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
            from_info->magic      = HOOK_MAGIC_NUM;
            from_info->enter_tick = tick;
            from->user_data       = from_info;
            from->cleanup         = clean_up_call;
        }
    }
    /* user data update */
    if (from_info->magic == HOOK_MAGIC_NUM)
    {
        from_info->leave_tick = tick;
        from_info->count_tick += from_info->leave_tick - from_info->enter_tick;
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
            to_info->magic = HOOK_MAGIC_NUM;
            to->user_data  = to_info;
            to->cleanup    = clean_up_call;
        }
    }
    /* user data update */
    if (to_info->magic == HOOK_MAGIC_NUM)
    {
        to_info->enter_tick = tick;
    }
}

static void update_per_second(void* parm)
{
    struct rt_object_information* info = (struct rt_object_information*)parm;
    thread_usage_t data;
    rt_thread_t tid;
    rt_list_t* node;

    while (1)
    {
        for (node = info->object_list.next; node != &(info->object_list); node = node->next)
        {
            tid  = rt_list_entry(node, struct rt_thread, list);
            data = (thread_usage_t)tid->user_data;
            if (data != RT_NULL && data->magic == HOOK_MAGIC_NUM)
            {
                data->cost_tick  = data->count_tick;
                data->count_tick = 0;
            }
        }
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}

static int init_schedule_hook(void)
{
    static struct rt_object_information* info;
    rt_thread_t tid;

    info = rt_object_get_information(RT_Object_Class_Thread);
    tid  = rt_thread_create("t_cpu", update_per_second, info, 2048, 0, 5);
    if (tid == RT_NULL)
    {
        return -1;
    }
    rt_thread_startup(tid);

    rt_scheduler_sethook(schedule_hook);

    return 0;
}
// INIT_APP_EXPORT(init_schedule_hook);
