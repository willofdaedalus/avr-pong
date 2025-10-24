#include "main.h"
#include "i2c.h"
#include "lcd.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

// track which player's input we're reading
static volatile uint8_t player1;
static volatile uint8_t cur_meter;
static volatile uint16_t adc;
static volatile uint8_t p1_delta;
static volatile uint8_t p2_delta;

// static uint8_t map[LCD_COLS];

enum GameState {
	GS_TITLE,
	GS_MAIN,
	GS_SCORE,
};

ISR(TIMER1_COMPA_vect)
{
	ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect)
{
	adc = ADCL | (ADCH << 8);
	player1 = !player1;
	// clear out the current meter being read
	ADMUX &= ~(cur_meter);
	// switch to the next potmeter
	cur_meter = (player1) ? PLAYER1_ADC : PLAYER2_ADC;
	// activate the next meter for reading on the next update
	ADMUX |= cur_meter;
}

void timer_init(void)
{
	// prescaler 1024 with CTC
	TCCR1B |= _BV(CS12) | _BV(CS10) | _BV(WGM12);
	// ~150ms for every update to reduce analogue noise and excessive
	// polling on the potmeters
	OCR1A = 2344;
	TIMSK1 = _BV(OCIE1A);
}

// initialize the adc interface to take readings of the potmeter
// this function doesn't set the default analogue channel selection
// check read_input for that
void adc_init(void)
{
	// enable adc, auto trigger and interrupts
	// ADCSRA |= _BV(ADEN) | _BV(ADATE) | _BV(ADIE);
	// enable adc and interrupts
	ADCSRA |= _BV(ADEN) | _BV(ADIE);
	// enable a prescaler of 128 giving us ~125kHz; perfect for adc
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// auto trigger on timer/counter1 comp match
	// ADCSRB = _BV(ADTS2) | _BV(ADTS0);
	// set reference selection to VCC
	ADMUX = _BV(REFS0) | PLAYER1_ADC;
}

int main(void)
{
	int8_t p1 = 0;
	int8_t p1_prev = 0;
	enum GameState state = GS_MAIN;
	// char buf[8] = {0};
	struct ball ball = {8, 2, 0.95, 1, 8, 2};

	_delay_ms(2000);
	i2c_init();
	lcd_init();

	timer_init();
	adc_init();

	player1 = 1;
	// starting meter is player1
	cur_meter = PLAYER1_ADC;


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
			lcd_clear();
			break;

		case GS_MAIN:
			move_ball(&ball);
			render_ball(&ball);
			// itoa(adc, buf, 10);
			if (player1) {
				if (adc > (uint16_t) (p1_delta + ADC_FILTER)) {
					p1 += 1;
				} else if (adc <
					   (uint16_t) (p1_delta - ADC_FILTER)) {
					p1 -= 1;
				}

				if (p1 < 0)
					p1 = 0;
				else if (p1 > LCD_ROWS - 1)
					p1 = LCD_ROWS - 1;

				lcd_set_cursor(p1_prev, 1);
				lcd_display(" ");
				lcd_set_cursor(p1, 1);
				lcd_display("|");

				// update the delta
				p1_delta = adc;
				p1_prev = p1;
			}
			// draw the two players
			// read input and convert
			// lcd_display("main loop!");
			break;

		case GS_SCORE:
			// draw the two players
			// read input and convert
			// lcd_display("main loop!");
			break;
		}

		_delay_ms(150);
	}
}
