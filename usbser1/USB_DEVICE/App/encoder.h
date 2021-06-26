/*
 * encoder.h
 *
 *  Created on: Jun 12, 2021
 *      Author: Brian Moran
 */

#ifndef APP_ENCODER_H_
#define APP_ENCODER_H_

// need this for GPIO typedefs
#include "stm32f1xx_hal.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "EvtQueue.h"

// define encoder pins
// clock and data are from the silkscreen on the encoder module

#define ENC_CLK GPIO_PIN_8
#define ENC_DATA GPIO_PIN_9
#define ENC_CLK_BANK GPIOB
#define ENC_DATA_BANK GPIOB

extern uint8_t reg_r; // this is in main
int encoder1_read(void);
int encoder_read(GPIO_TypeDef *clk_bank, uint16_t clk_pin, GPIO_TypeDef *data_bank, uint16_t data_pin, uint8_t invert);


#endif /* APP_ENCODER_H_ */
