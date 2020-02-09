#include <stdio.h>
#include <stdlib.h>

#include <sysctl.h>

#include "touchscreen.h"

#define LOG_TAG "drv.touch"
#include <ulog.h>

static struct rt_device touch;
static const int calibration[7] = {-20, -5806, 21542188, 4056, 66, -361856, 65536};

static void touch_thread(void *param);

static rt_err_t drv_touchscreen_init(rt_device_t dev)
{
    touchscreen_config_t config;
    rt_thread_t tid;
    int ret;

    rt_kprintf("calibration:[");
    for (int i = 0; i < 7; i++)
    {
        config.calibration[i] = calibration[i];
        rt_kprintf("%d,", calibration[i]);
    }
    rt_kprintf("]\n");

    ret = touchscreen_init((void *)&config);
    if (ret != 0) return -1;

    // tid = rt_thread_create("touch", touch_thread, RT_NULL, 4096, 20, 10);
    // rt_thread_startup(tid);

    return 0;
}

static int drv_touchscreen_deinit(void)
{
    int ret = touchscreen_deinit();
    if (ret != 0) return -1;
    return 0;
}

static rt_size_t drv_touchscreen_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_size_t i;
    int status, x, y;
    int *ptr = (int *)buffer;

    for (i = 0; i < size; i++)
    {
        touchscreen_read(&ptr[3 * i], &ptr[3 * i + 1], &ptr[3 * i + 2]);
    }

    return i;
}

static void touch_thread(void *param)
{
    int status, x, y;
    int data[3] = {0};

    while (1)
    {
        drv_touchscreen_read(&touch, 0, data, 1);
        status = data[0];
        x      = data[1];
        y      = data[2];
        if (status != TOUCHSCREEN_STATUS_IDLE && status != TOUCHSCREEN_STATUS_RELEASE)
        {
            rt_kprintf("status: %d, x: %d, y: %d\n", status, x, y);
        }
        rt_thread_mdelay(30);
    }
}

static int touch_register(void)
{
    rt_err_t ret;

    touch.type    = RT_Device_Class_Touch;
    touch.init    = drv_touchscreen_init;
    touch.open    = RT_NULL;
    touch.close   = RT_NULL;
    touch.read    = drv_touchscreen_read;
    touch.write   = RT_NULL;
    touch.control = RT_NULL;

    rt_device_register(&touch, "touch", RT_DEVICE_FLAG_RDONLY);

    return 0;
}
INIT_DEVICE_EXPORT(touch_register);

static void touch_run(void)
{
    if (touch.flag & RT_DEVICE_FLAG_ACTIVATED) return;
    rt_device_init(&touch);
}
MSH_CMD_EXPORT(touch_run, init touchscreen);
