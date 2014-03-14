//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <SI32_USB_A_Type.h>
#include <SI32_USBEP_A_Type.h>
#include <si32_device.h>

#include "si32Usb.h"
#include "myUsbDevice.h"
#include "myUSB0.h"



//-----------------------------------------------------------------------------
// Descriptor Declarations
//-----------------------------------------------------------------------------
const uint8_t myUsbHidKeypadUsage[]=
{
  0x05, 0x01,                         // USAGE_PAGE (Generic Desktop)
  0x09, 0x07,                         // USAGE (Keypad)
  0xa1, 0x01,                         // COLLECTION (Application)
  0x05, 0x07,                         //   USAGE_PAGE (Keyboard)
  0x19, 0x00,                         //   USAGE_MINIMUM (Reserved (no event indicated))
  0x29, 0x65,                         //   USAGE_MAXIMUM (Keyboard Application)
  0x15, 0x00,                         //   LOGICAL_MINIMUM (0)
  0x25, 0x65,                         //   LOGICAL_MAXIMUM (101)
  0x75, 0x08,                         //   REPORT_SIZE (8)
  0x95, 0x01,                         //   REPORT_COUNT (1)
  0x81, 0x00,                         //   INPUT (Data,Ary,Abs)
  0xc0
};


const si32UsbDeviceDescriptorType myUsbDeviceDescriptor =
{
  .bLength             = 0x12,
  .bDescriptorType     = 0x01,
  .bcdUsb              = 0x0200,              // bcdUSB
  .bDeviceClass        = 0x00,                // bDeviceClass
  .bDeviceSubclass     = 0x00,                // bDeviceSubClass
  .bDeviceProtocol     = 0x00,                // bDeviceProtocol
  .bMaxPacketSize0     = EP0_MAX_PACKET_SIZE, // bMaxPacketSize0
  .wIdVendor           = 0xffff,              // idVendor
  .wIdProduct          = 0xfffe,              // idProduct
  .bcdDevice           = 0x0100,              // bcdDevice
  .bIndexManufacturer  = 0x00,                // iManufacturer
  .bIndexProduct       = 0x00,                // iProduct
  .bIndexSerialNumber  = 0x00,                // iSerialNumber
  .bNumConfigurations  = 0x01                 // bNumConfigurations
}; //end of DeviceDesc

#define CONFIG_DESC_SIZE ( sizeof(myUsbConfigurationDescriptors) )

const myUsbConfigurationDescriptorsType myUsbConfigurationDescriptors =
{
  .configuration.bLength             = 9,
  .configuration.bDescriptorType     = USB_DESCRIPTOR_TYPE_CONFIG,
  .configuration.wTotalLength        = CONFIG_DESC_SIZE,
  .configuration.bNumInterfaces      = 0x01,
  .configuration.bConfigurationValue = 1,           // bConfigurationValue
  .configuration.bMaxPower           = 50,          // MaxPower (in 2mA units)
  .configuration.bIndexConfiguration = 0,           // iConfiguration
  .configuration.bmAttributes        = 0xC0,        // bmAttributes

  .interface.bLength                 = 9,
  .interface.bDescriptorType         = USB_DESCRIPTOR_TYPE_INTERFACE,
  .interface.bInterfaceNumber        = 0x00,
  .interface.bAlternateSetting       = 0x00,        // bAlternateSetting
  .interface.bNumEndpoints           = 0x01,
  .interface.bInterfaceClass         = USB_HID,     // bInterfaceClass
  .interface.bInterfaceSubClass      = 0x00,        // bInterfaceSubClass
  .interface.bInterfaceProtocol      = 0x00,        // bInterfaceProcotol
  .interface.iInterface              = 0x00,        // iInterface

  .hid.bLength                       = 9, // 6 + first 3 of the next descriptor
  .hid.bDescriptorType               = USB_HID_TYPE,
  .hid.bcdHIDL                       = 0x0100,
  .hid.bCountryCode                  = 0,
  .hid.bNumDescriptors               = 1,
  .hid.bReportDescriptorType         = USB_HID_REPORT_TYPE,
  .hid.wItemLength                   = sizeof(myUsbHidKeypadUsage),

  .endpoint_interrupt_in_1.bLength                 = 7,
  .endpoint_interrupt_in_1.bDescriptorType         = USB_DESCRIPTOR_TYPE_ENDPOINT,
  .endpoint_interrupt_in_1.bEndpointAddress        = 0x81,       // bEndpointAddress
  .endpoint_interrupt_in_1.Type                    = USB_EP_ATTRIBUTES_TYPE_INTERRUPT,       // bmAttributes.type
  .endpoint_interrupt_in_1.wMaxPacketSize          = 64,         // MaxPacketSize Low
  .endpoint_interrupt_in_1.bInterval               = 0x01        // bInterval
};




