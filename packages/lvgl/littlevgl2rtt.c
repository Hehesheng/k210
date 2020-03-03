#include "littlevgl2rtt.h"
#include "lvgl.h"
#include "touchscreen.h"

#define LVGL_BUFFER_SIZE (LV_HOR_RES_MAX * LV_VER_RES_MAX / 10)

static struct rt_device_graphic_info *info;
static uint32_t *framebuffer;
static lv_disp_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_color_t lv_buffer[LVGL_BUFFER_SIZE] = {0};

static bool input_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    static rt_bool_t touch_down = RT_FALSE;

    int status = TOUCHSCREEN_STATUS_IDLE;
    int x      = 0;
    int y      = 0;

    touchscreen_read(&status, &x, &y);
    if (status == TOUCHSCREEN_STATUS_PRESS)
    {
        touch_down = RT_TRUE;
    }
    else if (status == TOUCHSCREEN_STATUS_RELEASE)
    {
        touch_down = RT_FALSE;
    }

    data->point.x = x;
    data->point.y = y;
    data->state   = (touch_down == RT_TRUE) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    return false;
}

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x;
    int32_t y;

    for (y = area->y1; y <= area->y2; y++)
    {
        for (x = area->x1; x <= area->x2; x++)
        {
            framebuffer[x + info->width * y] = (uint32_t)color_p->full;
            color_p++;
        }
    }

    lv_disp_flush_ready(disp_drv);
}

static void lvgl_tick_run(void *p)
{
    while (1)
    {
        lv_tick_inc(20);
        rt_thread_delay(20);
    }
}

static void lvgl_handle_thread(void *p)
{
    rt_tick_t t = RT_TICK_PER_SECOND / 60;
    rt_tick_t start, end;
    uint32_t fps = 0;

    while (1)
    {
        start = rt_tick_get();
        lv_task_handler();
        rt_thread_delay(t);
        end = rt_tick_get();
        fps = RT_TICK_PER_SECOND * 100 / (end - start);
        /* keep 30fps */
        if (fps > 3000)
            t++;
        else
            t--;
        if (t > RT_TICK_PER_SECOND) t = 0;
    }
}

#if USE_LV_LOG
void littlevgl2rtt_log_register(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{
    if (level >= LV_LOG_LEVEL)
    {
        // Show the log level if you want
        if (level == LV_LOG_LEVEL_TRACE)
        {
            rt_kprintf("Trace:");
        }

        rt_kprintf("%s\n", dsc);
        // You can write 'file' and 'line' too similary if required.
    }
}
#endif

static int littlevgl2rtt_init(void)
{
    rt_device_t device;
    /* LCD Device Init */
    device = rt_device_find("tft");
    RT_ASSERT(device != RT_NULL);
    if (rt_device_init(device) != RT_EOK)
    {
        return -1;
    }
    info = (struct rt_device_graphic_info *)device->user_data;

    if ((info->bits_per_pixel != LV_COLOR_DEPTH) && (info->bits_per_pixel != 32 && LV_COLOR_DEPTH != 24))
    {
        rt_kprintf("Error: framebuffer color depth mismatch! (Should be %d to match with LV_COLOR_DEPTH)",
                   info->bits_per_pixel);
        return RT_ERROR;
    }
    framebuffer = (uint32_t *)info->framebuffer;

    device = rt_device_find("touch");
    RT_ASSERT(device != RT_NULL);
    if (rt_device_init(device) != RT_EOK)
    {
        return -2;
    }

    /* littlevgl Init */
    lv_init();

#if USE_LV_LOG
    /* littlevgl Log Init */
    lv_log_register_print(littlevgl2rtt_log_register);
#endif

    /* littlevGL Display device interface */
    lv_disp_buf_init(&disp_buf, lv_buffer, NULL, LVGL_BUFFER_SIZE);
    lv_disp_drv_init(&disp_drv);

    disp_drv.flush_cb = disp_flush;
    disp_drv.buffer   = &disp_buf;

    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);

    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = input_read;

    lv_indev_drv_register(&indev_drv);

    /* littlevGL Tick thread */
    rt_thread_t thread = RT_NULL;

    thread = rt_thread_create("lv_tick", lvgl_tick_run, RT_NULL, 1024, 6, 10);
    if (thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    thread = rt_thread_create("lvgl", lvgl_handle_thread, RT_NULL, 8192, 10, 100);
    if (thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    return 0;
}
INIT_ENV_EXPORT(littlevgl2rtt_init);
