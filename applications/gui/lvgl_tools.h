#ifndef __LVGL_TOOLS_H__
#define __LVGL_TOOLS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

#define LV_KB_CTRL_BTN_FLAGS (LV_BTNM_CTRL_NO_REPEAT | LV_BTNM_CTRL_CLICK_TRIG)

static const char * kb_map_lc[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                   "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                   "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                   LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""};

static const lv_btnm_ctrl_t kb_ctrl_lc_map[] = {
    LV_KB_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_uc[] = {"1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
                                   "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
                                   "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                   LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, "Tab", ""};

static const lv_btnm_ctrl_t kb_ctrl_uc_map[] = {
    LV_KB_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_spec[] = {"0", "1", "2", "3", "4" ,"5", "6", "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
                                     "abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                     "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                     LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, "Tab", ""};

static const lv_btnm_ctrl_t kb_ctrl_spec_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
    LV_KB_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2};

extern const void *lvgl_obj_start_point;
extern const void *lvgl_obj_end_point;

#define LVGL_LIST_ALL_EXPORT_OBJ(obj) \
    __typeof(obj) *_p;                \
    for (_p = (__typeof(obj) *)lvgl_obj_start_point, obj = *_p; _p < (__typeof(obj) *)lvgl_obj_end_point; _p++, obj = *_p)

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
};

#define LVGL_WIDGET_ITEM_EXPORT(text, widget)                                                               \
    const char __lvgl_widget_item_##text##_##widget##_name[] SECTION(".rodata.lvgl") = "__widget_" #text;   \
    const char __lvgl_widget_item_##text##_##widget##_text[] SECTION(".rodata.lvgl") = #text;               \
                                                                                                            \
    const struct __lvgl_widget_item __lvgl_widget_item_##text##_##widget##_obj SECTION(".rodata.lvgl") = {  \
        __lvgl_widget_item_##text##_##widget##_name, __lvgl_widget_item_##text##_##widget##_text, &widget}; \
    RT_USED const void *__lvgl_widget_##text##_##widget##_point SECTION("LVObjTab") =                       \
        &__lvgl_widget_item_##text##_##widget##_obj;

lv_obj_t *lv_find_obj_parent_by_type(lv_obj_t *obj, char *name);

int lv_add_activity_obj(lv_obj_t *obj);
lv_obj_t *lv_remove_activity_obj(void);
lv_obj_t *lv_get_activity_obj(void);

#ifdef __cplusplus
}
#endif

#endif  // __LVGL_TOOLS_H__
