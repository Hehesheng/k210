#include "lvgl_tools.h"

struct __lvgl_obj_base
{
    char *name;
    char *text;
};

typedef struct __lvgl_activity_obj_list
{
    lv_obj_t *obj;
    rt_list_t list;
} lvgl_activity_t;
lvgl_activity_t header = {
    .obj  = NULL,
    .list = RT_LIST_OBJECT_INIT(header.list),
};

extern const int __lvgl_apps_tab_start;
extern const int __lvgl_apps_tab_end;
const void *lvgl_obj_start_point = &__lvgl_apps_tab_start;
const void *lvgl_obj_end_point   = &__lvgl_apps_tab_end;

static void lvgl_tab_list(void)
{
    struct __lvgl_obj_base *obj;
    struct __lvgl_obj_base **point;

    rt_kprintf("start: 0x%X\n", lvgl_obj_start_point);
    rt_kprintf("end: 0x%X\n", lvgl_obj_end_point);

    for (point = (struct __lvgl_obj_base **)lvgl_obj_start_point; point < (struct __lvgl_obj_base **)lvgl_obj_end_point;
         point++)
    {
        obj = *point;

        rt_kprintf("point:0x%X\n", point);
        rt_kprintf("obj:0x%X\n", obj);
        rt_kprintf("name: %s\n", obj->name);
    }
}
MSH_CMD_EXPORT(lvgl_tab_list, list lvgl tab obj);

/**
 * @brief  根据类型名向对象父类寻找对象
 * @param  obj  子类
 * @param  name 要找的父类名
 * @return 找到的父类
 */
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

/**
 * @brief  添加一个活动对象至链表中
 * @param  obj 想要添加的对象
 * @return 成功返回0，否则返回-1
 */
int lv_add_activity_obj(lv_obj_t *obj)
{
    int ret = 0;
    lvgl_activity_t *node;

    node = rt_malloc(sizeof(lvgl_activity_t));
    if (node == NULL)
    {
        return -1;
    }
    node->obj = obj;
    rt_list_insert_after(&header.list, &node->list);

    return 0;
}

/**
 * @brief  将对象从活动链表中移除
 * @return 现在活动的对象
 */
lv_obj_t *lv_remove_activity_obj(void)
{
    lvgl_activity_t *act;
    lv_obj_t *ret;

    if (rt_list_isempty(&header.list))
    {
        return NULL;
    }
    act = rt_list_first_entry(&header.list, lvgl_activity_t, list);
    rt_list_remove(&act->list);
    ret = act->obj;
    rt_free(act);

    return ret;
}

/**
 * @brief  返回当前活动的对象
 * @return 活动对象
 */
lv_obj_t *lv_get_activity_obj(void)
{
    if (rt_list_isempty(&header.list))
    {
        return NULL;
    }
    return rt_list_first_entry(&header.list, lvgl_activity_t, list)->obj;
}
