#include "main.h"
#include "i2c.h"
#include "lcd.h"
#include <avr/interrupt.h>
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
	// ADCSRA |= _BV(ADEN) | _BV(ADATE) | _BV(ADIE);
	// enable adc, auto trigger
	ADCSRA |= _BV(ADEN) | _BV(ADATE);
	// enable a prescaler of 128 giving us ~125kHz; perfect for adc
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// auto trigger on timer/counter0 comp match
	ADCSRB = _BV(ADTS0) | _BV(ADTS2);
	// set reference selection to VCC
	ADMUX = _BV(REFS0);
}

// initialize timers; this will be useful for auto triggering in the adc
// whenever ocr0a hit that 249 value
void timer_init(void)
{
	// CTC mode, reset at OCR0A
	TCCR0A = _BV(WGM01);
	// prescaler 1024
	TCCR0B = _BV(CS02) | _BV(CS00);
	// compare match interrupt enable
	// TIMSK0 = _BV(OCIE0A);
	OCR0A = 249; // ~16ms
}

int main(void)
{
	enum GameState state = GS_TITLE;

	_delay_ms(2000);
	i2c_init();
	lcd_init();

	adc_init();
	timer_init();


	sei();

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
