//------------------------------------------------------------------------------
// Copyright (c) 2011 by Silicon Laboratories Inc.  All rights reserved.
// The program contained in this listing is proprietary to Silicon Laboratories,
// headquartered in Austin, Texas, U.S.A. and is subject to worldwide copyright
// protection, including protection under the United States Copyright Act of 1976
// as an unpublished work, pursuant to Section 104 and Section 408 of Title XVII
// of the United States code.  Unauthorized copying, adaptation, distribution,
// use, or display is prohibited by this law.
//
// Silicon Laboratories provides this software solely and exclusively
// for use on Silicon Laboratories' microcontroller products.
//
// This software is provided "as is".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// SILICON LABORATORIES SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
// INCIDENTAL, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
//------------------------------------------------------------------------------
// library
#include <stdio.h>
// hal
#include <si32_device.h>
#include <SI32_CLKCTRL_A_Type.h>
#include <SI32_WDTIMER_A_Type.h>
#include <SI32_PBCFG_A_Type.h>
#include <SI32_PBSTD_A_Type.h>
#include "config.h"
volatile uint32_t msTicks;

/* other code*/
//==============================================================================
//1st LEVEL  INTERRUPT HANDLERS
//==============================================================================
void SysTick_Handler(void)
{
   msTicks++;
   /*NO SENCOND LEVEL HANDERL SPESIFIED*/
}

void mySystemInit(void)
{
   SI32_WDTIMER_A_stop_counter (SI32_WDTIMER_0);
   // Enable the APB clock to the PB registers
   SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0);
   SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_1, 0x00000008);

   SI32_PBCFG_A_enable_crossbar_1(SI32_PBCFG_0);
   SI32_PBCFG_A_enable_crossbar_0(SI32_PBCFG_0);
   SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_2,0x00000300);
   // Enable the LED drivers (P2.10, P2.11)
   SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000C00);
   SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_1, 0x00000003);
   SysTick_Config(SystemCoreClock / 1000);
   // set Priority for Cortex-M0 System Interrupts.
   NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

}



//==============================================================================
// myApplication.
//==============================================================================
int main()
{
	unsigned int time_out;
	while(1) {
		time_out = msTicks + 1000;
		while(time_out > msTicks);
		SI32_PBSTD_A_toggle_pins(SI32_PBSTD_2, 0xC00);
		SI32_PBSTD_A_toggle_pins(SI32_PBSTD_1, 0x003);
	}
}
//---eof------------------------------------------------------------------------
