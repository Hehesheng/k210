#ifndef __LVGL_TOOLS_H__
#define __LVGL_TOOLS_H__

#include <lvgl.h>

struct __lvgl_app_item
{
    const char *name;
    const char *text;
    const void *img;
    lv_event_cb_t cb;
};

#define LVGL_APP_ITEM_EXPORT(text, img, cb)                                                       \
    const char __lvgl_app_item_##text##_##img##_name[] SECTION(".rodata.lvgl") = "__app_" #text;  \
    const char __lvgl_app_item_##text##_##img##_text[] SECTION(".rodata.lvgl") = #text;           \
                                                                                                  \
    const struct __lvgl_app_item __lvgl_app_item_##text##_##img##_obj SECTION(".rodata.lvgl") = { \
        __lvgl_app_item_##text##_##img##_name, __lvgl_app_item_##text##_##img##_text, img, cb};   \
    RT_USED const void *__lvgl_app_##text##_##img##_point SECTION("LVObjTab") = &__lvgl_app_item_##text##_##img##_obj;

struct __lvgl_widget_item
{
    const char *name;
    const char *text;
    lv_obj_t **obj;
    lv_event_cb_t cb;
};

#define LVGL_WIDGET_ITEM_EXPORT(text, widget, cb)                                                          \
    const char __lvgl_widget_item_##text##_##widget##_name[] SECTION(".rodata.name") = "__widget_" #text;  \
    const char __lvgl_widget_item_##text##_##widget##_text[] SECTION(".rodata.name") = #text;              \
    lv_obj_t *__lvgl_widget_item_##text##_##widget##_widget                          = widget;             \
                                                                                                           \
    const struct __lvgl_widget_item __lvgl_widget_item_##text##_##widget##_obj SECTION(".rodata.name") = { \
        __lvgl_widget_item_##text##_##widget##_name, __lvgl_widget_item_##text##_##widget##_text,          \
        &__lvgl_widget_item_##text##_##widget##_widget, cb};                                               \
    RT_USED const void *__lvgl_widget_##text##_##widget##_point SECTION("LVObjTab") =                      \
        &__lvgl_widget_item_##text##_##widget##_obj;

lv_obj_t *lv_find_obj_parent_by_type(lv_obj_t *obj, char *name);

#endif  // __LVGL_TOOLS_H__
