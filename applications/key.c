#include <msh.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <stdio.h>

#include <fpioa.h>
#include <gpio.h>
#include <gpiohs.h>

#include "multi_button.h"

#define LOG_TAG "key"
#include <ulog.h>

#define KEY0_PIN (0)
#define KEY1_PIN (1)
#define KEY2_PIN (2)

static uint8_t get_pin_value(button* btn) { return gpio_get_pin((size_t)btn->user); }

static void start_tft(void* param) { msh_exec("try_tft", strlen("try_tft")); }

static void button_callback(void* param)
{
    button* btn = (button*)param;
    rt_kprintf("button name: %s, event: %d\n", btn->name, get_button_event(btn));
}

static void key_thread(void* param)
{
    button* key[3] = {0};

    key[0] = button_create("DOWN", get_pin_value, 0);
    key[0]->user = (void*)KEY0_PIN;
    key[1] = button_create("MEDIUM", get_pin_value, 0);
    key[1]->user = (void*)KEY1_PIN;
    key[2] = button_create("UP", get_pin_value, 0);
    key[2]->user = (void*)KEY2_PIN;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < number_of_event; j++)
        {
            button_attach(key[i], j, button_callback);
        }
    }
    button_attach(key[1], SINGLE_CLICK, start_tft);

    while (1)
    {
        button_ticks();
        rt_thread_delay(RT_TICK_PER_SECOND / 10);
    }
}

static int key_init(void)
{
    rt_thread_t tid;

    gpio_init();

    gpio_set_drive_mode(KEY0_PIN, GPIO_DM_INPUT);
    fpioa_set_function(16, FUNC_GPIO0 + KEY0_PIN);
    gpio_set_drive_mode(KEY1_PIN, GPIO_DM_INPUT);
    fpioa_set_function(15, FUNC_GPIO0 + KEY1_PIN);
    gpio_set_drive_mode(KEY2_PIN, GPIO_DM_INPUT);
    fpioa_set_function(17, FUNC_GPIO0 + KEY2_PIN);

    tid = rt_thread_create("key", key_thread, RT_NULL, 4096, 10, 20);
    if (tid)
    {
        rt_thread_startup(tid);
    }
}
INIT_DEVICE_EXPORT(key_init);
