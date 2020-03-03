#ifndef __LITTLEVGL2RTT_H__
#define __LITTLEVGL2RTT_H__

#include <rtthread.h> 
#include <rtdevice.h> 

enum littlevgl2rtt_input_state
{
    LITTLEVGL2RTT_INPUT_NONE = 0x00,
    LITTLEVGL2RTT_INPUT_UP   = 0x01, 
    LITTLEVGL2RTT_INPUT_DOWN = 0x02, 
    LITTLEVGL2RTT_INPUT_MOVE = 0x03
}; 

#endif
