#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "stm32746g_discovery_lcd.h"
#include "mp3_audio.h"

/* color constants */
#define BG_COLOR ((uint32_t)0xFF241593)
#define OUTER_BUTTON_COLOR ((uint32_t)0xFF056CFF)
#define OUTER_SHADE_COLOR ((uint32_t)0xFF1E3454)
#define INNER_BUTTON_COLOR ((uint32_t)0xFF0805FF)
#define INNER_SHADE_COLOR ((uint32_t)0xFF0F0E3B)
#define LCD_X_SIZE RK043FN48H_WIDTH
#define LCD_Y_SIZE RK043FN48H_HEIGHT

/* button sizes */
int big_button_radius;
int medium_button_radius;
int small_button_radius;
int big_button_Y;
int medium_button_Y;
int small_button_Y;
int big_button_distance;
int medium_button_distance;
int small_button_distance;
int stop_button_X;
int play_button_X;
int skip_left_X;
int skip_right_X;
int minus_button_X;
int plus_button_X;

Point points[10];

/* GUI functions */
void draw_background();
void initialize_button_values();
void draw_previous_button();
void draw_stop_button();
void draw_play_button();
void draw_pause_button();
void draw_next_button();
void draw_minus_button();
void draw_plus_button();
void draw_volume();
void draw_title();

#endif
