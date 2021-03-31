/*
 * myevents.h
 *
 *  Created on: Mar 14, 2021
 *      Author: https://github.com/innomatica/pushbtn
 */

#ifndef APP_MYEVENTS_H_
#define APP_MYEVENTS_H_
#ifndef __MY_EVENTS_H
#define __MY_EVENTS_H

/** Pushbutton input event
 *
 * Event Data: (PBTN_ID)(EVT_TYPE)
 *
 *	* PBTN_ID: id of the pushbutton that generated the event
 *	* EVT_TYPE: type of the event such as single click, double click,
 */
#define EVT_PBTN_INPUT			0x10		///< event code for pushbutton input
#define PBTN_SCLK				0x01		///< single click
#define PBTN_LCLK				0x02		///< long click
#define PBTN_DCLK				0x03		///< double click
#define PBTN_TCLK				0x04		///< triple click
#define PBTN_DOWN				0x05		///< button state down
#define PBTN_ENDN				0x06		///< button state changed to up

#define EVT_ENCODER_UP			0x30
#define	EVT_ENCODER_DOWN		0x31

/** UART packet received
 *
 * Event Data: (UART PACKET)
 */

// System control
#define SYS_SRESET			0x00    ///< perform system reset
#define SYS_WRESET			0x01	///< perform watchdog reset
#define SYS_STPMOD			0x02	///< enter stop mode
#define SYS_DSLMOD			0x03	///< enter deep sleep mode
#define SYS_SLPMOD			0x04	///< enter sleep mode

#define EVT_UART_RXPKT		0x20		///< event code for UART RX packet
#define EVT_USB_DATA		0x40	///< event code for USB data received from host
#endif // __MY_EVENTS_H



#endif /* APP_MYEVENTS_H_ */
