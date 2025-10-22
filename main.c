#include "main.h"
#include "i2c.h"
#include "lcd.h"
#include <avr/io.h>
#include <util/delay.h>

enum GameState {
	GS_TITLE,
	GS_MAIN,
	GS_SCORE,
};

// initialize the adc interface to take readings of the potmeter
// this function doesn't set the default analogue channel selection
// check read_input for that
void adc_init(void)
{
	// enable adc, auto trigger and interrupts
	ADCSRA |= _BV(ADEN) | _BV(ADATE) | _BV(ADIE);
	// enable a prescaler of 128 giving us ~125kHz; perfect for adc
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// auto trigger on timer/counter0 overflow
	ADCSRB = _BV(ADTS2);
	// set reference selection to VCC
	ADMUX |= _BV(REFS0);
}

int main(void)
{
	enum GameState state = GS_TITLE;

	_delay_ms(2000);
	i2c_init();
	lcd_init();
	adc_init();

	while (1) {
		switch (state) {
		case GS_TITLE:
			lcd_clear();
			lcd_set_cursor(0, 0);
			lcd_display("pong by");
			lcd_set_cursor(1, 0);
			lcd_display("@willofdaedalus");
			_delay_ms(2000);
			state = GS_MAIN;
			break;

		case GS_MAIN:
			// draw the two players
			// read input and convert
			lcd_display("main loop!");
			break;

		case GS_SCORE:
			break;
		}
	}
	return 0;
}
