#include "buttons.h"


// size 480 x 272 px
void draw_background()
{

	// Select the LCD Background Layer
	BSP_LCD_SelectLayer(0);
	BSP_LCD_Clear(BG_COLOR);
	BSP_LCD_SetBackColor(BG_COLOR);

	initialize_button_values();
	draw_previous_button();
	draw_stop_button();
	draw_play_button();
	draw_next_button();
	draw_minus_button();
	draw_plus_button();

}


void initialize_button_values(){
    big_button_radius = 50;
    medium_button_radius = 40;
	small_button_radius = 25;
    big_button_Y = 80 + big_button_radius; // center of stop/play/pause
    medium_button_Y = 80 + big_button_radius;
	small_button_Y = big_button_Y + big_button_radius + small_button_radius + 20;
    big_button_distance = 20;
	medium_button_distance = 25;
	small_button_distance = 40;
    stop_button_X = LCD_X_SIZE/2 - big_button_radius - big_button_distance/2;
    play_button_X = LCD_X_SIZE/2 + big_button_radius + big_button_distance/2;
    skip_left_X = medium_button_distance + medium_button_radius;
    skip_right_X = LCD_X_SIZE - medium_button_distance - medium_button_radius;
	minus_button_X = LCD_X_SIZE/2 - small_button_radius - small_button_distance/2;
	plus_button_X = LCD_X_SIZE/2 + small_button_radius + small_button_distance/2;
}


void draw_play_button(){

	BSP_LCD_SetTextColor(OUTER_SHADE_COLOR);
	BSP_LCD_FillCircle(play_button_X, big_button_Y, big_button_radius + 3);

	BSP_LCD_SetTextColor(OUTER_BUTTON_COLOR);
	BSP_LCD_FillCircle(play_button_X, big_button_Y, big_button_radius);

	BSP_LCD_SetTextColor(INNER_SHADE_COLOR);
	points[0].X = play_button_X - 23;
	points[0].Y = big_button_Y - 28;
	points[1].X = play_button_X - 23;
	points[1].Y = big_button_Y + 28;
	points[2].X = play_button_X + 28;
	points[2].Y = big_button_Y;
	BSP_LCD_FillPolygon(points, 3);

	BSP_LCD_SetTextColor(INNER_BUTTON_COLOR);
	points[0].X = play_button_X - 20;
	points[0].Y = big_button_Y - 25;
	points[1].X = play_button_X - 20;
	points[1].Y = big_button_Y + 25;
	points[2].X = play_button_X + 25;
	points[2].Y = big_button_Y;
	BSP_LCD_FillPolygon(points, 3);
}


void draw_stop_button(){

	BSP_LCD_SetTextColor(OUTER_SHADE_COLOR);
	BSP_LCD_FillCircle(stop_button_X, big_button_Y, big_button_radius + 3);

	BSP_LCD_SetTextColor(OUTER_BUTTON_COLOR);
	BSP_LCD_FillCircle(stop_button_X, big_button_Y, big_button_radius);

	BSP_LCD_SetTextColor(INNER_SHADE_COLOR);
	BSP_LCD_FillRect(stop_button_X - 28, big_button_Y - 28, 56, 56);

	BSP_LCD_SetTextColor(INNER_BUTTON_COLOR);
	BSP_LCD_FillRect(stop_button_X - 27, big_button_Y - 27, 54, 54);
}


void draw_previous_button(){

	BSP_LCD_SetTextColor(OUTER_SHADE_COLOR);
	BSP_LCD_FillCircle(skip_left_X, medium_button_Y, medium_button_radius + 2);
	BSP_LCD_SetTextColor(OUTER_BUTTON_COLOR);
	BSP_LCD_FillCircle(skip_left_X, medium_button_Y, medium_button_radius);
	BSP_LCD_SetTextColor(INNER_SHADE_COLOR);
	points[0].X = skip_left_X - 23;
	points[0].Y = medium_button_Y - 19;
	points[1].X = skip_left_X - 3;
	points[1].Y = medium_button_Y - 8;
	points[2].X = skip_left_X - 3;
	points[2].Y = medium_button_Y - 19;
	points[3].X = skip_left_X + 31;
	points[3].Y = medium_button_Y;
	points[4].X = skip_left_X - 3;
	points[4].Y = medium_button_Y + 19;
	points[5].X = skip_left_X - 3;
	points[5].Y = medium_button_Y + 8;
	points[6].X = skip_left_X - 23;
	points[6].Y = medium_button_Y + 19;
	BSP_LCD_FillPolygon(points, 7);
	BSP_LCD_SetTextColor(INNER_BUTTON_COLOR);
	points[0].X = skip_left_X - 22;
	points[0].Y = medium_button_Y - 18;
	points[1].X = skip_left_X - 2;
	points[1].Y = medium_button_Y - 7;
	points[2].X = skip_left_X - 2;
	points[2].Y = medium_button_Y - 18;
	points[3].X = skip_left_X + 30;
	points[3].Y = medium_button_Y;
	points[4].X = skip_left_X - 2;
	points[4].Y = medium_button_Y + 18;
	points[5].X = skip_left_X - 2;
	points[5].Y = medium_button_Y + 7;
	points[6].X = skip_left_X - 22;
	points[6].Y = medium_button_Y + 18;
	BSP_LCD_FillPolygon(points, 7);
}


