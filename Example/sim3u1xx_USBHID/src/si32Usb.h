#ifndef __si32_usb_core___
#define __si32_usb_core___

#include <stdint.h>
#include <ctype.h>
#include <SI32_USBEP_A_Type.h>


#ifndef _min
#define _min(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef _max
#define _max(x,y) ((x)>(y)?(x):(y))
#endif

// Module control parameters
// #define force_low_speed true
#define force_low_speed 0


// USB Device State Machine
// ------------------------
// Define device states and events
#define   USB_DEVICE_STATE_DISCONNECTED   0x00
#define   USB_DEVICE_STATE_ATTACHED       0x01        // Device is in Attached State
#define   USB_DEVICE_STATE_POWERED        0x02        // Device is in Powered State
#define   USB_DEVICE_STATE_DEFAULT        0x03        // Device is in Default State
#define   USB_DEVICE_STATE_ADDRESSED      0x04        // Device is in Addressed State
#define   USB_DEVICE_STATE_CONFIGURED     0x05        // Device is in Configured State
#define   USB_DEVICE_STATE_SUSPENDED      0x06        // Device is in Suspended State

#define   DEV_ATTACHED       USB_DEVICE_STATE_ATTACHED
#define   DEV_POWERED        USB_DEVICE_STATE_POWERED
#define   DEV_DEFAULT        USB_DEVICE_STATE_DEFAULT
#define   DEV_ADDRESS        USB_DEVICE_STATE_ADDRESSED
#define   DEV_CONFIGURED     USB_DEVICE_STATE_CONFIGURED
#define   DEV_SUSPENDED      USB_DEVICE_STATE_SUSPENDED

#if (force_low_speed == false)
  #define EP0_MAX_PACKET_SIZE 64
#else
  #define EP0_MAX_PACKET_SIZE 8
#endif

//-----------------------------------------------------------------------------
// Standard USB Descriptor Types
//-----------------------------------------------------------------------------
#define  USB_DESCRIPTOR_TYPE_DEVICE                     0x01
#define  USB_DESCRIPTOR_TYPE_CONFIG                     0x02
#define  USB_DESCRIPTOR_TYPE_STRING                     0x03
#define  USB_DESCRIPTOR_TYPE_INTERFACE                  0x04
#define  USB_DESCRIPTOR_TYPE_ENDPOINT                   0x05
#define  USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER           0x06
#define  USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION  0x07
#define  USB_DESCRIPTOR_TYPE_INTERFACE_POWER            0x08
#define  USB_DESCRIPTOR_TYPE_OTG                        0x09
#define  USB_DESCRIPTOR_TYPE_DEBUG                      0x0A
#define  USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION      0x0B

//-----------------------------------------------------------------------------
// Standard Config Descriptor Type Definition
//-----------------------------------------------------------------------------

#pragma pack(1)
typedef struct si32PackedUsbDeviceDescriptorStruct
{
  uint8_t    bLength;            //! Size of this descriptor in bytes (18)
  uint8_t    bDescriptorType;    //! descriptor type
  uint16_t   bcdUsb;             //! USB specification release number (for version 2.00 0x0200)
  uint8_t    bDeviceClass;       //! Class code
  uint8_t    bDeviceSubclass;    //! Subclass code
  uint8_t    bDeviceProtocol;    //! Protocol code
  uint8_t    bMaxPacketSize0;    //! Maximum packet size for endpoint 0 (8, 16, 32 or 64)
  uint16_t   wIdVendor;          //! Vendor Id
  uint16_t   wIdProduct;         //! Product Id
  uint16_t   bcdDevice;          //! Device release number in BCD
  uint8_t    bIndexManufacturer; //! Index of string descriptor describing Manufacturer
  uint8_t    bIndexProduct;      //! Index of string descriptor describing Product
  uint8_t    bIndexSerialNumber; //! Index of string descriptor describing Device Serial Number
  uint8_t    bNumConfigurations; //! Number of possible configurations
} si32PackedUsbDeviceDescriptorType ;
typedef si32PackedUsbDeviceDescriptorType si32UsbDeviceDescriptorType;
#pragma pack()



