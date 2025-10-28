#include "lcd.h"
#include "main.h"

void move_ball(struct ball *b)
{
	if (!b)
		return;

	b->x += b->dx;
	b->y += b->dy;

	if (b->y < 0) {
		b->y = 0;
		b->dy = -b->dy;
	} else if (b->y > LCD_ROWS - 1) {
		b->y = LCD_ROWS - 1;
		b->dy = -b->dy;
	}

	if (b->x < 0) {
		b->x = 0;
		b->dx = -b->dx;
	} else if (b->x > LCD_COLS - 1) {
		b->x = LCD_COLS - 1;
		b->dx = -b->dx;
	}
}

void render_ball(struct ball *b)
{
	uint8_t new_x = (uint8_t) (b->x);
	uint8_t new_y = (uint8_t) (b->y);

	// erase old ball if it moved
	if (b->prev_x != new_x || b->prev_y != new_y) {
		lcd_set_cursor(b->prev_y, b->prev_x);
		lcd_display(" ");
	}

	// draw new ball
	lcd_set_cursor(new_y, new_x);
	lcd_display("o");

	// update previous position
	b->prev_x = new_x;
	b->prev_y = new_y;
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

	lcd_set_cursor(*prev, x);
	lcd_display(" ");
	lcd_set_cursor(new_pos, x);
	lcd_display("|");

	*prev = new_pos;
	*pos = new_pos;
}
