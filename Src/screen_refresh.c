#include "screen_refresh.h"

int lcd_start(void)
{
    /* LCD Initialization */
    if (BSP_LCD_Init() != LCD_OK)
    {
        return -1;
    }

    /* LCD Initialization */
    BSP_LCD_LayerDefaultInit(0, (unsigned int) 0xC0000000);
    BSP_LCD_LayerDefaultInit(1, (unsigned int) 0xC0000000 + (LCD_X_SIZE * LCD_Y_SIZE * 4));

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

    return 0;
}

int initialize_touchscreen(void)
{
    uint8_t status = 0;
    status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    if (status != TS_OK)
    {
        return -1;
    }
    return 0;
}

int pressed_in_Y_axis(int center, int radius)
{
    return (TS_State.touchY[0] < center + radius)
        && (TS_State.touchY[0] > center - radius);
}

int pressed_in_X_axis(int center, int radius)
{
    return (TS_State.touchX[0] < center + radius)
        && (TS_State.touchX[0] > center - radius);
}

void start_touch_task(void *argument)
{
    if (lcd_start() || initialize_touchscreen())
    {
        while (1) {}
    }

    draw_background();

    player_state = STOPPED;

    while (1)
    {
        if (redraw_title)
        {
            draw_title(FILES[CURRENT_FILE]);
            redraw_title = 0;
        }
        vTaskDelay(200);
        if (BSP_TS_GetState(&TS_State) != TS_OK)
        {
            while (1) {}
        }
        if (TS_State.touchDetected)
        {
            if (pressed_in_Y_axis(big_button_Y, big_button_radius))
            {
                if (pressed_in_X_axis(play_button_X, big_button_radius))
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
                } else if (pressed_in_X_axis(stop_button_X, big_button_radius))
                {
                    if (player_state == PLAYING)
                    {
                        player_state = STOP_PRESSED;
                        draw_play_button();
                    }
                } else if (pressed_in_Y_axis(medium_button_Y, medium_button_radius))
                {
                    if (pressed_in_X_axis(skip_left_X, medium_button_radius))
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
                    if (pressed_in_X_axis(skip_right_X, medium_button_radius))
                    {
                        switch (player_state)
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
            } else if (pressed_in_Y_axis(small_button_Y, small_button_radius))
            {
                if (player_state == PLAYING)
                {
                    if (pressed_in_X_axis(minus_button_X, small_button_radius))
                    {
                        player_state = VOL_DOWN_PRESSED;
                        draw_volume(volume);
                    } else if (pressed_in_X_axis(plus_button_X, small_button_radius))
                    {
                        player_state = VOL_UP_PRESSED;
                        draw_volume(volume);
                    }
                }
            }
        }
    }
}
