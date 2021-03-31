/*
 * EvtQueue.h
 *
 *  Created on: Mar 14, 2021
 *      Author: https://github.com/innomatica/pushbtn
 */

#ifndef APP_EVTQUEUE_H_
#define APP_EVTQUEUE_H_
/**
 * \file
 * \author	<a href="http://www.innomatic.ca">innomatic</a>
 * \brief	FIFO ring buffer for event data
 * \copyright <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons Licence" style="border-width:0" src="https://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.
 *
 * This program controls the FIFO ring buffer that is to contain event data.
 * However any similar data of an array of uint8_t can use this routine as
 * long as the size does not exceed the width of the queue (EVT_QWIDTH).
 * It should be noted that the interpretation of the contents is in the hand
 * of the user.
 *
 * In the case of event scenario, events are supposed to be posted by various
 * functions and then consumed by the main state machine.  Each event consists
 * of one byte of event code and event data whose size varies depending on the
 * event code. For the meaning of the data, please refer to the definiiton of
 * each event code.
\verbatim
(EVT_CODE)(EVT_DATA)
	* EVT_CODE: one byte that represent the source of the event
	* EVT_DATA: event data bytes whose length varies
\endverbatim
 *
 * Code Example:
 *
\code
bool flag;
uint8 event[EVT_QWIDTH];

// event code byte: pushbutton input event
event[0] = EVT_PBTN_INPUT;
// event data byte: pushbutton id
event[1] = 0x01:
// event data byte: event type
event[2] = PBTN_SCLK:
// post event
flag = Cmd_EnQueue(event);
// do something if the registration fails
if(flag == false)
{
    ...
}

// retrieve the oldest event from the FIFO
flag = Cmd_EnQueue(event);
// false return value means the queue is empty
if(flag)
{
    // valid event retrieved.
    ...
}

\endcode
 */

#ifndef __EVT_QUEUE_H
#define __EVT_QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include "myevents.h"


/** Maximum number of events the queue can hold.  This number should be less
 * than 256.
 */
#define EVT_QDEPTH				(8)
/** The maximum size of the event data. It consists of one byte of event code
 * with variable length of data bytes.
 */
#define MAX_USB_PAYLOAD			(64)			// TODO put this in a different .h
#define EVT_QWIDTH				(16+MAX_USB_PAYLOAD)


/// Register a new event
bool Evt_EnQueue(uint8_t *event);
/// Checkout the oldest event
bool Evt_DeQueue(uint8_t *event);
/// Initialize the event queue
void Evt_InitQueue(void);

#endif // __EVT_QUEUE_H



#endif /* APP_EVTQUEUE_H_ */
