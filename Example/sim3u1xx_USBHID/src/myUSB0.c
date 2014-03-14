//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------

#include "myUSB0.h"
#include "si32Usb.h"
#include "myUsbDevice.h"

#include <SI32_USB_A_Type.h>
#include <SI32_USBEP_A_Type.h>
#include <SI32_CLKCTRL_A_Type.h>
#include <si32_device.h>

extern void myUsbDevice_request_handler(void);

volatile uint32_t myUSB0_sof_count;

volatile uint32_t myUSB0_device_state = USB_DETACHED;
si32UsbSetupType myUSB0_setup;

volatile uint32_t  myUSB0_ep0_state = EP0_DISABLED;
uint8_t * myUSB0_ep0_data_pointer;
uint32_t  myUSB0_ep0_data_size = 0;

volatile uint32_t  myUSB0_ep1_state = EPN_DISABLED;




//==============================================================================
// 2nd Level Interrupt Handlers (Called from generated code)
//==============================================================================
void UsbStateMachine(void);

//------------------------------------------------------------------------------
void USB0_ep0_handler(void)
{
   uint32_t * SetupPointer = (uint32_t *)&myUSB0_setup.wRequest;
   uint32_t ControlReg = SI32_USB_A_read_ep0control(SI32_USB_0);

   if(ControlReg & SI32_USB_A_EP0CONTROL_STSTLI_MASK)
   {
      SI32_USB_A_clear_stall_sent_ep0(SI32_USB_0);
      myUSB0_ep0_state = EP0_HALT;
   }

   if(ControlReg & SI32_USB_A_EP0CONTROL_SUENDI_MASK)
   {
      SI32_USB_A_clear_setup_end_early_ep0(SI32_USB_0);
      myUSB0_ep0_state = EP0_WAIT_SETUP;
   }

   if(ControlReg & SI32_USB_A_EP0CONTROL_OPRDYI_MASK)
   {
      if(EP0_HALT == myUSB0_ep0_state)
      {
         myUSB0_ep0_state = EP0_WAIT_SETUP;
      }

      if(EP0_WAIT_SETUP != myUSB0_ep0_state)
      {
         while(1);
      }
      *SetupPointer++ = SI32_USB_A_read_ep0_fifo_u32(SI32_USB_0);
      *SetupPointer   = SI32_USB_A_read_ep0_fifo_u32(SI32_USB_0);
      myUSB0_ep0_state = EP0_PROCESS_SETUP;
      myUsbDevice_request_handler();
   }


   UsbStateMachine();
}

//------------------------------------------------------------------------------
void USB0_ep1_in_handler(void)
{
  myUSB0_ep1_state = EPN_IDLE;
}

//------------------------------------------------------------------------------
void USB0_start_of_frame_handler(void)
{
}

//------------------------------------------------------------------------------
void USB0_resume_handler(void)
{
}

//------------------------------------------------------------------------------
void USB0_reset_handler(void)
{
  myUSB0_device_state = USB_DEFAULT;
  myUSB0_ep0_state = EP0_WAIT_SETUP;
  SI32_USB_A_enable_ep0(SI32_USB_0);
}

//------------------------------------------------------------------------------
void USB0_suspend_handler(void)
{
}


//==============================================================================
// Support Functions
//==============================================================================
//------------------------------------------------------------------------------
void UsbStateMachine(void)
{
  uint32_t count;
   switch(myUSB0_ep0_state)
   {
      case EP0_NODATA_STATUS:
         SI32_USB_A_clear_out_packet_ready_ep0(SI32_USB_0);
         myUSB0_ep0_state = EP0_WAIT_SETUP;
         break;

      case EP0_START_IN_DATA:
         SI32_USB_A_clear_out_packet_ready_ep0(SI32_USB_0);
         myUSB0_ep0_state = EP0_CONTINUE_IN_DATA;
         // fall through
      case EP0_CONTINUE_IN_DATA:
         count = _min(myUSB0_ep0_data_size, myUSB0_setup.wLength);

         if(count > EP0_MAX_PACKET_SIZE)
         {
            count = USB0_EP0_write_fifo(myUSB0_ep0_data_pointer, EP0_MAX_PACKET_SIZE);
            myUSB0_ep0_data_pointer += count;
            myUSB0_ep0_data_size    -= count;
         }
         else
         {
            USB0_EP0_write_fifo(myUSB0_ep0_data_pointer, count);
            SI32_USB_A_set_data_end_ep0(SI32_USB_0);
            myUSB0_ep0_state = EP0_WAIT_SETUP;
         }
         SI32_USB_A_set_in_packet_ready_ep0(SI32_USB_0);
         break;

      case EP0_SEND_STALL:
         SI32_USB_A_clear_out_packet_ready_ep0(SI32_USB_0);
         SI32_USB_A_send_stall_ep0(SI32_USB_0);
         break;

      case EP0_START_OUT_DATA:
      case EP0_OUT_STATUS:
      default:
         break;
   }
}

