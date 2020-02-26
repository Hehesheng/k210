#include "lvgl.h"
#include "lvgl_tools.h"

static lv_obj_t *parent;
static lv_obj_t *header;
LVGL_WIDGET_ITEM_EXPORT(display_parent, parent);
LVGL_WIDGET_ITEM_EXPORT(display_header, header);

static void header_create(void)
{
    header = lv_cont_create(parent, NULL);
    lv_obj_set_width(header, lv_disp_get_hor_res(NULL));

    lv_obj_t *sym = lv_label_create(header, NULL);
    lv_label_set_text(sym, LV_SYMBOL_GPS LV_SYMBOL_WIFI LV_SYMBOL_BLUETOOTH LV_SYMBOL_VOLUME_MAX);
    lv_obj_align(sym, NULL, LV_ALIGN_IN_RIGHT_MID, -LV_DPI / 10, 0);

    lv_obj_t *name = lv_label_create(header, NULL);
    lv_label_set_text(name, "Hehesheng");
    lv_obj_align(name, NULL, LV_ALIGN_IN_LEFT_MID, LV_DPI / 10, 0);

    lv_cont_set_fit2(header, LV_FIT_NONE, LV_FIT_TIGHT); /*Let the height set automatically*/
    lv_obj_set_pos(header, 0, 0);
}

static void menu_list_create(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);
    /* cont 显示 */
    lv_obj_t *interface = lv_cont_create(parent, NULL);
    lv_obj_set_size(interface, hres, vres - lv_obj_get_height(header));
    lv_obj_align(interface, header, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    /* list 加载 */
    lv_obj_t *list = lv_list_create(interface, NULL);
    lv_list_set_style(list, LV_LIST_STYLE_BG, &lv_style_transp_fit);
    lv_list_set_sb_mode(list, LV_SB_MODE_DRAG);

    lv_obj_set_size(list, lv_obj_get_width(interface), lv_obj_get_height(interface));
    lv_obj_align(list, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    struct __lvgl_app_item *obj;
    LVGL_LIST_ALL_EXPORT_OBJ(obj)
    {
        if (strncmp(obj->name, "__app_", 6) == 0)
        {
            lv_obj_t *btn = lv_list_add_btn(list, obj->img, obj->text);
            lv_obj_set_event_cb(btn, obj->cb);
        }
    }
}

static void lvgl_startup_thread(void *p)
{
    rt_thread_t tid;

    while (1)
    {
        tid = (rt_thread_t)rt_object_find("lvgl", RT_Object_Class_Thread);
        if (tid)
        {
            break;
        }
        rt_thread_delay(RT_TICK_PER_SECOND / 10);
    }
    rt_thread_delay(RT_TICK_PER_SECOND / 10);

    /* set theme */
    lv_theme_t *th_act;
    th_act = lv_theme_night_init(180, NULL);
    lv_theme_set_current(th_act);

    parent = lv_obj_create(NULL, NULL);
    lv_disp_load_scr(parent);

    header_create();
    menu_list_create();
}

static int default_page_init(void)
{
    rt_thread_t tid = rt_thread_create("lvgl_init", lvgl_startup_thread, NULL, 4096, 20, 100);
    if (tid)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
INIT_APP_EXPORT(default_page_init);
