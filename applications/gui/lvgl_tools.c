#include "lvgl_tools.h"

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
