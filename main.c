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
static volatile uint16_t adc_read;


enum GameState {
	GS_TITLE,
	GS_MAIN,
	GS_SCORE,
};

ISR(ADC_vect)
{
	adc_read = ADCL | (ADCH << 8);
	// adc_read = ADCL | (ADCH << 8);
	// player1 = !player1;
	// // clear out the current meter being read
	// ADMUX &= ~(cur_meter << 1);
	// // switch to the next potmeter
	// cur_meter = (player1) ? PLAYER1_ADC : PLAYER2_ADC;
	// // activate the next meter for reading on the next update
	// ADMUX |= (cur_meter << 1);
}

void timer_init(void)
{
	// prescaler 1024 with CTC
	TCCR1B |= _BV(CS12) | _BV(CS10) | _BV(WGM12);
	// ~150ms for every update to reduce analogue noise and excessive
	// polling on the potmeters
	OCR1A = 2344;
}

void adc_init(void)
{
	// enable adc, auto trigger and interrupts
	// ADCSRA |= _BV(ADEN) | _BV(ADATE) | _BV(ADIE);
	// enable adc, auto trigger
	ADCSRA |= _BV(ADEN) | _BV(ADATE) | _BV(ADIE);
	// enable a prescaler of 128 giving us ~125kHz; perfect for adc
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// auto trigger on timer/counter1 comp match
	ADCSRB = _BV(ADTS2) | _BV(ADTS0);
	// set reference selection to VCC
	ADMUX = _BV(REFS0) | PLAYER1_ADC;
}

// initialize the adc interface to take readings of the potmeter
// this function doesn't set the default analogue channel selection
// check read_input for that
// void timer_init(void)
// {
// 	// CTC mode: WGM12 = 1
// 	TCCR1A = 0;
// 	TCCR1B = _BV(WGM12);
// 	// prescaler 1024
// 	TCCR1B |= _BV(CS12) | _BV(CS10);
// 	// ~150ms for every update to reduce analogue noise and excessive
// 	// polling on the potmeters
// 	OCR1A = 2344;
// }
//
// void adc_init(void)
// {
// 	// enable adc, auto trigger
// 	ADCSRA = _BV(ADEN) | _BV(ADATE);
// 	// prescaler 128 (~125 kHz)
// 	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
// 	// auto trigger source: timer/counter1 compare match b
// 	ADCSRB = _BV(ADTS2) | _BV(ADTS0);
// 	// reference VCC
// 	ADMUX = _BV(REFS0);
//
// 	// very important line; player one always starts first and we switch
// 	// everytime OCR1A hits its MAX to balance both potmeters on the same
// 	// ADC line thanks to the multiplexer
// 	ADMUX |= (PLAYER1_ADC << 1);
// }

int main(void)
{
	enum GameState state = GS_MAIN;
	char buf[8] = {0};

	_delay_ms(2000);
	i2c_init();
	lcd_init();

	timer_init();
	adc_init();

	// player1 = 1;
	// // starting meter is player1
	// cur_meter = PLAYER1_ADC;


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
			itoa(adc_read, buf, 10);
			lcd_set_cursor(0, 0);
			lcd_display("     ");
			lcd_set_cursor(0, 0);
			lcd_display(buf);
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
	}
}
