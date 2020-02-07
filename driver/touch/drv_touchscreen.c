#include <stdio.h>
#include <stdlib.h>

#include <sysctl.h>

#include "touchscreen.h"

#define LOG_TAG "drv.touch"
#include <ulog.h>

static int drv_touchscreen_init(void)
{
    touchscreen_config_t config;
    int ret;
    int calibration[7] = {-20, -5806, 21542188, 4056, 66, -361856, 65536};

    for (int i = 0; i < 7; i++)
    {
        config.calibration[i] = calibration[i];
    }

    ret = touchscreen_init((void *)&config);
    if (ret != 0) return -1;
    return 0;
}
INIT_DEVICE_EXPORT(drv_touchscreen_init);

static int drv_touchscreen_deinit(void)
{
    int ret = touchscreen_deinit();
    if (ret != 0) return -1;
    return 0;
}

static int drv_touchscreen_read(int *status, int *x, int *y)
{
    int ret;

    ret = touchscreen_read(status, x, y);

    return ret;
}

static void touch_test(void)
{
    int status, x, y;
    int ret;

    ret = drv_touchscreen_read(&status, &x, &y);
    if (ret)
    {
        rt_kprintf("touch read error: %d\n", ret);
    }
    rt_kprintf("status: %d, x: %d, y: %d\n", status, x, y);
}
MSH_CMD_EXPORT(touch_test, touch test);

static void touch_thread(void *param)
{
    int status, x, y;
    int ret;

    while (1)
    {
        ret = drv_touchscreen_read(&status, &x, &y);
        if (ret)
        {
            rt_kprintf("touch read error: %d\n", ret);
        }
        if (status != TOUCHSCREEN_STATUS_IDLE && status != TOUCHSCREEN_STATUS_RELEASE)
        {
            rt_kprintf("status: %d, x: %d, y: %d\n", status, x, y);
        }
        rt_thread_mdelay(30);
    }
}

static int _test(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("touch", touch_thread, RT_NULL, 4096, 20, 10);
    rt_thread_startup(tid);

    return 0;
}
INIT_APP_EXPORT(_test);
