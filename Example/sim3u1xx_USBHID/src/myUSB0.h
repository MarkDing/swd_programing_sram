// Copyright (c) 2012

#ifndef __MYUSB0_H__
#define __MYUSB0_H__

#include <stdbool.h>
#include <stdint.h>
#include "si32Usb.h"

// Defines
enum // For UsbState
{
   USB_DETACHED = 0,
   USB_ATTACHED,
   USB_POWERED,
   USB_DEFAULT,
   USB_ADDRESS,
   USB_CONFIGURED,
   USB_SUSPENDED,
   USB_MAX
};

enum // For Ep0State
{
   EP0_DISABLED = 0,
   EP0_WAIT_SETUP,
   EP0_PROCESS_SETUP,
   EP0_NODATA_STATUS,
   EP0_START_OUT_DATA,
   EP0_CONTINUE_OUT_DATA,
   EP0_OUT_STATUS,
   EP0_START_IN_DATA,
   EP0_CONTINUE_IN_DATA,
   EP0_IN_STATUS,
   EP0_SEND_STALL,
   EP0_HALT,
   EP0_MAX
};

enum // For EPn State
{
  EPN_DISABLED = 0,
  EPN_IDLE,
  EPN_BUSY,
};

extern void USB0_connect(void);
extern void USB0_disconnect(void);
extern void USB0_start_of_frame_handler(void);
extern void USB0_resume_handler(void);
extern void USB0_reset_handler(void);
extern void USB0_suspend_handler(void);
extern void USB0_ep0_handler(void);
extern void USB0_ep1_in_handler(void);

extern uint32_t USB0_EP0_read_fifo(uint8_t * dst,  uint32_t count);
extern uint32_t USB0_EP0_write_fifo(uint8_t * src, uint32_t count);

typedef struct SI32_USBEP_A_Struct SI32_USBEP_A_Type;
extern void USB0_EPn_handler(SI32_USBEP_A_Type * basePointer);
extern uint32_t USB0_EPn_read_fifo(SI32_USBEP_A_Type *ep,  uint8_t * dst,  uint32_t count);
extern uint32_t USB0_EPn_write_fifo(SI32_USBEP_A_Type *ep, uint8_t * src, uint32_t count);

// State variables
extern volatile uint32_t myUSB0_sof_count;
extern volatile uint32_t myUSB0_device_state;
extern si32UsbSetupType myUSB0_setup;

extern volatile uint32_t  myUSB0_ep0_state;
extern uint8_t * myUSB0_ep0_data_pointer;
extern uint32_t  myUSB0_ep0_data_size;

extern volatile uint32_t  myUSB0_ep1_state;
extern uint8_t * myUSB0_ep1_data_pointer;
extern uint32_t  myUSB0_ep1_data_size;


void UsbStateMachine(void);

#ifndef DEBUG
#define printf(...)
#else
#include <stdio.h>
#endif


#endif //__MYUSB0_H__
