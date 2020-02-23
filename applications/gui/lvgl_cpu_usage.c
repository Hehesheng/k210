#include <lvgl.h>
#include "cpu_usage.h"
#include "lvgl_tools.h"

typedef struct list_btn_info
{
    lv_obj_t *btn;
    lv_obj_t *label[2];
    lv_obj_t *bar[2];
    lv_obj_t *chart;
    lv_obj_t *chart_label;
    lv_chart_series_t *cpu_ser[2];
    void *user_data;
} list_btn_info_t;

static lv_task_t *task;

/* 定时刷新 */
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

            lv_label_set_text_fmt(btn_info->label[0], "%3d%%", data->major[0]);
            lv_label_set_text_fmt(btn_info->label[1], "%3d%%", data->major[1]);

            if (btn_info->chart != NULL)
            {
                lv_label_set_text_fmt(btn_info->chart_label, "%s %2d.%02d%% %2d.%02d%%", tid->name, data->major[0],
                                      data->minor[0], data->major[1], data->minor[1]);
                lv_chart_set_next(btn_info->chart, btn_info->cpu_ser[0], data->major[0]);
                lv_chart_set_next(btn_info->chart, btn_info->cpu_ser[1], data->major[1]);
            }
        }
    }
}

static void chart_close_btn_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *cont = lv_find_obj_parent_by_type(btn, "lv_cont");

        list_btn_info_t *btn_info = (list_btn_info_t *)btn->user_data;

        btn_info->chart       = NULL;
        btn_info->chart_label = NULL;
        btn_info->cpu_ser[0]  = NULL;
        btn_info->cpu_ser[1]  = NULL;
        lv_obj_del(cont);
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
    else if (event == LV_EVENT_CLICKED)
    {
        list_btn_info_t *btn_info = (list_btn_info_t *)btn->user_data;
        thread_usage_t data       = (thread_usage_t)btn_info->user_data;

        lv_obj_t *page = lv_find_obj_parent_by_type(btn, "lv_page");
        lv_obj_t *cont = lv_cont_create(page, NULL);
        lv_cont_set_layout(cont, LV_LAYOUT_COL_M);
        lv_obj_set_size(cont, lv_obj_get_width(page) - LV_DPI, lv_obj_get_height(page) - LV_DPI);
        lv_obj_align(cont, page, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *chart = lv_chart_create(cont, NULL);
        lv_obj_set_size(chart, lv_obj_get_width(cont), lv_obj_get_height(cont) - LV_DPI * 5 / 4);
        lv_obj_align(chart, cont, LV_ALIGN_IN_TOP_MID, 0, 0);
        lv_chart_set_type(chart, LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE);
        lv_chart_set_point_count(chart, 10);
        lv_chart_set_series_opa(chart, LV_OPA_70);
        lv_chart_set_series_width(chart, 4);
        lv_chart_set_range(chart, 0, 100);

        lv_obj_t *label = lv_label_create(cont, NULL);
        lv_label_set_text_fmt(label, "NULL %2d.%02d%% %2d.%02d%%", data->major[0], data->minor[0], data->major[1],
                              data->minor[1]);

        btn_info->chart       = chart;
        btn_info->chart_label = label;
        btn_info->cpu_ser[0]  = lv_chart_add_series(chart, LV_COLOR_RED);
        btn_info->cpu_ser[1]  = lv_chart_add_series(chart, LV_COLOR_GREEN);

        lv_obj_t *close = lv_btn_create(cont, NULL);
        lv_label_set_text(lv_label_create(close, NULL), "close");
        lv_obj_set_event_cb(close, chart_close_btn_cb);
        close->user_data = btn_info;
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
            rt_memset(btn_info, 0, sizeof(list_btn_info_t));
            btn_info->btn = lv_list_add_btn(list, NULL, tid->name);
            lv_btn_set_layout(btn_info->btn, LV_LAYOUT_PRETTY);

            btn_info->label[0] = lv_label_create(btn_info->btn, NULL);
            lv_label_set_text(btn_info->label[0], "  0%");
            btn_info->bar[0] = lv_bar_create(btn_info->btn, NULL);

            btn_info->label[1] = lv_label_create(btn_info->btn, NULL);
            lv_label_set_text(btn_info->label[1], "  0%");
            btn_info->bar[1] = lv_bar_create(btn_info->btn, NULL);

            lv_obj_set_event_cb(btn_info->btn, cpu_list_btn_cb);
            data->user_data          = btn_info;
            btn_info->btn->user_data = btn_info;
            btn_info->user_data      = data;
        }
    }

    task = lv_task_create(refresh_cpu_task, 1000, LV_TASK_PRIO_MID, NULL);
}

static void close_press_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        lv_task_del(task);
        lv_obj_del(lv_find_obj_parent_by_type(btn, "lv_page"));
    }
}

void lvgl_cpu_usage_press_cb(lv_obj_t *btn, lv_event_t event)
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
LVGL_APP_ITEM_EXPORT(CPU, LV_SYMBOL_LIST, lvgl_cpu_usage_press_cb);