//------------------------------------------------------------------------------
void USB0_connect(void)
{
   myUSB0_device_state = USB_ATTACHED;
   myUSB0_ep0_state = EP0_WAIT_SETUP;
   SI32_USB_A_enable_internal_pull_up(SI32_USB_0);
}

//------------------------------------------------------------------------------
void USB0_disconnect()
{
   SI32_USB_A_disable_internal_pull_up(SI32_USB_0);
}

//------------------------------------------------------------------------------
void USB0_stop()
{
   // Disable USB interrupts
   NVIC_DisableIRQ(USB0_IRQn);

   // Reset USB0 module, which disables it also
   SI32_USB_A_reset_module(SI32_USB_0);

   // Disable USB clocks here
   myUSB0_device_state = USB_DETACHED;
   myUSB0_ep0_state = EP0_DISABLED;
}

//------------------------------------------------------------------------------
uint32_t USB0_EP0_read_fifo(uint8_t * dst,  uint32_t count)
{
  uint32_t * pTmp32;
  uint32_t result;
  count = _min( count, SI32_USB_A_read_ep0_count(SI32_USB_0) );
  result=count;
  while( (((uint32_t) dst) & 0x3) && count)
  {
    *dst++=SI32_USB_A_read_ep0_fifo_u8(SI32_USB_0);
    count--;
  };

  pTmp32=(uint32_t*) dst;
  while(count>3)
  {
    *pTmp32++=SI32_USB_A_read_ep0_fifo_u32(SI32_USB_0);
    count-=4;
  }
  dst = (uint8_t*) pTmp32;
  while(count)
  {
    *dst++=(uint32_t) SI32_USB_A_read_ep0_fifo_u8(SI32_USB_0);
    count--;
  }
  return result;
}



//------------------------------------------------------------------------------
uint32_t USB0_EP0_write_fifo(uint8_t * src, uint32_t count)
{
  uint32_t * pTmp32;
  uint32_t result;
  result=count=_min( count, EP0_MAX_PACKET_SIZE);

   // ensure src buffer is 32-bit aligned
   while( (((uint32_t) src) & 0x3) && count)
   {
     SI32_USB_A_write_ep0_fifo_u8(SI32_USB_0, *src);
     src++;
     count--;
   }

   // use 32-bit fifo writes, now that we know that the src buffer
   // is aligned.
   pTmp32=(uint32_t*) src;
   while( count > 3 )
   {
     SI32_USB_A_write_ep0_fifo_u32(SI32_USB_0, *pTmp32);
     pTmp32++;
     count-=4;
   }
   src = (uint8_t*) pTmp32;

   // Write out any residue bytes 1 byte at a time
   while(count)
   {
     SI32_USB_A_write_ep0_fifo_u8(SI32_USB_0, *src);
     src++;
     count--;
   }
  return result;
}


//------------------------------------------------------------------------------
uint32_t USB0_EPn_read_fifo(SI32_USBEP_A_Type *ep,  uint8_t * dst,  uint32_t count)
{
  uint32_t * pTmp32;
  uint32_t result;
  count = _min( count, SI32_USBEP_A_read_data_count(ep) );
  result=count;

  while ( (((uint32_t) dst) & 0x3) && count)
  {
    *dst++=SI32_USBEP_A_read_fifo_u8(ep);
    count--;
  };

  pTmp32=(uint32_t*) dst;
  while (count>3)
  {
    *pTmp32++=SI32_USBEP_A_read_fifo_u32(ep);
    count-=4;
  }
  dst = (uint8_t*) pTmp32;
  while (count)
  {
    *dst++=(uint32_t) SI32_USBEP_A_read_fifo_u8(ep);
    count--;
  }
  return result;
}



//------------------------------------------------------------------------------
uint32_t USB0_EPn_write_fifo(SI32_USBEP_A_Type *ep, uint8_t * src, uint32_t count)
{
  uint32_t * pTmp32;
  uint32_t result;
  result=count=_min( count, (SI32_USBEP_A_get_in_max_packet_size(ep)<<3));

  // ensure src buffer is 32-bit aligned
  while ( (((uint32_t) src) & 0x3) && count)
  {
    SI32_USBEP_A_write_fifo_u8(ep, *src);
    src++;
    count--;
  }

  // use 32-bit fifo writes, now that we know that the src buffer
  // is aligned.
  pTmp32=(uint32_t*) src;
  while ( count > 3 )
  {
    SI32_USBEP_A_write_fifo_u32(ep, *pTmp32);
    pTmp32++;
    count-=4;
  }
  src = (uint8_t*) pTmp32;

  // Write out any residue bytes 1 byte at a time
  while (count)
  {
    SI32_USBEP_A_write_fifo_u8(ep, *src);
    src++;
    count--;
  }
  return result;
}


//-eof--------------------------------------------------------------------------
