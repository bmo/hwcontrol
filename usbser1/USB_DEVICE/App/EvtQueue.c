/**
 * \file
 * \author	<a href="http://www.innomatic.ca">innomatic</a>
 * \copyright <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons Licence" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
 */
// https://github.com/innomatica/pushbtn

#include "EvtQueue.h"
#include "UsrTimer.h"

extern void HAL_SuspendTick(void);
extern void HAL_ResumeTick(void);

#define ADVANCE_QPTR(x)     ((x+1) % EVT_QDEPTH)

static struct
{
	uint8_t buff[EVT_QDEPTH][EVT_QWIDTH];
	uint8_t head;
	uint8_t tail;
} evt_queue;


/**
 * Append a new event at the end of the queue. If the queue is full, then
 * the event is ignored and it returns with false.
 *
 * \param  event data in an array of uint8_t
 * \return false if the queue is full
 */
bool Evt_EnQueue(uint8_t *event)
{
	unsigned i;
	uint8_t next = ADVANCE_QPTR(evt_queue.head);

	// queue is full
	if(next == evt_queue.tail)
	{
		// event will be lost
		return false;
	}

	// copy event bytes into the buffer
	for(i = 0; i < EVT_QWIDTH; i++)
	{
		evt_queue.buff[evt_queue.head][i] = event[i];
	}
	// move to the next position
	evt_queue.head = next;

	return true;
}

/**
 * Retrieve the oldest event from the queue. If the return value is false
 * the retrieved event data should be ignored. Note that the access of the
 * queue is protected by HAL_SuspendTick / Hal_ResumeTick. If any other
 * interrupt service routine were to access the queue through Evt_EnQueue,
 * corresponding interrupt should be suspended here.
 *
 * \param  event data in an array of uint8_t
 * \return false if the queue is empty
 */
bool Evt_DeQueue(uint8_t *event)
{
	uint8_t i;
	bool flag = false;

	// disable all timers
	UsrTimer_Enable(false);

	// queue is not empty
	if(evt_queue.tail != evt_queue.head)
	{
		// copy event bytes into the buffer
		for(i = 0; i < EVT_QWIDTH; i++)
		{
			event[i] = evt_queue.buff[evt_queue.tail][i];
		}
		// move to the next position
		evt_queue.tail = ADVANCE_QPTR(evt_queue.tail);
		// set flag
		flag = true;
	}

	// enable all timers
	UsrTimer_Enable(true);

	// return with the flag
	return flag;
}

/**
 * The tail and the head pointers are set to zero. This will invalidate all
 * the data in the queue.
 */
void Evt_InitQueue(void)
{
	// clear queue by resetting the pointers
	evt_queue.head = evt_queue.tail = 0;
}