//------------------------------------------------------------------------------
// From Hid Usage Table 1.1
// http://www.usb.org/developers/devclass_docs/Hut1_11.pdf
// Page 54,55
//
uint8_t s_KeyTable[17] =
{   // Keypad 0,1,2
  0x62, 0x59, 0x5a,
  // 3,4,5
  0x5b, 0x5c, 0x5d,
  // 6,7,8
  0x5e, 0x5f, 0x60,
  // 9, '.'
  0x61, 0x63,
};
#pragma pack()


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void myHidTransmitKey(int key_index)
{
  if (myUSB0_ep1_state == EPN_DISABLED)
    return;

  while (myUSB0_ep1_state != EPN_IDLE)
    ;

  myUSB0_ep1_state = EPN_BUSY;
  USB0_EPn_write_fifo(SI32_USB_0_EP1, &s_KeyTable[key_index], 1);
  SI32_USBEP_A_set_in_packet_ready(SI32_USB_0_EP1);

}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void myUsbDevice_request_handler(void)
{
  switch (myUSB0_setup.wRequest)
  {
  case USB_REQUEST_STANDARD_DEVICE_GET_DESCRIPTOR: // Support device, configuration, string
    if (myUSB0_setup.wValue == 0x0100)
    {
      myUSB0_ep0_data_pointer = (uint8_t *)&myUsbDeviceDescriptor;
      myUSB0_ep0_data_size    = _min(myUSB0_setup.wValue, sizeof(myUsbDeviceDescriptor));
      myUSB0_ep0_state        = EP0_START_IN_DATA;
    }
    else if (myUSB0_setup.wValue == 0x0200)
    {
      myUSB0_ep0_data_pointer = (uint8_t *)&myUsbConfigurationDescriptors;
      myUSB0_ep0_data_size    = _min(myUSB0_setup.wValue, sizeof(myUsbConfigurationDescriptors));
      myUSB0_ep0_state        = EP0_START_IN_DATA;
    }
    else
    {
      myUSB0_ep0_state = EP0_SEND_STALL;
    }
    break;

  case USB_REQUEST_STANDARD_DEVICE_SET_ADDRESS:
    SI32_USB_A_write_faddr(SI32_USB_0, (0x7F & myUSB0_setup.wValue));
    myUSB0_ep0_state = EP0_NODATA_STATUS;
    break;

  case USB_REQUEST_STANDARD_DEVICE_SET_CONFIG:
  case USB_REQUEST_STANDARD_DEVICE_SET_INTERFACE:
    SI32_USBEP_A_set_endpoint_direction_in(SI32_USB_0_EP1);
    SI32_USBEP_A_clear_in_data_underrun(SI32_USB_0_EP1);
    SI32_USBEP_A_select_in_bulk_interrupt_mode(SI32_USB_0_EP1);
    SI32_USBEP_A_stop_in_stall(SI32_USB_0_EP1);
    SI32_USBEP_A_reset_in_data_toggle(SI32_USB_0_EP1);
    SI32_USBEP_A_set_in_max_packet_size(SI32_USB_0_EP1, 64>>3);
    SI32_USB_A_enable_ep1(SI32_USB_0);
    myUSB0_ep1_state=EPN_IDLE;
    myUSB0_ep0_state = EP0_NODATA_STATUS;
    break;


  case USB_REQUEST_STANDARD_INTERFACE_GET_DESCRIPTOR:
    if (myUSB0_setup.wValue == 0x2200)
    {
      myUSB0_ep0_data_pointer = (uint8_t *)&myUsbHidKeypadUsage;
      myUSB0_ep0_data_size    = _min(myUSB0_setup.wValue, sizeof(myUsbHidKeypadUsage));
      myUSB0_ep0_state        = EP0_START_IN_DATA;
      break;
    }

  default:
    myUSB0_ep0_state = EP0_SEND_STALL;
    break;
  }
}

//-eof--------------------------------------------------------------------------
