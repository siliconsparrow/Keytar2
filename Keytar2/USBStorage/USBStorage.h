/*
 * USBStorage.h - Manage USB access to the SD card.
 *
 *  Created on: 30 Dec 2020
 *      Author: adam
 */

#ifndef USBSTORAGE_USBSTORAGE_H_
#define USBSTORAGE_USBSTORAGE_H_

class USBStorage
{
public:
	static USBStorage *instance();

	USBStorage();

	void poll();

private:
	bool _sdToUsb;

public:
	void sdCardToUsb();
};

#endif // USBSTORAGE_USBSTORAGE_H_
