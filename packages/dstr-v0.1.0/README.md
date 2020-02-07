# dstr

## 1、介绍
这是一个在 RT-Thread 上，基于ANSI/ISO C(C89)实现的动态字符串软件包。

### 1.1 目录结构

| 名称 | 说明 |
| ---- | ---- |
| inc  | 头文件目录 |
| src  | 源代码目录 |
| examples | 例程目录 |

### 1.2 许可证

dstr package 遵循 LGPLv2.1 许可，详见 `LICENSE` 文件。

### 1.3 依赖

对 RT-Thread 无依赖，也可用于裸机。

## 2、如何打开 dstr

使用 dstr package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    miscellaneous packages --->
        [*] dstr: a dynamic string package
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3、使用 dstr

使用dstr可参考例子工程，该工程位于：[/dstr/examples/examples_dstr.c](dstr/examples/examples_dstr.c)

### 3.1 创建动态字符串对象
`rt_dstr_t *rt_dstr_new(const char *str);`

| 参数 | 描述 |
| ---- | ---- |
| name  | 源字符串 |
| return  | 创建成功，将返回dstr对象指针；创建失败则返回NULL |


### 3.2 删除动态字符串对象
`void rt_dstr_del(rt_dstr_t *thiz);`

| 参数 | 描述 |
| ---- | ---- |
| thiz  | 动态字符串对象 |
| return  | 无 |


### 3.3 动态字符串连接
`rt_dstr_t *rt_dstr_cat(rt_dstr_t *const thiz, const char *src);`

| 参数 | 描述 |
| ---- | ---- |
| thiz  | 动态字符串对象 |
| src  | 源字符串 |
| return  | cat成功，将返回dstr对象指针；cat失败，则返回NULL，但不会更改thiz，交由用户处理 |


### 3.4 动态字符串连接（连接几个字符）
`rt_dstr_t *rt_dstr_ncat(rt_dstr_t *const thiz, const char *src, size_t n);`

| 参数 | 描述 |
| ---- | ---- |
| thiz  | 动态字符串对象 |
| src  | 源字符串 |
| n  | 需要连接几个字符 |
| return  | cat成功，将返回dstr对象指针；cat失败，则返回NULL，但不会更改thiz，交由用户处理 |

### 3.5 动态字符串比较
`int rt_dstr_cmp(rt_dstr_t *const dstr1, rt_dstr_t *const dstr2);`

| 参数 | 描述 |
| ---- | ---- |
| dstr1  | 动态字符串1 |
| dstr2  | 动态字符串2 |
| return  | dstr1 < dstr2，返回负数 |
| return  | dstr1 = dstr2，返回0 |
| return  | dstr1 > dstr2，返回正数 |



### 3.6 动态字符串比较（比较几个字符）
`int rt_dstr_ncmp(rt_dstr_t *const dstr1, rt_dstr_t *const dstr2, size_t n);`

| 参数 | 描述 |
| ---- | ---- |
| dstr1  | 动态字符串1 |
| dstr2  | 动态字符串2 |
| n  | 需要比较几个字符 |
| return  | dstr1 < dstr2，返回负数 |
| return  | dstr1 = dstr2，返回0 |
| return  | dstr1 > dstr2，返回正数 |

### 3.7 动态字符串比较（忽略大小写）
`int rt_dstr_casecmp(rt_dstr_t *const dstr1, rt_dstr_t *const dstr2);`

| 参数 | 描述 |
| ---- | ---- |
| dstr1  | 动态字符串1 |
| dstr2  | 动态字符串2 |
| return  | dstr1 < dstr2，返回负数 |
| return  | dstr1 = dstr2，返回0 |
| return  | dstr1 > dstr2，返回正数 |

### 3.8 测量动态字符串长度
`int rt_dstr_strlen(rt_dstr_t *const thiz);`

| 参数 | 描述 |
| ---- | ---- |
| thiz  | 动态字符串对象 |
| return  | 参数非NULL，将返回动态字符串长度；参数为NULL则返回-1 |


### 3.9 字符串格式化输出
`int rt_dstr_sprintf(rt_dstr_t *const thiz, const char *fmt, ...);`

| 参数 | 描述 |
| ---- | ---- |
| thiz  | 动态字符串对象 |
| fmt  | 格式化字符串 |
| argument...  | 可选参数，任何类型的数据 |
| return  | 返回动态字符串长度 |


## 4、注意事项

暂无

## 5、联系方式 & 感谢

* 维护：[never](https://github.com/neverxie)
* 主页：https://github.com/RT-Thread-packages/dstr
