#ifndef __CPU_USAGE_H__
#define __CPU_USAGE_H__

#include <rtthread.h>

#define MAGIC_NUM 0x6e6e56e8

typedef struct thread_usage
{
    rt_uint32_t magic;
    rt_uint64_t enter_tick[2];
    rt_uint64_t leave_tick[2];
    rt_uint64_t count_tick[2];
    rt_uint64_t cost_tick[2];
    rt_uint8_t major[2];
    rt_uint8_t minor[2];
    void* user_data;
} * thread_usage_t;

#endif  // __CPU_USAGE_H__
