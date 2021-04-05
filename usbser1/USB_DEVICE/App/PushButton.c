/**
 * \file
 * \author	<a href="http://www.innomatic.ca">innomatic</a>
 * \copyright <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons Licence" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
 */

#include "PushButton.h"
#include "EvtQueue.h"
#include "UsrTimer.h"

typedef struct
{
	uint8_t old_state;		///< button state old
	uint8_t new_state;		///< button state new
	uint8_t click[8];		///< number of button click
	uint16_t duration[8];	///< time passed since last click
	uint8_t mask;			///< mask for buttons to be ignored
	uint8_t mode;			///< click mode / updown mode
	bool flag;				///< internal flag to prevent false detection
} pushbtn_param;

pushbtn_param pp;
static void PushButton_Routine(void);
static void PushButton_ClearLog(uint8_t index);

/** Clear the parameter struct and register the main routine to the UserTimer
 * by calling UsrTimer_Set() function. The period of the routine,
 * PUSHBTN_TMR_PERIOD should be tuned by checking mechanical characteristics
 * (bouncing effect) of the button.
 *
 * \param	mask the mask byte that denotes which bit of the uint8_t data
 * supplied by PushButton_Read() function are assigned to the actual push
 * button.
 */
void PushButton_Init(uint8_t mask)
{
	int i ;

	// clear data
	pp.old_state = pp.new_state = 0;
	pp.mask = mask;
	pp.mode = 0;
	pp.flag = false;

	// clear log
	for(i = 0; i < 8; i++)
	{
		PushButton_ClearLog(i);
	}

	// register pushbutton main routine
	UsrTimer_Set(PUSHBTN_TMR_PERIOD, 0, PushButton_Routine);
}

/** In CLICK mode, the change of the button state is detected. In UDOWN mode
 * the up or down state of the button is detected.
 *
 * \param mode either PUSHBTN_MODE_CLICK or PUSHBTN_MODE_UDOWN
 * \param flag true if the button whose mode is changes is currently being
 *			pressed.
 */
void PushButton_SetMode(uint8_t mode, bool flag)
{
	int i;

	// clear data
	pp.old_state = pp.new_state = 0;
	// this looks weird but correct
	pp.flag = !flag;

	// clear log
	for(i = 0; i < 8; i++)
	{
		PushButton_ClearLog(i);
	}

	// change mode
	pp.mode = mode;
}

/** Clear the duration parameter and the click count parameter of the button.
 */
void PushButton_ClearLog(uint8_t index)
{
	if(index < 8)
	{
		pp.click[index] = pp.duration[index] = 0;
	}
}

/** Main routine
 */
void PushButton_Routine()
{
	int i;
	uint8_t diff_state;
	uint8_t event[EVT_QWIDTH];

	extern uint8_t reg_s;

	pp.new_state = PushButton_Read();

	// difference in the button state
	diff_state = pp.old_state ^ pp.new_state;

	for(i = 0; i < 8; i++)
	{
		// skip if no button is assigned
		if(((pp.mask >> i) & 0x01) == 0x00)
		{
			continue;
		}

		// up-down mode
		if(((pp.mode >> i) & 0x01)  == PUSHBTN_MODE_UDOWN)
		{
			// the button pressed
			if(((pp.new_state >> i) & 0x01) == 0x01)
			{
				event[0] = EVT_PBTN_INPUT;
				event[1] = (uint8_t)(i);
				event[2] = PBTN_DOWN;

				// post the event as long as the button is pressed down
				Evt_EnQueue(event);
			}
			// button released
			else
			{
				// actually it has just risen
				if(((pp.old_state >> i) & 0x01) == 0x01)
				{
					event[0] = EVT_PBTN_INPUT;
					event[1] = (uint8_t)(i);
					event[2] = PBTN_ENDN;

					// post the event to indicate the end of the down state
					Evt_EnQueue(event);
				}
			}
		}
		// click mode
		else
		{
			// the button state changed
			if((diff_state >> i) & 0x01)
			{
				// (re)start duration count
				pp.duration[i] = 1;

				// the button released
				if(((pp.new_state >> i) & 0x01) == 0x00)
				{
					if(pp.flag)
					{
						pp.flag = false;
						//
						pp.duration[i] = 0;
					}
					else
					{
						// increase click count
						pp.click[i]++;
					}
				}
			}
			// button state not changed
			else
			{
				// increase duration count
				if((pp.duration[i] > 0) &&
						(pp.duration[i] < PUSHBTN_TO_MAX))
				{
					pp.duration[i]++;
				}
			}

			// triple click
			if(pp.click[i] >= 3)
			{
				// triple click event
				event[0] = EVT_PBTN_INPUT;
				event[1] = (uint8_t)(i);
				event[2] = PBTN_TCLK;
				// post event
				Evt_EnQueue(event);

				// clear log
				PushButton_ClearLog(i);
			}
			// button relased and short timeout passed
			else if((pp.duration[i] > PUSHBTN_TO_SHORT) &&
					(((pp.new_state >> i) & 0x01) == 0x00))
			{
				event[0] = EVT_PBTN_INPUT;
				event[1] = (uint8_t)(i);

				// double click
				if(pp.click[i] == 2)
				{
					// double click event
					event[2] = PBTN_DCLK;
				}
				// single click
				else
				{
					// single click event
					event[2] = PBTN_SCLK;
				}
				// post the event
				Evt_EnQueue(event);

				// clear log
				PushButton_ClearLog(i);
			}
			// button pressed and long timeout passed
			else if((pp.duration[i] > PUSHBTN_TO_LONG) &&
					(((pp.new_state >> i) & 0x01) == 0x01))
			{
				// long click event
				event[0] = EVT_PBTN_INPUT;
				event[1] = (uint8_t)(i);
				event[2] = PBTN_LCLK;

				// post the event
				Evt_EnQueue(event);

				// clear log
				PushButton_ClearLog(i);

				// raise flag: this will prevent false detect after long click
				pp.flag = true;
			}
		}
	}
	// update pin state
	pp.old_state = pp.new_state;
}


