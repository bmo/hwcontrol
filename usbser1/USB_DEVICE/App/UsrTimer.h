/*
 * UsrTimer.h
 *
 *  Created on: Mar 14, 2021
 *      Author: https://github.com/innomatica/pushbtn
 */

#ifndef APP_USRTIMER_H_
#define APP_USRTIMER_H_

/**
 * \file
 * \author	<a href="http://www.innomatic.ca">innomatic</a>
 * \brief	Software timer for running time-critical tasks.
 * \copyright <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons Licence" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
 * \warning Each timer callback function should take no longer than one
 *			period of the base timer.
 *
 * This routine allows a task to be run at a specific period for a specific
 * number of times. The main timer routine UsrTimer_Routine() must reside
 * in a hardware timer callback funcition, whose period will determine the
 * base unit for the UsrTimer. For example, if you put the main routine
 * inside the SysTick callback:
\code
void HAL_SYSTICK_Callback(void)
{
	UsrTimer_Routine();
}
\endcode
 * then the base tick will be 1msec. The interval and the duration of
 * corresponding UserTimer will have this 1msec as base unit.
 *
 * If you want your routine to be called every 10msec 5 times, then
\code
void my_routine(void);
...
UsrTimer_Set(10, 5, my_routine);
\endcode
 * If the duration is zero, then it will run indefinitely.
 *
 */
#ifndef __USRTIMER_H
#define __USRTIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_USRTIMER            20	///< maximum number of timers

typedef void (* usrtimer_callback)(void);

/// Initialize all timers
void UsrTimer_Init();
/// Enable or disable main routine
void UsrTimer_Enable(bool flag);
/// Clear the timer
void UsrTimer_Clear(uint32_t index);
/// Pause the timer
void UsrTimer_Pause(uint32_t index);
/// Resume the timer
void UsrTimer_Resume(uint32_t index);
/// Main timer routine
void UsrTimer_Routine(void);
/// Set a new timer with the callback function
int UsrTimer_Set(uint32_t interval, uint32_t duration, usrtimer_callback f);

#endif // __USR_TIMER_H


#endif /* APP_USRTIMER_H_ */
