#include <lvgl.h>
#include "lvgl_tools.h"

static lv_style_t style_kb;
static lv_style_t style_kb_rel;
static lv_style_t style_kb_pr;

static void set_kb_style(lv_style_t *normal, lv_style_t *pressed, lv_style_t *released)
{
    lv_style_copy(normal, &lv_style_plain);

    normal->body.opa            = LV_OPA_70;
    normal->body.main_color     = lv_color_hex3(0x333);
    normal->body.grad_color     = lv_color_hex3(0x333);
    normal->body.padding.left   = 0;
    normal->body.padding.right  = 0;
    normal->body.padding.top    = 0;
    normal->body.padding.bottom = 0;
    normal->body.padding.inner  = 0;

    lv_style_copy(pressed, &lv_style_plain);
    pressed->body.radius       = 0;
    pressed->body.opa          = LV_OPA_50;
    pressed->body.main_color   = LV_COLOR_WHITE;
    pressed->body.grad_color   = LV_COLOR_WHITE;
    pressed->body.border.width = 1;
    pressed->body.border.color = LV_COLOR_SILVER;

    lv_style_copy(released, &lv_style_plain);
    released->body.opa          = LV_OPA_TRANSP;
    released->body.radius       = 0;
    released->body.border.width = 1;
    released->body.border.color = LV_COLOR_SILVER;
    released->body.border.opa   = LV_OPA_50;
    released->body.main_color   = lv_color_hex3(0x333); /*Recommended if LV_VDB_SIZE == 0 and bpp > 1 fonts are used*/
    released->body.grad_color   = lv_color_hex3(0x333);
    released->text.color        = LV_COLOR_WHITE;
}

static void anim_set_text_area_size(lv_obj_t *ta, lv_coord_t height) { lv_obj_set_height(ta, height); }

void lvgl_terminal_press_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *interface    = lv_find_obj_parent_by_type(btn, "lv_cont");
        lv_coord_t interface_w = lv_obj_get_width(interface);
        lv_coord_t interface_h = lv_obj_get_height(interface);
        /* page 初始化 */
        lv_obj_t *page = lv_page_create(interface, NULL);
        lv_page_set_style(page, LV_LIST_STYLE_BG, &lv_style_transp_fit);
        lv_page_set_sb_mode(page, LV_SB_MODE_DRAG);

        lv_obj_set_size(page, interface_w, interface_h);
        lv_obj_align(page, interface, LV_ALIGN_CENTER, 0, 0);
        /* 创建 Text area */
        lv_obj_t *ta = lv_ta_create(page, NULL);
        lv_obj_set_size(ta, interface_w, interface_h / 2);
        lv_obj_align(ta, page, LV_ALIGN_IN_TOP_MID, 0, 0);
        lv_ta_set_text(ta, "");
        lv_ta_set_text_sel(ta, true);
        /* 创建键盘 */
        lv_obj_t *kb = lv_kb_create(page, NULL);
        lv_obj_set_size(kb, interface_w, interface_h / 2);
        lv_obj_align(kb, ta, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        lv_kb_set_ta(kb, ta);
        set_kb_style(&style_kb, &style_kb_pr, &style_kb_rel);
        lv_kb_set_style(kb, LV_KB_STYLE_BG, &style_kb);
        lv_kb_set_style(kb, LV_KB_STYLE_BTN_REL, &style_kb_rel);
        lv_kb_set_style(kb, LV_KB_STYLE_BTN_PR, &style_kb_pr);
        // lv_obj_set_event_cb(kb, lv_kb_def_event_cb);

        lv_anim_t a;
        /* 键盘动画 */
        a.var            = kb;
        a.start          = LV_VER_RES;
        a.end            = lv_obj_get_y(kb);
        a.exec_cb        = (lv_anim_exec_xcb_t)lv_obj_set_y;
        a.path_cb        = lv_anim_path_linear;
        a.ready_cb       = NULL;
        a.act_time       = 0;
        a.time           = 300;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        lv_anim_create(&a);
        /* 文本框动画 */
        a.var            = ta;
        a.start          = interface_h;
        a.end            = interface_h / 2;
        a.exec_cb        = (lv_anim_exec_xcb_t)anim_set_text_area_size;
        a.path_cb        = lv_anim_path_linear;
        a.ready_cb       = NULL;
        a.act_time       = 0;
        a.time           = 300;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        lv_anim_create(&a);
        /* 添加活动对象 */
        lv_add_activity_obj(page);
    }
}
LVGL_APP_ITEM_EXPORT(Terminal, LV_SYMBOL_WARNING, lvgl_terminal_press_cb);
