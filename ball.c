#include "lcd.h"
#include "main.h"


void move_ball(struct ball *b)
{
	if (!b)
		return;

	b->x += b->dx;
	b->y += b->dy;

	// vertical walls (top/bottom)
	if (b->y < 0) {
		b->y = 0;
		b->dy = -b->dy;
	} else if (b->y > LCD_ROWS - 1) {
		b->y = LCD_ROWS - 1;
		b->dy = -b->dy;
	}

	// horizontal walls (left/right)
	if (b->x < 0) {
		b->x = 0;
		b->dx = -b->dx;
	} else if (b->x > LCD_COLS - 1) {
		b->x = LCD_COLS - 1;
		b->dx = -b->dx;
	}
}

void render_ball(struct ball *ball)
{
	if (!ball)
		return;

	lcd_clear();
	lcd_set_cursor((uint8_t) ball->y, (uint8_t) ball->x);
	// lcd_set_cursor((uint8_t) (ball->y + 0.5f), (uint8_t) (ball->x +
	// 0.5f));
	lcd_display("o");
}

void check_collisions(struct ball *ball)
{
	if (!ball)
		return;
}
