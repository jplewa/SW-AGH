#include "screen_refresh.h"
#include "mp3audio.h"

static void lcd_start(void)
{
  /* LCD Initialization */
  BSP_LCD_Init();

  /* LCD Initialization */
  BSP_LCD_LayerDefaultInit(0, (unsigned int)0xC0000000);
  BSP_LCD_LayerDefaultInit(1, (unsigned int)0xC0000000 + (LCD_X_SIZE * LCD_Y_SIZE * 4));

  /* Enable the LCD */
  BSP_LCD_DisplayOn();

  /* Select the LCD Background Layer  */
  BSP_LCD_SelectLayer(0);

  /* Clear the Background Layer */
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

  BSP_LCD_SetColorKeying(1, LCD_COLOR_WHITE);

  /* Select the LCD Foreground Layer  */
  BSP_LCD_SelectLayer(1);

  /* Clear the Foreground Layer */
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

  /* Configure the transparency for foreground and background :
     Increase the transparency */
  BSP_LCD_SetTransparency(0, 255);
  BSP_LCD_SetTransparency(1, 255);
}

int initialize_touchscreen(void)
{
  uint8_t status = 0;
  status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  if (status != TS_OK)
    return -1;
  return 0;
}

void start_touch_task(void* argument)
{
    lcd_start();
    draw_background();
    draw_volume(30);
    initialize_touchscreen();
    player_state = STOPPED;
    while (1)
    {
        if (redraw_title){
            draw_title(FILES[CURRENT_FILE]);
            redraw_title = 0;
        }
        vTaskDelay(200);
        BSP_TS_GetState(&TS_State);
        if (TS_State.touchDetected)
        {
            if ((TS_State.touchY[0] < big_button_Y + big_button_radius) && (TS_State.touchY[0] > big_button_Y - big_button_radius))
            {
                if ((TS_State.touchX[0] < play_button_X + big_button_radius) && (TS_State.touchX[0] > play_button_X - big_button_radius))
                {
                    switch (player_state)
                    {
                        case PLAYING:
                            player_state = PAUSE_PRESSED;
                            draw_play_button();
                            break;
                        
                        case PAUSED:
                            player_state = RESUME_PRESSED;
                            draw_pause_button();
                            break;
                        
                        case STOPPED:
                            player_state = PLAY_PRESSED;
                            draw_pause_button();
                            break;
                        
                        default:
                            break;
                    }
                }
                else if ((TS_State.touchX[0] < stop_button_X + big_button_radius) && (TS_State.touchX[0] > stop_button_X - big_button_radius))
                {
                    if ((player_state == PLAYING) || (player_state == STOPPED))
                    {
                        player_state = STOP_PRESSED;
                        draw_play_button();
                    }
                }
                else if ((TS_State.touchY[0] < medium_button_Y + medium_button_radius) && (TS_State.touchY[0] > medium_button_Y - medium_button_radius))
                {
                    if ((TS_State.touchX[0] < skip_left_X + medium_button_radius) && (TS_State.touchX[0] > skip_left_X - medium_button_radius))
                    {
                        switch (player_state)
                        {
                            case PLAYING:
                                player_state = PREV_PRESSED_PLAYING;
                                draw_pause_button();
                                break;
                            
                            case PAUSED:
                                player_state = PREV_PRESSED_PAUSED;
                                draw_play_button();
                                break;
                        
                            case STOPPED:
                                player_state = PREV_PRESSED_STOPPED;
                                draw_play_button();
                                break;
                            
                            default:
                                break;                            
                        }
                    }
                    if ((TS_State.touchX[0] < skip_right_X + medium_button_radius) && (TS_State.touchX[0] > skip_right_X - medium_button_radius))
                    {
                        switch(player_state)
                        {
                            case PLAYING:
                                player_state = NEXT_PRESSED_PLAYING;
                                draw_pause_button();
                                break;

                            case PAUSED:
                                player_state = NEXT_PRESSED_PAUSED;
                                draw_play_button();
                                break;
                            
                            case STOPPED:
                                player_state = NEXT_PRESSED_STOPPED;
                                draw_play_button();
                                break;

                            default:
                                break;                     
                        }
                    }
                }
            }
            else if ((TS_State.touchY[0] < small_button_Y + small_button_radius) && (TS_State.touchY[0] > small_button_Y - small_button_radius))
            {
                if (player_state == PLAYING)
                {
                    if ((TS_State.touchX[0] < minus_button_X + small_button_radius) && (TS_State.touchX[0] > minus_button_X - small_button_radius))
                    {
                        player_state = VOL_DOWN_PRESSED;
                        draw_volume(volume);
                    }
                    else if ((TS_State.touchX[0] < plus_button_X + small_button_radius) && (TS_State.touchX[0] > plus_button_X - small_button_radius))
                    {
                        player_state = VOL_UP_PRESSED;
                        draw_volume(volume);
                    }
                }
            }
        }
    }
}
