#include "main.h"
#include "i2c.h"
#include "lcd.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// track which player's input we're reading
volatile static uint8_t cur_player;
volatile static uint8_t adc_read;


enum GameState {
	GS_TITLE,
	GS_MAIN,
	GS_SCORE,
};

ISR(ADC_vect)
{
	adc_read = ADCL | (ADCH << 8);
	cur_player = !cur_player;
}

// initialize the adc interface to take readings of the potmeter
// this function doesn't set the default analogue channel selection
// check read_input for that
void timer_init(void)
{
	// CTC mode: WGM12 = 1
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	// prescaler 1024
	TCCR1B |= _BV(CS12) | _BV(CS10);
	// ~150ms for every update to reduce analogue noise and excessive
	// polling on the potmeters
	OCR1A = 2344;
}

void adc_init(void)
{
	// enable adc, auto trigger
	ADCSRA = _BV(ADEN) | _BV(ADATE);
	// prescaler 128 (~125 kHz)
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// auto trigger source: timer/counter1 compare match b
	ADCSRB = _BV(ADTS2) | _BV(ADTS0);
	// reference VCC
	ADMUX = _BV(REFS0);
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
