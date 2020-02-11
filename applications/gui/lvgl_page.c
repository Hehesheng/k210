#include "cpu_usage.h"
#include "lvgl.h"

typedef struct list_btn_info
{
    lv_obj_t *btn;
    lv_obj_t *label[2];
    lv_obj_t *bar[2];
} list_btn_info_t;

static lv_obj_t *parent;
static lv_obj_t *header;
static lv_task_t *task;

static lv_obj_t *lv_find_obj_parent_by_type(lv_obj_t *obj, char *name)
{
    lv_obj_t *ret = obj;
    lv_obj_type_t tmp;

    while (ret)
    {
        lv_obj_get_type(ret, &tmp);
        // for (int i = 0; tmp.type[i] != NULL; i++)
        // {
        //     rt_kprintf("%d:%s\n", i, tmp.type[i]);
        // }
        if (strcmp(tmp.type[0], name) == 0)
        {
            break;
        }
        ret = lv_obj_get_parent(ret);
    }

    return ret;
}

static void refresh_cpu_task(lv_task_t *task)
{
    struct rt_object_information *info = rt_object_get_information(RT_Object_Class_Thread);
    thread_usage_t data;
    rt_thread_t tid;
    rt_list_t *node;
    list_btn_info_t *btn_info;

    for (node = info->object_list.next; node != &(info->object_list); node = node->next)
    {
        tid  = rt_list_entry(node, struct rt_thread, list);
        data = (thread_usage_t)tid->user_data;
        if (data != RT_NULL && data->magic == MAGIC_NUM)
        {
            btn_info = (list_btn_info_t *)data->user_data;

            lv_bar_set_value(btn_info->bar[0], data->major[0], true);
            lv_bar_set_value(btn_info->bar[1], data->major[1], true);

            lv_label_set_text_fmt(btn_info->label[0],"%3d%%", data->major[0]);
            lv_label_set_text_fmt(btn_info->label[1],"%3d%%", data->major[1]);
        }
    }
}

static void cpu_list_btn_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_DELETE)
    {
        if (btn->user_data)
        {
            rt_free(btn->user_data);
        }
    }
}

static void add_thread_list(lv_obj_t *list)
{
    struct rt_object_information *info = rt_object_get_information(RT_Object_Class_Thread);
    thread_usage_t data;
    rt_thread_t tid;
    rt_list_t *node;
    list_btn_info_t *btn_info;

    for (node = info->object_list.next; node != &(info->object_list); node = node->next)
    {
        tid  = rt_list_entry(node, struct rt_thread, list);
        data = (thread_usage_t)tid->user_data;
        if (data != RT_NULL && data->magic == MAGIC_NUM)
        {
            btn_info = rt_malloc(sizeof(list_btn_info_t));
            btn_info->btn = lv_list_add_btn(list, NULL, tid->name);
            lv_btn_set_layout(btn_info->btn, LV_LAYOUT_PRETTY);

            btn_info->label[0] = lv_label_create(btn_info->btn, NULL);
            lv_label_set_text(btn_info->label[0], "  0%");
            btn_info->bar[0] = lv_bar_create(btn_info->btn, NULL);

            btn_info->label[1] = lv_label_create(btn_info->btn, NULL);
            lv_label_set_text(btn_info->label[1], "  0%");
            btn_info->bar[1] = lv_bar_create(btn_info->btn, NULL);

            lv_obj_set_event_cb(btn_info->btn, cpu_list_btn_cb);
            data->user_data = btn_info;
            btn_info->btn->user_data = btn_info;
        }
    }

    task = lv_task_create(refresh_cpu_task, 1000, LV_TASK_PRIO_MID, NULL);
}

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

static void close_press_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        lv_task_del(task);
        lv_obj_del(lv_find_obj_parent_by_type(btn, "lv_page"));
    }
}

static void cpu_press_cb(lv_obj_t *btn, lv_event_t event)
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
        /* 创建 list */
        lv_obj_t *list = lv_list_create(page, NULL);
        lv_list_set_style(list, LV_LIST_STYLE_BG, &lv_style_transp_fit);
        lv_list_set_sb_mode(list, LV_SB_MODE_DRAG);

        lv_obj_set_size(list, lv_obj_get_width(page), lv_obj_get_height(page));
        lv_obj_align(list, page, LV_ALIGN_CENTER, 0, 0);
        /* 创建关闭按钮 */
        lv_obj_t *close = lv_list_add_btn(list, LV_SYMBOL_LEFT, "close");
        lv_obj_set_event_cb(close, close_press_cb);
        /* 显示线程 */
        add_thread_list(list);
    }
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
    /* 创建各个按钮 */
    lv_obj_t *btn = lv_list_add_btn(list, LV_SYMBOL_LIST, "CPU");
    lv_obj_set_event_cb(btn, cpu_press_cb);
    lv_list_add_btn(list, LV_SYMBOL_WARNING, "Terminal");
    lv_list_add_btn(list, LV_SYMBOL_DRIVE, "Touchscreen");
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
