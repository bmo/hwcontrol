/**
 * \file
 * \author	<a href="http://www.innomatic.ca">innomatic</a>
 * \copyright <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons Licence" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
 */

#include "UsrTimer.h"

/// Timer mode
typedef enum
{
	USRTIMER_UNASGN = 0,			///< timer is not assigned
	USRTIMER_PAUSED,				///< timer is paused
	USRTIMER_FINITE,				///< timer with finite duration
	USRTIMER_FOREVR					///< perpetual timer
} usrtimer_mode;


/// Timer structure
static struct
{
	int32_t period;
	int32_t duration;
	int32_t count;
	usrtimer_mode mode;
	usrtimer_callback callback;
} USRTimers[MAX_USRTIMER];

volatile bool usrtimer_enable = true;

/** Timer structure will be cleared.
 */
void UsrTimer_Init()
{
	unsigned i;

	// initialize the struct
	for(i = 0; i < MAX_USRTIMER; i++)
	{
		UsrTimer_Clear(i);
	}
}

/** Call this function to pause or to resume all timers at once.
 */
void UsrTimer_Enable(bool flag)
{
	usrtimer_enable = flag;
}

/** The unit of the period and the duration is determined by the period of
 * the callback function inside of which the UsrTimer_Routine() is located.
 * For example, if UsrTimer_Routine() is in the HAL_SYSTICK_Callback() then
 * the unit is 1msec. You can set arbitrary period by creating a timer of
 * your own period and put the UsrTimer_Routine() there.
 *
 * This function enumerates the timer struct array for the empty spot and
 * register the timer if possible. Then it will return the index of the
 * array. It fails and returns -1 if it cannot find any available spot.
 *
 * \param   period timer period
 * \param   duration number of repetition. 0 for perpetual timer.
 * \param   usrtimer_callback callback function
 * \return	the index of the timer created. if failed, -1 will be returned.
 */
int UsrTimer_Set(uint32_t period, uint32_t duration, usrtimer_callback f)
{
	int i;

	// check unoccupied spot
	for(i = 0; i < MAX_USRTIMER; i++)
	{
		if(USRTimers[i].mode == USRTIMER_UNASGN)
		{
			// finite duration timer
			if(duration > 0)
			{
				USRTimers[i].mode = USRTIMER_FINITE;
			}
			// perpetual timer
			else
			{
				USRTimers[i].mode = USRTIMER_FOREVR;
			}

			USRTimers[i].period = (int32_t)period;
			USRTimers[i].duration = (int32_t)duration;
			USRTimers[i].count = 0;
			USRTimers[i].callback = f;
			// return with the timer index
			return i;
		}
	}

	// no empty slot
	return -1;
}

/** This will stop the timer from further execution and clear the relevant
 * information from the timer struct array.
 *
 * \param index timer id
 */
void UsrTimer_Clear(uint32_t index)
{
	if(index >= MAX_USRTIMER)
	{
		return;
	}

	USRTimers[index].period = 0;
	USRTimers[index].duration = 0;
	USRTimers[index].count = 0;
	USRTimers[index].mode = USRTIMER_UNASGN;
	USRTimers[index].callback = NULL;
}

/** The timer will be stopped but other information is intact. It can be
 * resumed by UsrTimer_Resume().
 *
 * \param   index timer id
 */
void UsrTimer_Pause(uint32_t index)
{
	if(index >= MAX_USRTIMER)
	{
		return;
	}

	USRTimers[index].mode = USRTIMER_PAUSED;
}

/** When the timer is restarted, it will become finite timer if the
 * duration is nonzero. Otherwise it will be a perpetual timer.
 *
 * \param   index timer id
 */
void UsrTimer_Resume(uint32_t index)
{
	if(index >= MAX_USRTIMER)
	{
		return;
	}

	if(USRTimers[index].mode == USRTIMER_PAUSED)
	{
		// zero duration implies perpetual timer
		if(USRTimers[index].duration == 0)
		{
			USRTimers[index].mode = USRTIMER_FOREVR;
		}
		// nonzero duration means finite timer
		else
		{
			USRTimers[index].mode = USRTIMER_FINITE;
		}
	}
}


/** Put this routine inside of the base(hardware) timer callback function.
 *  It is highly recommended to protect the function by disabling the
 *  timer interrupt during the execution so that only one instance of this
 *  function is excuted at any moment of time.
 */
void UsrTimer_Routine(void)
{
	int i;

	if(!usrtimer_enable)
	{
		return;
	}

	// increase the counter value of each timer
	for(i = 0; i < MAX_USRTIMER; i++)
	{
		// skip unassigned or stopped timers
		if( (USRTimers[i].mode != USRTIMER_UNASGN) &&
		        (USRTimers[i].mode != USRTIMER_PAUSED) )
		{
			USRTimers[i].count++;
		}
	}

	// check the count value for the timeout
	for(i = 0; i < MAX_USRTIMER; i++)
	{
		// timeout occurred
		if(USRTimers[i].count >= USRTimers[i].period)
		{
			// the timer should have valid callback
			if(USRTimers[i].callback)
			{
				// run callback
				USRTimers[i].callback();
				// decrease the count value
				USRTimers[i].count -= USRTimers[i].period;
				// handle finite duration timer
				if(USRTimers[i].mode == USRTIMER_FINITE)
				{
					// decrease the duration
					if(--USRTimers[i].duration == 0)
					{
						UsrTimer_Clear(i);
					}
				}

				// we only handle one task at a time
				break;
			}
		}
	}
}
