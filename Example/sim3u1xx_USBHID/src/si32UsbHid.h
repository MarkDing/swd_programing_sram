//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------

#ifndef __si32UsbHID_h__
#define __si32UsbHID_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// HID Class Codes
#define  USB_HID                       (0x03)

// HID Subclass Codes

// HID Protocol Codes

// HID Class-specific descriptor types
#define  USB_HID_TYPE                   0x21
#define  USB_HID_REPORT_TYPE            0x22



//-----------------------------------------------------------------------------
// Standard Class Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbHidReportStruct
{
   // Total length of report descriptor table Low Byte
   uint16_t wItemLength;
   // Report descriptor conents
   uint8_t  * Descriptor;
} si32UsbHidReportType;

typedef struct si32UsbHidStruct
{
   uint8_t  bCountryCode;                  // Localized country code
   uint8_t  bNumDescriptors;               // Number of class descriptors to follow
   si32UsbHidReportType ** ReportDescriptors;
} si32UsbHidType;

#pragma pack(1)
typedef struct si32UsbHidReportDescriptorStruct
{
   // Report descriptor type (USB_HID_REPORT_TYPE=0x22)
   uint8_t  bReportDescriptorType;
   // Total length of report descriptor table Low Byte
   uint16_t wItemLength;
} si32UsbHidReportDescriptorType;

typedef struct si32UsbHidDescriptorStruct
{
  // Size of this Descriptor in Bytes (=9)  (6 + 3 of first report descriptor
  uint8_t  bLength;
  // Descriptor Type ( USB_HID_TYPE = 0x21)
  uint8_t  bDescriptorType;
  // HID Class Specification release number Low Byte
  uint16_t bcdHIDL;
  // Localized country code
  uint8_t  bCountryCode;
  // Number of class descriptors to follow
  uint8_t  bNumDescriptors;
  // ------------------------------------------------- // 
  // Report descriptor type (USB_HID_REPORT_TYPE=0x22)
  uint8_t  bReportDescriptorType;
  // Total length of report descriptor table Low Byte
  uint16_t wItemLength;
} si32UsbHidDescriptorType;


#pragma pack()


// HID Request Codes
#define  USBHID_GET_REPORT       0x01
#define  USBHID_GET_IDLE         0x02
#define  USBHID_GET_PROTOCOL     0x03
#define  USBHID_SET_REPORT       0x09
#define  USBHID_SET_IDLE         0x0A
#define  USBHID_SET_PROTOCOL     0x0B

// HID Class Requests
#define USBHID_IFC_GET_REPORT    USB_REQUEST_CODE(USB_REQUEST_DIRECTION_IN, USB_REQUEST_TYPE_CLASS, USB_REQUEST_RECIPIENT_INTERFACE, USBHID_GET_REPORT)
#define USBHID_IFC_SET_REPORT    USB_REQUEST_CODE(USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_CLASS, USB_REQUEST_RECIPIENT_INTERFACE, USBHID_SET_REPORT)
#define USBHID_IFC_GET_IDLE      USB_REQUEST_CODE(USB_REQUEST_DIRECTION_IN, USB_REQUEST_TYPE_CLASS, USB_REQUEST_RECIPIENT_INTERFACE, USBHID_GET_IDLE)
#define USBHID_IFC_SET_IDLE      USB_REQUEST_CODE(USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_CLASS, USB_REQUEST_RECIPIENT_INTERFACE, USBHID_SET_IDLE)
#define USBHID_IFC_GET_PROTOCOL  USB_REQUEST_CODE(USB_REQUEST_DIRECTION_IN, USB_REQUEST_TYPE_CLASS, USB_REQUEST_RECIPIENT_INTERFACE, USBHID_GET_PROTOCOL)

// USBHID Specific Request Formats
typedef struct si32UsbHidRequestStruct
{
  union
  {
    si32UsbSetupType Setup;
    struct si32UsbHidGetReportRequest
    {
      // wRequest Code
      uint16_t wRequest;
      // wValue decoded
      uint16_t wValueLo :8;
      uint16_t wValueHi :8;
      // wIndex
      uint8_t wIndexLo;
      uint8_t wIndexHi;
      // wlength
      uint16_t wLength;
    } HidRequestStruct;
  };
} si32UsbHidRequestType;
#endif // __si32UsbHID_h__

//-eof--------------------------------------------------------------------------
