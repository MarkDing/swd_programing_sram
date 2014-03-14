//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------
// library
// hal
// application
#include "gUSB0.h"
#include "gCPU.h"
#include <stdint.h>
#include <SI32_USB_A_Type.h>
#include <SI32_USBEP_A_Type.h>
#include <SI32_CLKCTRL_A_Type.h>
#include <si32_device.h>


//==============================================================================
// 2nd Level Interrupt Handlers
//==============================================================================
extern void USB0_start_of_frame_handler(void);
extern void USB0_resume_handler(void);
extern void USB0_reset_handler(void);
extern void USB0_suspend_handler(void);
extern void USB0_ep0_handler(void);
extern void USB0_ep1_in_handler(void);
extern void USB0_ep1_out_handler(void);
extern void USB0_ep2_in_handler(void);
extern void USB0_ep2_out_handler(void);
extern void USB0_ep3_in_handler(void);
extern void USB0_ep3_out_handler(void);
extern void USB0_ep4_in_handler(void);
extern void USB0_ep4_out_handler(void);

//==============================================================================
//1st LEVEL  INTERRUPT HANDLERS
//==============================================================================
void USB0_IRQHandler(void)
{
  uint32_t usbCommonInterruptMask = SI32_USB_A_read_cmint(SI32_USB_0);
  uint32_t usbEpInterruptMask = SI32_USB_A_read_ioint(SI32_USB_0);

  SI32_USB_A_write_cmint(SI32_USB_0, usbCommonInterruptMask);
  SI32_USB_A_write_ioint(SI32_USB_0, usbEpInterruptMask);

  if (usbEpInterruptMask & SI32_USB_A_IOINT_EP0I_MASK)
  {
    USB0_ep0_handler();
  }

  if (usbEpInterruptMask & SI32_USB_A_IOINT_IN1I_MASK )
  {
    USB0_ep1_in_handler();
  }

  // Handle Start of Frame Interrupt
  if (usbCommonInterruptMask & SI32_USB_A_CMINT_SOFI_MASK)
  {
    USB0_start_of_frame_handler();
  }

  // Handle Resume Interrupt
  if (usbCommonInterruptMask & SI32_USB_A_CMINT_RESI_MASK)
  {
    USB0_resume_handler();
  }

  // Handle Reset Interrupt
  if (usbCommonInterruptMask & SI32_USB_A_CMINT_RSTI_MASK)
  {
    USB0_reset_handler();
  }

  // Handle Suspend interrupt
  if (usbCommonInterruptMask & SI32_USB_A_CMINT_SUSI_MASK)
  {
    USB0_suspend_handler();
  }
}


//==============================================================================
// Configuration Functions
//==============================================================================

void USB0_enter_default_mode()
{
  SI32_USB_A_enable_usb_oscillator(SI32_USB_0);

  // Perform asynchronous reset of the USB module
  SI32_USB_A_reset_module (SI32_USB_0);

  // Enable Endpoint 0 interrupts
  SI32_USB_A_write_cmint (SI32_USB_0, 0x00000000);
  SI32_USB_A_write_ioint (SI32_USB_0, 0x00000000);
  SI32_USB_A_enable_ep0_interrupt (SI32_USB_0);

  // Enable Reset, Resume, Suspend interrupts
  //  SI32_USB_A_enable_suspend_interrupt (SI32_USB_0);
  //  SI32_USB_A_enable_resume_interrupt (SI32_USB_0);
  SI32_USB_A_enable_reset_interrupt (SI32_USB_0);
  //  SI32_USB_A_enable_start_of_frame_interrupt (SI32_USB_0);

  // Enable Transceiver, fullspeed
  SI32_USB_A_write_tcontrol (SI32_USB_0, 0x00);
  SI32_USB_A_select_transceiver_full_speed (SI32_USB_0);
  SI32_USB_A_enable_transceiver (SI32_USB_0);

  // Enable clock recovery, single-step mode disabled
  SI32_USB_A_enable_clock_recovery (SI32_USB_0);
  SI32_USB_A_select_clock_recovery_mode_full_speed (SI32_USB_0);
  SI32_USB_A_select_clock_recovery_normal_cal  (SI32_USB_0);

   // Enable USB interrupts
   NVIC_EnableIRQ(USB0_IRQn);

   // Enable the module
   SI32_USB_A_enable_module(SI32_USB_0);
}


//==============================================================================
// Support Functions
//==============================================================================



//-eof--------------------------------------------------------------------------
