#include "lvgl_tools.h"

struct __lvgl_obj_base
{
    char *name;
    char *text;
};

extern const int __lvgl_apps_tab_start;
extern const int __lvgl_apps_tab_end;
static const void *lvgl_apps_start_point = &__lvgl_apps_tab_start;
static const void *lvgl_apps_end_point   = &__lvgl_apps_tab_end;

static void lvgl_tab_list(void)
{
    struct __lvgl_obj_base *obj;
    struct __lvgl_obj_base **point;

    rt_kprintf("start: 0x%X\n", lvgl_apps_start_point);
    rt_kprintf("end: 0x%X\n", lvgl_apps_end_point);

    for (point = (struct __lvgl_obj_base **)lvgl_apps_start_point; point < (struct __lvgl_obj_base **)lvgl_apps_end_point;
         point++)
    {
        obj = *point;

        rt_kprintf("point:0x%X\n", point);
        rt_kprintf("obj:0x%X\n", obj);
        rt_kprintf("name: %s\n", obj->name);
    }
}
MSH_CMD_EXPORT(lvgl_tab_list, list lvgl tab obj);

lv_obj_t *lv_find_obj_parent_by_type(lv_obj_t *obj, char *name)
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
