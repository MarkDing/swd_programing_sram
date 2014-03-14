//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------


#ifndef  __myUsbDevice__
#define  __myUsbDevice__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "si32Usb.h"
#include "si32UsbHid.h"

#define USB_VENDOR_ID_SILICON_LABS        (0x10C4)
#define USB_PRODUCT_ID__SI32_USBHID       (0xff00)

#pragma pack(1)
typedef struct myUsbConfigurationDescriptorsStruct
{
  si32UsbConfigurationDescriptorType configuration;
  si32UsbInterfaceDescriptorType     interface;
  si32UsbHidDescriptorType           hid;
  si32UsbEndpointDescriptorType      endpoint_interrupt_in_1;
} myUsbConfigurationDescriptorsType ;
#pragma pack()

//------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------
extern void myUsbDevice_request_handler(void);
extern void myHidTransmitKey(int key_index);

extern const si32UsbDeviceDescriptorType myUsbDeviceDescriptor;
extern const myUsbConfigurationDescriptorsType myUsbConfigurationDescriptors;
extern const si32UsbHidReportDescriptorType myUsbHidReportDescriptor;
extern const si32UsbHidDescriptorType myUsbHidDescriptor;
extern const uint8_t myUsbHidKeypadUsage[];

#endif

//-eof--------------------------------------------------------------------------
