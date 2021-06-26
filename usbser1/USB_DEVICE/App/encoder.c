/*
 * encoder.c
 *
 *  Created on: Jun 12, 2021
 *      Author: Brian Moran
 */

#include "encoder.h"


// call this from the systick interrupt every millisecond
// modified from the original code by dannyf
// at https://github.com/dannyf00/My-MCU-Libraries---2nd-try/blob/master/encoder1.c
// minor amount of filtering..

int encoder1_read(void)
{
  volatile static uint8_t ABs = 0;
  ABs = (ABs << 2) & 0x0f; //left 2 bits now contain the previous AB key read-out;
  ABs |= (HAL_GPIO_ReadPin(ENC_CLK_BANK, ENC_CLK) << 1) | HAL_GPIO_ReadPin(ENC_DATA_BANK, ENC_DATA);
  switch (ABs)
  {
    case 0x0e:
      return +1;
      break;
    case 0x0d:
      return -1;
      break;
  }
  return 0;
}

// generic encoder read with two pins
int encoder_read(GPIO_TypeDef *clk_bank, uint16_t clk_pin, GPIO_TypeDef *data_bank, uint16_t data_pin, uint8_t invert)
{
  volatile static uint8_t ABs = 0;
  ABs = (ABs << 2) & 0x0f; //left 2 bits now contain the previous AB key read-out;
  ABs |= (HAL_GPIO_ReadPin(clk_bank, clk_pin) << 1) | HAL_GPIO_ReadPin(data_bank, data_pin);
  switch (ABs)
  {
    case 0x0e:
      return invert? -1 : +1;
      break;
    case 0x0d:
      return invert? +1 : -1;
      break;
  }
  return 0;
}

#define MAX_ENCODER_INTERVAL 24
#define MAX_ENCODER_DIVIDER 4

void encoder1_routine_1ms() {
	static unsigned int msecs_since_last = 0;
	//static uint8_t pcount = 0;
	static int duration = 0;
	static int encoder_1_value = 0;
	static int delta = 0;
	static int accum = 0;

	uint8_t mult = 0;

	uint8_t event[EVT_QWIDTH];

	// decrement duration
	duration--;

	msecs_since_last++;
	if (msecs_since_last > MAX_ENCODER_INTERVAL) msecs_since_last = MAX_ENCODER_INTERVAL;
	mult = msecs_since_last >= MAX_ENCODER_INTERVAL ? 1 : (uint8_t)((MAX_ENCODER_INTERVAL - msecs_since_last)/ MAX_ENCODER_DIVIDER);
	if (mult==0) mult=1;

	accum = accum - (accum >> 5) + mult;

	// read encoder values
	delta = encoder1_read();
	// delta = encoder_read(ENC_CLK_BANK, ENC_CLK, ENC_DATA_BANK, ENC_DATA, false); // takes 60 more bytes.

	if (delta)
		{
			//pcount++;
			msecs_since_last = 0;
		}
    encoder_1_value += delta ;//* mult;

	if (duration < 0 && encoder_1_value) {
		//encoder_1_value =  (accum >> 5);
		if (encoder_1_value < 0) {
			event[0] = EVT_ENCODER_DOWN;
			event[2] = (uint8_t) ((accum >> 5));
			//event[2] = (uint8_t) (pcount);
			event[1] = 1;

			// post the event to indicate the end of the down state
			Evt_EnQueue(event);
		}

		if (encoder_1_value > 0) {
			event[0] = EVT_ENCODER_UP;
			event[2] = (uint8_t) ((accum >> 5));
			//event[2] = (uint8_t) (pcount);
			event[1] = 1;

			// post the event to indicate the end of the down state
			Evt_EnQueue(event);
		}
		duration = reg_r;
		encoder_1_value = 0;
	}

}
