#include <lvgl.h>
#include "lvgl_tools.h"

void lvgl_terminal_press_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *interface = lv_find_obj_parent_by_type(btn, "lv_cont");
        /* page 初始化 */
        lv_obj_t *page = lv_page_create(interface, NULL);
        lv_page_set_style(page, LV_LIST_STYLE_BG, &lv_style_transp_fit);
        lv_page_set_sb_mode(page, LV_SB_MODE_DRAG);

        lv_obj_set_size(page, lv_obj_get_width(interface), lv_obj_get_height(interface));
        lv_obj_align(page, interface, LV_ALIGN_CENTER, 0, 0);
        /* 创建 Text area */
        lv_obj_t *ta = lv_btn_create(page, NULL);
        lv_obj_set_size(ta, lv_obj_get_width(interface), lv_obj_get_height(interface));
        lv_obj_align(ta, page, LV_ALIGN_IN_TOP_MID, 0, 0);
    }
}
LVGL_APP_ITEM_EXPORT(Terminal, LV_SYMBOL_WARNING, lvgl_terminal_press_cb);