//-----------------------------------------------------------------------------
// Standard Config Descriptor Type Definition
//-----------------------------------------------------------------------------
#pragma pack(1)
typedef struct si32UsbConfigurationDescriptorStruct
{
  uint8_t     bLength;                  // Size of this Descriptor in Bytes
  uint8_t     bDescriptorType;          // Descriptor Type (=4)
  uint16_t    wTotalLength;             // Total length of data returned for this config.
  uint8_t     bNumInterfaces;           // Number of Interfaces supported by this configuration
  uint8_t     bConfigurationValue;      // Value to use as an argument to the SetConfiguration() request
  uint8_t     bIndexConfiguration;      // Index of string descriptor describing this configuration
  union
  {
    struct
    {                                   // Configuration Characteristics
      uint8_t : 5;                      // -  D4..0:   Reserved (reset to zero)
      uint8_t RemoteWakeup : 1;         // -  D5:      Remote Wakeup
      uint8_t SelfPowered  : 1;         // -  D6:      Self-powered
      uint8_t : 1;                      // -  D7:      Reserved (set to one)
    } ;
    uint8_t     bmAttributes;
  };
  uint8_t     bMaxPower;                // Maximum power consumption expressed in 2mA units.
} si32UsbConfigurationDescriptorType;

typedef struct si32PackedUsbConfigurationDescriptorStruct
{
    uint8_t   bLength;                      // Size of this descriptor in bytes
    uint8_t   bDescriptorType;              // Descriptor Type
    uint16_t  wTotalLength;                 // Total length of data returned for this config.
    uint8_t   bNumInterfaces;               // Number of Interfaces supported by this configuration
    uint8_t   bConfigurationValue;          // Value to use as an argument to the SetConfiguration() request
    uint8_t   bIndexConfiguration;          // Index of string descriptor describing this configuration
    uint8_t   bmAttributes;                 // Configuration Characteristics
                                            // -  D7:      Reserved (set to one)
                                            // -  D6:      Self-powered
                                            // -  D5:      Remote Wakeup
                                            // -  D4..0:   Reserved (reset to zero)
    uint8_t   bMaxPower;                    // Maximum power consumption expressed in 2mA units.
} si32PackedUsbConfigurationDescriptorType;
#pragma pack()


//-----------------------------------------------------------------------------
// Standard String Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbStringDescriptorStruct
{
  uint8_t         bLength;                  // Size of this Descriptor in Bytes
  uint8_t         bDescriptorType;          // Descriptor Type (=3)
  uint16_t *      String;
} si32UsbStringDescriptorType;

#pragma pack(1)
//! Struct USB String Descriptor Header
typedef struct si32PackedUsbStringDescriptorStruct
{
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint16_t    pUSBString[1];        // Anchored array
} si32PackedUsbStringDescriptorType;
#pragma pack()

typedef struct si32UsbStringTableStruct
{
  si32UsbStringDescriptorType *   LanguageIDs;
  si32UsbStringDescriptorType *** LanguageArray;
} si32UsbStringTableType ;


#define _USB_STRING_DESCRIPTOR_STATIC_INIT( __array ) \
    { sizeof( (__array) )+2, 0x03, __array }

#define _USB_STRING_DESCRIPTOR_INIT_FROM_LSTRING( __string ) \
    _USB_STRING_DESCRIPTOR_STATIC_INIT( __string )

