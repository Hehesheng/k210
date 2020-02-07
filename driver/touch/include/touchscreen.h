#ifndef __TOUCHSCREEN_H_
#define __TOUCHSCREEN_H_

#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
#include "stdbool.h"

typedef enum
{
    TOUCHSCREEN_STATUS_IDLE = 0,
    TOUCHSCREEN_STATUS_RELEASE,
    TOUCHSCREEN_STATUS_PRESS,
    TOUCHSCREEN_STATUS_MOVE
} touchscreen_type_t;

int touchscreen_init(void* arg);
int touchscreen_read(int* type, int* x, int* y);
int touchscreen_deinit(void);
bool touchscreen_is_init(void);

/////////////// HAL ////////////////////

#define CALIBRATION_SIZE 7

typedef struct
{
    int calibration[CALIBRATION_SIZE];  // 7 Bytes
} touchscreen_config_t;

#define touchscreen_malloc(p) rt_malloc(p)
#define touchscreen_free(p) rt_free(p)

#endif
