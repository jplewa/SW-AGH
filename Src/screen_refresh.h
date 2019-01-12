#ifndef __SCREEN_REFRESH_H
#define __SCREEN_REFRESH_H

#include "stm32746g_discovery_ts.h"
#include "buttons.h"
#include "wm8994/wm8994.h"
#include "term_io.h"
#include "stm32746g_discovery_audio.h"
#include "pub/mp3dec.h"
#include "pub/mp3common.h"
#include "fatfs.h"
#include <sys/unistd.h>
#include "usb_host.h"
#include "stm32f7xx_hal.h"
#include "mp3audio.h"

typedef enum {
    STOP_PRESSED,
    STOPPED,
    PLAY_PRESSED,
    PLAYING,
    PAUSE_PRESSED,
    PAUSED,
    RESUME_PRESSED,
    PREV_PRESSED_PLAYING,
    NEXT_PRESSED_PLAYING,
    PREV_PRESSED_STOPPED,
    NEXT_PRESSED_STOPPED,
    PREV_PRESSED_PAUSED,
    NEXT_PRESSED_PAUSED,
    VOL_UP_PRESSED,
    VOL_DOWN_PRESSED,
} player_states;

TS_StateTypeDef TS_State;

volatile player_states touch_sound_communication;

void StartTouchTask(void *argument);

#endif