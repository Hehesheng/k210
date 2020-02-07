/*
 * File      : examples_dstr.c
 * This file is the example code of dstr(dynamic string) package
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-06-07     never        the first version
 */

#include <rtthread.h>
#include <finsh.h>
#include "dstr.h"

void rt_dstr_printf(rt_dstr_t *thiz)
{
    if (thiz == NULL)
        return;
    rt_kprintf("%s\n", thiz->str);
}

void dstr_test_new(void)
{
    rt_dstr_t *p = NULL;
    
    p = rt_dstr_new("new dstr");
    rt_dstr_printf(p);
    rt_dstr_del(p);
}

void dstr_test_cat(void)
{   
    rt_dstr_t *p = NULL;
    
    p = rt_dstr_new("cat");
    
    rt_dstr_cat(p, " dstr");
    
    rt_dstr_printf(p);
    
    rt_dstr_del(p);
}

void dstr_test_ncat(void)
{   
    rt_dstr_t *p1 = NULL;
    
    p1 = rt_dstr_new("ncat");
    
    rt_dstr_ncat(p1, " dstrnnn", 5);
    
    rt_dstr_ncat(p1, "1234", 3);
    
    rt_dstr_printf(p1);
    rt_kprintf("p2 str:%s\n",p1->str);
    
    rt_dstr_del(p1);
}

void dstr_test_cmp(void)
{
    rt_dstr_t *p1 = NULL;
    rt_dstr_t *p2 = NULL;
    int res = 0;
    
    p1 = rt_dstr_new("helle");
    p2 = rt_dstr_new("hellc");
    
    res = rt_dstr_cmp(p1, p2);
    rt_kprintf("cmp result: %d\n", res);
    
    rt_dstr_del(p1);
    rt_dstr_del(p2);
    
    //  NULL
    p1 = rt_dstr_new("abc");
    res = rt_dstr_cmp(p1, NULL);
    rt_kprintf("s2:NULL result: %d\n", res);
    rt_dstr_del(p1);
    
    p1 = rt_dstr_new("efg");
    res = rt_dstr_cmp(NULL, p1);
    rt_kprintf("s1:NULL result: %d\n", res);
    rt_dstr_del(p1);

    res = rt_dstr_cmp(NULL, NULL);
    rt_kprintf("two NULL result: %d\n", res);
}

void dstr_test_ncmp(void)
{
    rt_dstr_t *p1 = NULL;
    rt_dstr_t *p2 = NULL;
    int res = 0;
    
    p1 = rt_dstr_new("hello");
    p2 = rt_dstr_new("hella");
    
    res = rt_dstr_ncmp(p1, p2, 5);
    rt_kprintf("ncmp result: %d\n", res);
    
    rt_dstr_del(p1);
    rt_dstr_del(p2);
    
    //  NULL
    p1 = rt_dstr_new("abc");
    res = rt_dstr_ncmp(p1, NULL, 2);
    rt_kprintf("s2:NULL ncmp result: %d\n", res);
    rt_dstr_del(p1);
    
    p1 = rt_dstr_new("efg");
    res = rt_dstr_ncmp(NULL, p1, 3);
    rt_kprintf("s1:NULL ncmp result: %d\n", res);
    rt_dstr_del(p1);

    res = rt_dstr_ncmp(NULL, NULL, 4);
    rt_kprintf("two NULL ncmp result: %d\n", res);
}

void dstr_test_casecmp(void)
{
    rt_dstr_t *p1 = NULL;
    rt_dstr_t *p2 = NULL;
    int res = 0;
    
    p1 = rt_dstr_new("hello");
    p2 = rt_dstr_new("HELLO");
    
    res = rt_dstr_casecmp(p1, p2);
    rt_kprintf("casecmp result: %d\n", res);
    
    rt_dstr_del(p1);
    rt_dstr_del(p2);
    
    //  NULL
    p1 = rt_dstr_new("abc");
    res = rt_dstr_casecmp(p1, NULL);
    rt_kprintf("s2:NULL casecmp result: %d\n", res);
    rt_dstr_del(p1);
    
    p1 = rt_dstr_new("efg");
    res = rt_dstr_casecmp(NULL, p1);
    rt_kprintf("s1:NULL casecmp result: %d\n", res);
    rt_dstr_del(p1);

    res = rt_dstr_casecmp(NULL, NULL);
    rt_kprintf("two NULL casecmp result: %d\n", res);    
}

void dstr_test_strlen(void)
{
    rt_dstr_t *p1 = NULL;
    int res = 0;
    
    p1 = rt_dstr_new("hello strlen");
    
    res = rt_dstr_strlen(p1);
    
    if (res == -1)
        return;
    
    rt_kprintf("length: %d\n", res);
    
    rt_dstr_del(p1);
}

void dstr_test_sprintf(void)
{    
    const char *src = "test sprintf";
    rt_dstr_t *p1 = NULL;
    rt_dstr_t *p2 = NULL;
    
    //  string format
    p1 = rt_dstr_new("");
    
    rt_dstr_sprintf(p1, "%s", src);
    
    rt_dstr_printf(p1);
    
    rt_dstr_del(p1);
    
    //  hex format    
    p2 = rt_dstr_new("");
    
    rt_dstr_sprintf(p2, "%08x", 0x20180604);
    
    rt_dstr_printf(p2);
    
    rt_dstr_del(p2);    
}

void dstr_test(void)
{
    dstr_test_new();
    dstr_test_cat();
    dstr_test_ncat();
    dstr_test_cmp();
    dstr_test_ncmp();
    dstr_test_casecmp();
    dstr_test_strlen();
    dstr_test_sprintf();
}

MSH_CMD_EXPORT(dstr_test, dstr example);
