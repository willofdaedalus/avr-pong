#ifndef _MAIN_H
#define _MAIN_H

#include <stdint.h>

#define ADC_CONST (1023.0f)
#define ADC_FILTER (10)

#define PLAYER1_ADC (0x7) // pin A0
#define PLAYER2_ADC (0x0) // pin A5

#define BALL_SPEED (0.5f)

struct ball {
	float x, y;	      // ball vector coordinates in memory
	float dx, dy;	      // delta y and delta x
	float prev_x, prev_y; // track ball's prev positions for redraws
};

void render_ball(struct ball *ball);
void move_ball(struct ball *ball);
void paddle_logic(uint16_t adc,
		  int8_t *pos,
		  uint8_t *del,
		  uint8_t *prev,
		  uint8_t x);

#endif
