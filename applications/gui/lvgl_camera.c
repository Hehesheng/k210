#include <lvgl.h>
#include <stdlib.h>
#include "drv_ov2640.h"
#include "lvgl_tools.h"

#define LOG_TAG "lv.cam"
#include <ulog.h>

static lv_img_dsc_t buff = {0};
static lv_obj_t* img;
static lv_task_t *task;

static void camera_task(lv_task_t* task)
{
    buff.data = (uint8_t*)camera_get_buff();
    lv_img_set_src(img, &buff);
}

static void camera_img_cb(lv_obj_t* img, lv_event_t event)
{
    if (event == LV_EVENT_DELETE)
    {
        lv_task_del(task);
    }
}

void lvgl_camera_press_cb(lv_obj_t* btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        buff.header.cf = LV_IMG_CF_TRUE_COLOR;
        buff.header.w  = LV_HOR_RES_MAX;
        buff.header.h  = LV_VER_RES_MAX;
        buff.data      = (uint8_t*)camera_get_buff();
        buff.data_size = LV_HOR_RES_MAX * LV_VER_RES_MAX * 2;

        img = lv_img_create(lv_scr_act(), NULL);

        lv_obj_set_size(img, LV_HOR_RES_MAX, LV_VER_RES_MAX);
        lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

        lv_img_set_src(img, &buff);

        lv_obj_set_event_cb(img, camera_img_cb);
        task = lv_task_create(camera_task, 50, LV_TASK_PRIO_HIGH, NULL);

        /* 添加活动对象 */
        lv_add_activity_obj(img);
    }
}
LVGL_APP_ITEM_EXPORT(Camera, LV_SYMBOL_VIDEO, lvgl_camera_press_cb);