#define _USB_STRING_DESCRIPTOR_STATIC_INIT_CSTRING( __cstring ) \
    _USB_STRING_DESCRIPTOR_STATIC_INIT( L##__cstring )




//-----------------------------------------------------------------------------
// Standard Interface Descriptor Type Definition
//-----------------------------------------------------------------------------
#pragma pack(1)
typedef struct si32UsbInterfaceDescriptorStruct
{
   uint8_t  bLength;                  // Size of this Descriptor in Bytes
   uint8_t  bDescriptorType;          // Descriptor Type (=4)
   uint8_t  bInterfaceNumber;         // Number of *this* Interface (0..)
   uint8_t  bAlternateSetting;        // Alternative for this Interface (if any)
   uint8_t  bNumEndpoints;            // No of EPs used by this IF (excl. EP0)
   uint8_t  bInterfaceClass;          // Interface Class Code
   uint8_t  bInterfaceSubClass;       // Interface Subclass Code
   uint8_t  bInterfaceProtocol;       // Interface Protocol Code
   uint8_t  iInterface;               // Index of String Desc for this Interface
} si32UsbInterfaceDescriptorType;

//-----------------------------------------------------------------------------
// Packed Interface Descriptor Type Definition
//-----------------------------------------------------------------------------

typedef struct si32PackedUsbInterfaceDescriptorStruct
{
   uint8_t  bLength;                    // Size of this Descriptor in Bytes
   uint8_t  bDescriptorType;            // Descriptor Type (=4)
   uint8_t  bInterfaceNumber;           // Number of *this* Interface (0..)
   uint8_t  bAlternateSetting;          // Alternative for this Interface (if any)
   uint8_t  bNumEndpoints;              // No of EPs used by this IF (excl. EP0)
   uint8_t  bInterfaceClass;            // Interface Class Code
   uint8_t  bInterfaceSubClass;         // Interface Subclass Code
   uint8_t  bInterfaceProtocol;         // Interface Protocol Code
   uint8_t  iInterface;                 // Index of String Desc for this Interface
} si32PackedUsbInterfaceDescriptorType;
#pragma pack()



//-----------------------------------------------------------------------------
// Standard Endpoint Descriptor Type Definition
//-----------------------------------------------------------------------------

#pragma pack(1)
typedef struct si32UsbEndpointDescriptorStruct
{
  uint8_t    bLength;             // Size of this descriptor in bytes (7)
  uint8_t    bDescriptorType;     // descriptor type (5)

  union
  {
    struct
    {
      uint8_t Number : 4;         // - Bit 3..0: Endpoint number
      uint8_t : 3;                // - Bit 6..4: Reserved, reset to zero
      uint8_t Direction: 1;       // - Bit 7:    Direction. Ignored for control endpoints (0 OUT, 1 IN)
    };
    uint8_t    bEndpointAddress;  // Address of the endpoint on the USB device described by this descriptor
  } ;

  union                           // Endpoint attributes
  {
    struct
    {
      uint8_t Type :2;            //     -  bits 1..0: Transfer Type (00 Control, 01 Iso, 10 Bulk, 11 Int)
                                  #define USB_EP_ATTRIBUTES_TYPE_MASK      0x03
                                  #define USB_EP_ATTRIBUTES_TYPE_CONTROL   0
                                  #define USB_EP_ATTRIBUTES_TYPE_ISO       1
                                  #define USB_EP_ATTRIBUTES_TYPE_BULK      2
                                  #define USB_EP_ATTRIBUTES_TYPE_INTERRUPT 3
      uint8_t : 4;                //     -  bits 5..2: If not an iso endpoints, they are reserved and must be reset to zero.
                                  #define USB_EP_ATTRIBUTES_ISO_SYNCHRONOUS (0x0c)
      uint8_t : 2;                //     -  bits 7..6: Reserved bits, must be reset to zero.
    };
    uint8_t bmAttributes;
  } ;

  uint16_t   wMaxPacketSize;      // Maximum packet size. Refer to standard for bits specification
  uint8_t    bInterval;           // Interval for polling endpoint for data transfers (refer to standard)
} si32UsbEndpointDescriptorType;




typedef struct si32PackedUsbEndpointDescriptorStruct
{
  uint8_t    bLength;
  uint8_t    bDescriptorType;
  uint8_t    bEndpointAddress;
  uint8_t    bmAttributes;
  uint16_t   wMaxPacketSize;
  uint8_t    bInterval;
} si32PackedUsbEndpointDescriptorType;
#pragma pack()




//-----------------------------------------------------------------------------
// Standard Device Qualifier Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbDeviceQualifierDescriptorStruct
{
  uint8_t bLength;                // Size of this descriptor in bytes
  uint8_t bDescriptorType;        // Constant: USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER
  // TBD
} si32UsbDeviceQualifierDescriptorType;


//-----------------------------------------------------------------------------
// Standard Other-Speed Configuration Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbOtherSpeedConfigurationDescriptorStruct
{
  uint8_t bLength;                // Size of this descriptor in bytes
  uint8_t bDescriptorType;        // Constant: USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION
  // TBD
} si32UsbOtherSpeedConfigurationDescriptorType;

//-----------------------------------------------------------------------------
// Standard Interface Power Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbInterfacePowerDescriptorStruct
{
  uint8_t bLength;                // Size of this descriptor in bytes
  uint8_t bDescriptorType;        // Constant: Constant: USB_DESCRIPTOR_TYPE_INTERFACE_POWER
  // TBD
} si32UsbInterfacePowerDescriptorType;


//-----------------------------------------------------------------------------
// Standard OTG Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbOtgDescriptorStruct
{
  uint8_t bLength;                 // Size of this descriptor in bytes
  uint8_t bDescriptorType;         // Constant: USB_DESCRIPTOR_TYPE_OTG
  // TBD
} si32UsbOtgDescriptorType;


//-----------------------------------------------------------------------------
// Standard Debug Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbDebugDescriptorStruct
{
  uint8_t bLength;                // Size of this descriptor in bytes
  uint8_t bDescriptorType;        // Constant: USB_DESCRIPTOR_TYPE_DEBUG
  // TBD
} si32UsbDebugDescriptorType;


//-----------------------------------------------------------------------------
// Standard Interface Association Descriptor Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbInterfaceAssociationDescriptorStruct
{
  uint8_t bLength;                // Size of this descriptor in bytes
  uint8_t bDescriptorType;        // Constant: INTERFACE ASSOCIATION Descriptor.
  uint8_t bFirstInterface;        // Interface number of the first interface that is associated with this function.
  uint8_t bInterfaceCount;        // Number of contiguous interfaces that are associated with this function.
  uint8_t bFunctionClass;         // Class code (assigned by USB-IF).
                                  // A value of zero is not allowed in this descriptor.
                                  // If this field is FFH, the function class is vendorspecific.
                                  // All other values are reserved for assignment by the USB-IF.
  uint8_t bFunctionSubClass;      // Subclass code (assigned by USB-IF).
                                  // If the bFunctionClass field is not set to FFH all
                                  // values are reserved for assignment by the USBIF.
  uint8_t bFunctionProtocol;      // Protocol code (assigned by USB-IF). These
                                  // codes are qualified by the values of the
                                  // bFunctionClass and bFunctionSubClass fields.
  uint8_t iFunctionIndex;         // Index of string descriptor describing this function.
} si32UsbInterfaceAssociationDescriptorType;


//-----------------------------------------------------------------------------
// Setup Packet Type Definition
//-----------------------------------------------------------------------------
typedef struct si32UsbSetupStruct
{
  union
  {
    struct
    {
      uint8_t Recipient   : 2;  // Intended request recipient:  Device, Interface, Endpoint, or Other
      uint8_t             : 3;  // Reserved (should be 0)
      uint8_t Type        : 2;  // Type of request:  Standard, Class, or Vendor
      uint8_t Direction   : 1;  // Request direction - (0-OUT) Host-to-Device, or (1-IN) Device-to-Host
      uint8_t bRequest;         // The request code (meaningful only for the Recipient)
    } ;
    uint16_t wRequest;
  } ;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} si32UsbSetupType ;

typedef struct si32UsbRequestStruct
{
  union
  {
    struct si32UsbSetupStruct Setup;
    struct si32UsbGetDescriptorRequestStruct
    {
      uint16_t :16 ;
      uint8_t  DescriptorIndex;
      uint8_t  DescriptorType;
      union
      {
        uint16_t wIndex;
        uint16_t LanguageID;
      } ;
      uint16_t wLength;
    } GetDescriptorRequest ;
  };
} si32UsbRequestType;

#define USB_REQUEST_RECIPIENT_DEVICE                 (0)        // Deliver Request to USB Device
#define USB_REQUEST_RECIPIENT_INTERFACE              (1)        // Deliver Request to Specific USB Interface
#define USB_REQUEST_RECIPIENT_ENDPOINT               (2)        // Deliver Request to Specific USB Endpoint
#define USB_REQUEST_RECIPIENT_OTHER                  (3)

#define USB_REQUEST_TYPE_STANDARD                    (0)        // Standard Request Type
#define USB_REQUEST_TYPE_CLASS                       (1)        // Class-Specific Request Type
#define USB_REQUEST_TYPE_VENDOR                      (2)        // Vendor-Specific Request
#define USB_REQUEST_TYPE_RESERVED                    (3)

#define USB_REQUEST_DIRECTION_OUT                    (0)        // Host-To-Device Request
#define USB_REQUEST_DIRECTION_IN                     (1)        // Device-To-Host Request


// Standard Requests (aka USB Chapter 9 Requests)
#define USB_REQUEST_STANDARD_GET_STATUS              (0x00)
#define USB_REQUEST_STANDARD_CLEAR_FEATURE           (0x01)
#define USB_REQUEST_STANDARD_RESERVED_1              (0x02)
#define USB_REQUEST_STANDARD_SET_FEATURE             (0x03)
#define USB_REQUEST_STANDARD_RESERVED_2              (0x04)
#define USB_REQUEST_STANDARD_SET_ADDRESS             (0x05)
#define USB_REQUEST_STANDARD_GET_DESCRIPTOR          (0x06)
#define USB_REQUEST_STANDARD_SET_DESCRIPTOR          (0x07)
#define USB_REQUEST_STANDARD_GET_CONFIGURATION       (0x08)
#define USB_REQUEST_STANDARD_SET_CONFIGURATION       (0x09)
#define USB_REQUEST_STANDARD_GET_INTERFACE           (0x0A)
#define USB_REQUEST_STANDARD_SET_INTERFACE           (0x0B)
#define USB_REQUEST_STANDARD_SYNCH_FRAME             (0x0C)


#define USB_REQUEST_CODE( __direction, __type, __recipient, __code   ) \
 ( (uint16_t) \
   ( \
     ( ( ( (uint16_t) __code << 8 ) & 0xff00 ) )      |\
     ( ( ( (uint16_t) __direction ) << 7) & 0x0080 )  |\
     ( ( ( (uint16_t) __type ) << 5) & 0x0060 )       |\
     ( ( ( (uint16_t) __recipient ) ) & 0x001F )      \
   ) \
 )

// Fully qualified Standard USB Requests
#define USB_REQUEST_STANDARD_DEVICE_GET_STATUS        USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_DEVICE,    USB_REQUEST_STANDARD_GET_STATUS )
#define USB_REQUEST_STANDARD_DEVICE_CLR_FEATURE       USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_DEVICE,    USB_REQUEST_STANDARD_CLEAR_FEATURE)
#define USB_REQUEST_STANDARD_DEVICE_SET_FEATURE       USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_DEVICE,    USB_REQUEST_STANDARD_SET_FEATURE)
#define USB_REQUEST_STANDARD_DEVICE_SET_ADDRESS       USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_DEVICE,    USB_REQUEST_STANDARD_SET_ADDRESS)
#define USB_REQUEST_STANDARD_DEVICE_GET_DESCRIPTOR    USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_DEVICE,    USB_REQUEST_STANDARD_GET_DESCRIPTOR )
#define USB_REQUEST_STANDARD_DEVICE_SET_DESCRIPTOR    USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_DEVICE,    USB_REQUEST_STANDARD_SET_DESCRIPTOR )
#define USB_REQUEST_STANDARD_DEVICE_GET_CONFIG        USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_DEVICE,    USB_REQUEST_STANDARD_GET_CONFIGURATION  )
#define USB_REQUEST_STANDARD_DEVICE_SET_CONFIG        USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_DEVICE,    USB_REQUEST_STANDARD_SET_CONFIGURATION  )

#define USB_REQUEST_STANDARD_DEVICE_GET_INTERFACE     USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_INTERFACE, USB_REQUEST_STANDARD_GET_INTERFACE  )
#define USB_REQUEST_STANDARD_DEVICE_SET_INTERFACE     USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_INTERFACE, USB_REQUEST_STANDARD_SET_INTERFACE  )
#define USB_REQUEST_STANDARD_INTERFACE_GET_STATUS     USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_INTERFACE, USB_REQUEST_STANDARD_GET_STATUS )
#define USB_REQUEST_STANDARD_INTERFACE_CLR_FEATURE    USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_INTERFACE, USB_REQUEST_STANDARD_CLEAR_FEATURE)
#define USB_REQUEST_STANDARD_INTERFACE_SET_FEATURE    USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_INTERFACE, USB_REQUEST_STANDARD_SET_FEATURE)
#define USB_REQUEST_STANDARD_INTERFACE_GET_DESCRIPTOR USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_INTERFACE, USB_REQUEST_STANDARD_GET_DESCRIPTOR )
#define USB_REQUEST_STANDARD_INTERFACE_SET_DESCRIPTOR USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_INTERFACE, USB_REQUEST_STANDARD_SET_DESCRIPTOR )

#define USB_REQUEST_STANDARD_ENDPOINT_GET_STATUS      USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_ENDPOINT,  USB_REQUEST_STANDARD_GET_STATUS )
#define USB_REQUEST_STANDARD_ENDPOINT_CLR_FEATURE     USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_ENDPOINT,  USB_REQUEST_STANDARD_CLEAR_FEATURE)
#define USB_REQUEST_STANDARD_ENDPOINT_SET_FEATURE     USB_REQUEST_CODE( USB_REQUEST_DIRECTION_OUT, USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_ENDPOINT,  USB_REQUEST_STANDARD_SET_FEATURE)
#define USB_REQUEST_STANDARD_ENDPOINT_GET_DESCRIPTOR  USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_ENDPOINT,  USB_REQUEST_STANDARD_GET_DESCRIPTOR )
#define USB_REQUEST_STANDARD_ENDPOINT_SET_DESCRIPTOR  USB_REQUEST_CODE( USB_REQUEST_DIRECTION_IN,  USB_REQUEST_TYPE_STANDARD, USB_REQUEST_RECIPIENT_ENDPOINT,  USB_REQUEST_STANDARD_SET_DESCRIPTOR )


#endif // __si32_usb_component__

