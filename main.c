#include "main.h"
#include "i2c.h"
#include "lcd.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>

#define BALL_VEL (0.5)

// track which player's input we're reading
static volatile uint8_t adc_chan;
static volatile uint8_t cur_meter;
static volatile uint16_t adc;

enum GameState {
	GS_TITLE,
	GS_MAIN,
	GS_SCORE,
};

ISR(TIMER1_COMPA_vect)
{
	// on time compare match begin adc conversion
	ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect)
{
	adc = ADCL | (ADCH << 8);
	adc_chan = !adc_chan;
	// switch to the next potmeter
	cur_meter = (adc_chan) ? PLAYER1_ADC : PLAYER2_ADC;
	// activate the next meter for reading on the next update
	ADMUX = (ADMUX & 0xf0) | (cur_meter & 0x0f);
}

void timer_init(void)
{
	// prescaler 1024 with CTC
	TCCR1B |= _BV(CS12) | _BV(CS10) | _BV(WGM12);
	// ~150ms for every update to reduce analogue noise and excessive
	// polling on the potmeters
	OCR1A = 780;
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
	int8_t p1 = 0, p2 = 0;
	uint8_t chan;
	uint8_t p1_prev = 0, p2_prev = 0;
	uint16_t adc_copy;
	enum GameState state = GS_MAIN;
	struct ball ball = {.x = 8,
			    .y = 2,
			    .dx = BALL_VEL,
			    .dy = BALL_VEL,
			    .prev_x = 8,
			    .prev_y = 2};

	_delay_ms(2000);
	i2c_init();
	lcd_init();

	timer_init();
	adc_init();

	adc_chan = 1;
	// starting meter is adc_chan
	cur_meter = PLAYER1_ADC;


	sei();

	while (1) {
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			adc_copy = adc;
			chan = adc_chan;
		}

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

			if (chan) {
				paddle_logic(adc_copy, &p1, &p1_prev, 1);
			} else {
				paddle_logic(adc_copy, &p2, &p2_prev, 18);
			}
			break;

		case GS_SCORE:
			// draw the two players
			// read input and convert
			// lcd_display("main loop!");
			break;
		}

		_delay_ms(50);
	}
}
