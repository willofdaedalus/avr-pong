#include "lcd.h"
#include "main.h"
#include <math.h>
#include <stdlib.h>

void move_ball(struct ball *ball, int8_t p1, int8_t p2)
{
	int8_t diff;

	if (!ball)
		return;

	ball->x += ball->dx;
	ball->y += ball->dy;

	// check collision on the top and bottom
	if (ball->y < 0) {
		ball->y = 0;
		ball->dy = -ball->dy;
	} else if (ball->y > LCD_ROWS - 1) {
		ball->y = LCD_ROWS - 1;
		ball->dy = -ball->dy;
	}

	// check collisions on the left and right
	if (ball->x < 0) {
		ball->x = 0;
		ball->dx = -ball->dx;
	} else if (ball->x > LCD_COLS - 1) {
		ball->x = LCD_COLS - 1;
		ball->dx = -ball->dx;
	}

	// collision with paddles; not correct but works
	if (ball->x == PLAYER1_COL) {
		diff = ball->y - p1;
		if (abs(diff) <= 1) {
			ball->dx = -ball->dx;

			if (diff < 0)
				ball->dy = -fabs(ball->dy);
			else if (diff > 0)
				ball->dy = fabs(ball->dy);
		}
	} else if (ball->x == PLAYER2_COL) {
		diff = ball->y - p2;
		if (abs(diff) <= 1) {
			ball->dx = -ball->dx;

			if (diff < 0)
				ball->dy = -fabs(ball->dy);
			else if (diff > 0)
				ball->dy = fabs(ball->dy);
		}
	}
}

void render_ball(struct ball *ball)
{
	uint8_t new_x = (uint8_t) (ball->x);
	uint8_t new_y = (uint8_t) (ball->y);

	// erase old ball if it moved
	if (ball->prev_x != new_x || ball->prev_y != new_y) {
		lcd_set_cursor(ball->prev_y, ball->prev_x);
		lcd_display(" ");
	}

	// draw new ball
	lcd_set_cursor(new_y, new_x);
	lcd_display("o");

	// update previous position
	ball->prev_x = new_x;
	ball->prev_y = new_y;
}


void paddle_logic(uint16_t adc, int8_t *pos, uint8_t *prev, uint8_t x)
{
	int8_t new_pos;

	// direct mapping: adc (0-1023) -> rows (0-3)
	new_pos = (adc * LCD_ROWS) / 1024;

	// safety clamp
	if (new_pos < 0)
		new_pos = 0;
	else if (new_pos > LCD_ROWS - 1)
		new_pos = LCD_ROWS - 1;

	// draw over the old space and re-render the paddle
	lcd_set_cursor(*prev, x);
	lcd_display(" ");
	lcd_set_cursor(new_pos, x);
	lcd_display("|");

	*prev = new_pos;
	*pos = new_pos;
}
