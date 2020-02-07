/*
 * File      : dstr.c
 * This file is part of dstr (dynamic string) package
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
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dstr.h"

#define DBG_ENABLE
#undef  DBG_ENABLE
#define DBG_SECTION_NAME  "[RTDSTR]"
#define DBG_LEVEL         DBG_INFO
#define DBG_COLOR
#include <rtdbg.h>

/**
 * This function will create a dstr(dynamic string) object.
 *
 * @param str the string
 *
 * @return a new dstr thiz
 */
rt_dstr_t *rt_dstr_new(const char *str)
{
    rt_dstr_t *thiz = NULL;
    
    if (str == NULL)
    {
        dbg_log(DBG_ERROR, "new.string param error\n");
        return NULL;
    }
    
    thiz = (rt_dstr_t *)malloc(sizeof(struct rt_dstr));
    if (thiz == NULL)
    {
        dbg_log(DBG_ERROR, "new malloc error\n");
        return NULL;
    }

    thiz->length = strlen(str) + 1; //  allocated space
    thiz->str = (char *)malloc(sizeof(char) * thiz->length);

    if (thiz->str == NULL)
    {
        free(thiz);
        return NULL;
    }

    memcpy(thiz->str, str, thiz->length);

    return thiz;
}

/**
 * This function will delete a dstr(dynamic string) object.
 *
 * @param thiz a dstr(dynamic string) thiz
 *
 * @return none
 */
void rt_dstr_del(rt_dstr_t *thiz)
{
    if (thiz == NULL)
        return;

    if (thiz->str == NULL)
    {
        free(thiz);
        return;
    }

    free(thiz->str);

    free(thiz);
}

static int rt_dstr_resize(rt_dstr_t *const thiz, size_t new_spacesize)
{
    char *p = NULL;

    if (thiz == NULL)
    {
        dbg_log(DBG_ERROR, "resize.thiz param error\n");
        return -1;
    }
    
    p = (char *)realloc(thiz->str, new_spacesize);

    if (p == NULL)
    {
        dbg_log(DBG_ERROR, "resize.realloc error\n");
        return -1;
    }
    else
    {   
        thiz->length = new_spacesize;
        dbg_log(DBG_INFO, "new_spacesize:%d\n", thiz->length);       
        thiz->str = p;
        return 0;
    }
}

/**
 * This function appends the src string to the dest object, 
 * overwriting the terminating null byte '\0' at the end of the dest,
 * and then adds a terminating null byte.
 *
 * @param thiz the dstr(dynamic string) thiz
 * @param src  the string
 *
 * @return the dest dstr
 */
rt_dstr_t *rt_dstr_cat(rt_dstr_t *const thiz, const char *src)
{
    return rt_dstr_ncat(thiz, src, strlen(src));
}

/**
 * This function is similar, except that it will use at most n bytes from src,
 * and src does not need to be null-terminated if it contains n or more bytes.
 *
 * @param thiz the dstr(dynamic string) thiz
 * @param src  the string
 * @param count the maximum compare length
 *
 * @return the dest dstr
 */
rt_dstr_t *rt_dstr_ncat(rt_dstr_t *const thiz, const char *src, size_t n)
{
    int res = 0;
    size_t new_spacesize = 0, old_spacesize = 0;
    
    old_spacesize = thiz->length;       //  allocated space

    new_spacesize = n + old_spacesize;  //  allocated space   
    
    res = rt_dstr_resize(thiz, new_spacesize);

    if (res == -1)
    {
        dbg_log(DBG_ERROR, "nacat.resize error\n");
        return NULL;
    }

    memcpy(thiz->str + (old_spacesize - 1), src, n); 
    *(thiz->str + (old_spacesize - 1) + n) = '\0';
    
    return thiz;
}

/**
 * This function compares dstr1 and dstr2.
 *
 * @param dstr1 the dstr to be compared
 * @param dstr2 the dstr to be compared
 *
 * @return the result
 */
int rt_dstr_cmp(rt_dstr_t *const dstr1, rt_dstr_t *const dstr2)
{   
    char *str1, *str2;
    
    if ((dstr1 == NULL) && (dstr2 == NULL))
    {
        str1 = NULL;
        str2 = NULL;
    }
    else if(dstr1 == NULL)
    {
        str1 = NULL;
        str2 = dstr2->str;
    }
    else if(dstr2 == NULL)
    {
        str2 = NULL;
        str1 = dstr1->str;
    }
    else
    {
        str1 = dstr1->str;
        str2 = dstr2->str;
    }
    
    return strcmp(str1, str2);
}
    
/**
 * This function compares dstr1 and dstr2.
 *
 * @param dstr1 the dstr to be compared
 * @param dstr2 the dstr to be compared
 * @param count the maximum compare length
 *
 * @return the result
 */
int rt_dstr_ncmp(rt_dstr_t *const dstr1, rt_dstr_t *const dstr2, size_t n)
{
    char *str1, *str2;
    
    if ((dstr1 == NULL) && (dstr2 == NULL))
    {
        str1 = NULL;
        str2 = NULL;
    }
    else if(dstr1 == NULL)
    {
        str1 = NULL;
        str2 = dstr2->str;
    }
    else if(dstr2 == NULL)
    {
        str2 = NULL;
        str1 = dstr1->str;
    }
    else
    {
        str1 = dstr1->str;
        str2 = dstr2->str;
    }
    
    return strncmp(str1, str2, n);
}

/**
 * This function will compare two dstrs while ignoring differences in case.
 *
 * @param dstr1 the dstr to be compared
 * @param dstr2 the dstr to be compared
 *
 * @return the result
 */
int rt_dstr_casecmp(rt_dstr_t *const dstr1, rt_dstr_t *const dstr2)
{
    char *str1, *str2;
    
    if ((dstr1 == NULL) && (dstr2 == NULL))
    {
        str1 = NULL;
        str2 = NULL;
    }
    else if(dstr1 == NULL)
    {
        str1 = NULL;
        str2 = dstr2->str;
    }
    else if(dstr2 == NULL)
    {
        str2 = NULL;
        str1 = dstr1->str;
    }
    else
    {
        str1 = dstr1->str;
        str2 = dstr2->str;
    }
    
    return strcasecmp (str1, str2);
}

/**
 * This function will return the length of a dstr, which terminate will
 * null character.
 *
 * @param thiz the dstr
 *
 * @return the length of dstr
 */
int rt_dstr_strlen(rt_dstr_t *const thiz)
{
    if (thiz == NULL)
        return -1;
    
    return strlen(thiz->str);
}

/**
 * This function will return the length of a dstr, which terminate will
 * null character.
 *
 * @param thiz the dstr
 *
 * @return the length of dstr
 */
int rt_dstr_sprintf(rt_dstr_t *const thiz, const char *fmt, ...)
{
    va_list  arg_ptr;
    va_list  tmp;
    int status = 0, res = 0, new_length = 0;
    
    va_start(arg_ptr, fmt);
   
    va_copy(tmp, arg_ptr);
    
    new_length = vsnprintf(NULL, 0, fmt, tmp);      // strlen("test sprintf") = 12
    
    va_end(tmp);
     
    status = rt_dstr_resize(thiz, new_length + 1);  //  allocated space

    if (status == -1)
    {
        va_end(arg_ptr);
        return -1;
    }  
    
    res = vsnprintf(thiz->str, new_length + 1, fmt, arg_ptr);

    va_end(arg_ptr);
    
    return res;
}