void draw_next_button(){

	BSP_LCD_SetTextColor(OUTER_SHADE_COLOR);
	BSP_LCD_FillCircle(skip_right_X, medium_button_Y, medium_button_radius + 2);
	BSP_LCD_SetTextColor(OUTER_BUTTON_COLOR);
	BSP_LCD_FillCircle(skip_right_X, medium_button_Y, medium_button_radius);
	BSP_LCD_SetTextColor(INNER_SHADE_COLOR);
	points[0].X = skip_right_X - 23;
	points[0].Y = medium_button_Y - 19;
	points[1].X = skip_right_X - 3;
	points[1].Y = medium_button_Y - 8;
	points[2].X = skip_right_X - 3;
	points[2].Y = medium_button_Y - 19;
	points[3].X = skip_right_X + 31;
	points[3].Y = medium_button_Y;
	points[4].X = skip_right_X - 3;
	points[4].Y = medium_button_Y + 19;
	points[5].X = skip_right_X - 3;
	points[5].Y = medium_button_Y + 8;
	points[6].X = skip_right_X - 23;
	points[6].Y = medium_button_Y + 19;
	BSP_LCD_FillPolygon(points, 7);
	BSP_LCD_SetTextColor(INNER_BUTTON_COLOR);
	points[0].X = skip_right_X - 22;
	points[0].Y = medium_button_Y - 18;
	points[1].X = skip_right_X - 2;
	points[1].Y = medium_button_Y - 7;
	points[2].X = skip_right_X - 2;
	points[2].Y = medium_button_Y - 18;
	points[3].X = skip_right_X + 30;
	points[3].Y = medium_button_Y;
	points[4].X = skip_right_X - 2;
	points[4].Y = medium_button_Y + 18;
	points[5].X = skip_right_X - 2;
	points[5].Y = medium_button_Y + 7;
	points[6].X = skip_right_X - 22;
	points[6].Y = medium_button_Y + 18;
	BSP_LCD_FillPolygon(points, 7);

}


void draw_minus_button(){

	BSP_LCD_SetTextColor(OUTER_SHADE_COLOR);
	BSP_LCD_FillCircle(minus_button_X, small_button_Y, small_button_radius + 2);

	BSP_LCD_SetTextColor(OUTER_BUTTON_COLOR);
	BSP_LCD_FillCircle(minus_button_X, small_button_Y, small_button_radius);

	BSP_LCD_SetTextColor(INNER_SHADE_COLOR);
	BSP_LCD_FillRect(minus_button_X - 14, small_button_Y - 5, 28, 10);

	BSP_LCD_SetTextColor(INNER_BUTTON_COLOR);
	BSP_LCD_FillRect(minus_button_X - 13, small_button_Y - 4, 26, 8);
}


void draw_plus_button(){

	BSP_LCD_SetTextColor(OUTER_SHADE_COLOR);
	BSP_LCD_FillCircle(plus_button_X, small_button_Y, small_button_radius + 2);

	BSP_LCD_SetTextColor(OUTER_BUTTON_COLOR);
	BSP_LCD_FillCircle(plus_button_X, small_button_Y, small_button_radius);

	BSP_LCD_SetTextColor(INNER_SHADE_COLOR);
	BSP_LCD_FillRect(plus_button_X - 14, small_button_Y - 5, 28, 10);
	BSP_LCD_FillRect(plus_button_X - 5, small_button_Y - 14, 10, 28);

	BSP_LCD_SetTextColor(INNER_BUTTON_COLOR);
	BSP_LCD_FillRect(plus_button_X - 13, small_button_Y - 4, 26, 8);
	BSP_LCD_FillRect(plus_button_X - 4, small_button_Y - 13, 8, 26);
}


void draw_title(uint8_t *title){
	// to nie dziala
	BSP_LCD_SelectLayer(0);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(0, LCD_Y_SIZE / 2 + 53 + 20, title, CENTER_MODE);
}