/*
 * USBMidi.h
 *
 *  Created on: 20 Sep 2020
 *      Author: adam
 */

#ifndef USBMIDI_H_
#define USBMIDI_H_

#include "usbh_def.h"

class USBMidi
{
public:
	static USBMidi *instance();

	USBMidi();

	void poll();

private:
	enum CDC_Demo_State {
	  CDC_DEMO_IDLE = 0,
	  CDC_DEMO_WAIT,
	  CDC_DEMO_SEND,
	  CDC_DEMO_RECEIVE,
	};

	enum CDC_Send_State {
	  CDC_SEND_IDLE = 0,
	  CDC_SEND_WAIT,
	};

	enum CDC_Receive_State {
	  CDC_RECEIVE_IDLE = 0,
	  CDC_RECEIVE_WAIT,
	  CDC_RECEIVE_RECEIVE,
	};

	struct CDC_DEMO_StateMachine {
	  __IO CDC_Demo_State          state;
	  __IO CDC_Send_State          Send_state;
	  __IO CDC_Receive_State       Receive_state;
	  __IO uint8_t                 select;
	  __IO uint8_t                 lock;
	};

	typedef enum {
	  CDC_SELECT_MENU = 0,
	  CDC_SELECT_FILE ,
	  CDC_SELECT_CONFIG,
	}CDC_DEMO_SelectMode;

	typedef enum {
	  APPLICATION_IDLE = 0,
	  APPLICATION_START,
	  APPLICATION_READY,
	  APPLICATION_RUNNING,
	  APPLICATION_DISCONNECT,
	}CDC_ApplicationTypeDef;

	USBH_HandleTypeDef hUSBHost;
	CDC_ApplicationTypeDef Appli_state = APPLICATION_IDLE;
	CDC_DEMO_StateMachine CdcDemo;


	void userProcess(USBH_HandleTypeDef *phost, uint8_t id);
	void CDC_MenuProcess();

	static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
};

#endif // USBMIDI_H_
