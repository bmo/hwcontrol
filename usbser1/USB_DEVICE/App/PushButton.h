/*
 * PushButton.h
 *
 *  Created on: Mar 14, 2021
 *      Author: https://github.com/innomatica/pushbtn
 */

#ifndef APP_PUSHBUTTON_H_
#define APP_PUSHBUTTON_H_
/**
 * \file
 * \author	<a href="http://www.innomatic.ca">innomatic</a>
 * \brief	Pushbutton state check
 * \copyright <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons Licence" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
 *
 * This program keeps track of up and down state of the buttons by reading the
 * pushbtn_param structure and records the time passed after the last change
 * of the state. The main routine, PushButton_Routine() is supposed to be
 * called at each PUSHBTN_TMR_PERIOD. The PushButton_Init() fuction does the
 * job by registering the main routine to the UserTimer.
 *
 * Internally, each pushbutton state (on/off or up/down) takes one bit of
 * a uint8_t button state byte, whose value should be supplied by the user
 * function PushButton_Read(). In other word, user must write the function
 * that reads the state of all the pushbuttons (up to 8) and returns with a
 * single bytes of data, each bit of which represents the up/down state of
 * certain pushbutton.
 *
 * Arrangement of the bit location in the button state byte is arbitrary
 * however the value should be consistent and should be matched with the mask
 * byte that is registered with PushButton_Init() call.
 *
 * The main routine, PushButton_Routine() keeps track of the button state
 * and post an event when certain conditions are met. In the current
 * implementation, it will generate following events: single click, double
 * click, triple click and long click.
 *
\code
// override PushButton_Read() function
uint8_t PushButton_Read()
{
	uint8_t button_state;

	// read the state of all buttons up to 8

	return button_state;
}

main()
{
	// initialize the pushbutton check routine with the mask byte
	PushButton_Init(0x01);

	while(1)
	{
		if(Evt_DeQueue(event))
		{
			switch(event[0])
			{
				// pushbutton event
				case EVT_PBTN_INPUT:
					// process pushbutton input event
					...

				default:
					break;
			}
		}
	}
}
\endcode
 */

#ifndef __PUSH_BUTTON_H
#define __PUSH_BUTTON_H

#include <stdint.h>
#include <stdbool.h>

/// PushButton_Routine timer period in msec
#define PUSHBTN_TMR_PERIOD		5  // 80
/// Criteria for determination of short click and long click
#define PUSHBTN_TO_SHORT		0x14 	// 3 * PUSHBTN_TMR_PERIOD
#define PUSHBTN_TO_LONG			0x64	// 10 * PUSHBTN_TMR_PERIOD
#define PUSHBTN_TO_MAX			4080	// (16 * 255) maximum duration count

#define PUSHBTN_MODE_CLICK		0x00	// click mode: detect change
#define PUSHBTN_MODE_UDOWN		0x01	// up down mode: detect level

/// Initialize the pushbutton data
void PushButton_Init(uint8_t mask);
/// Switch detect mode between CLICK and UDOWN
void PushButton_SetMode(uint8_t mode, bool flag);
/// Read all pushbutton state and return them in a uint8_t form
uint8_t PushButton_Read(void) __attribute__((weak));

#endif // __PUSH_BUTTON_H



#endif /* APP_PUSHBUTTON_H_ */
