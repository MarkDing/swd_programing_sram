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
#include <si32_device.h>
#include <SI32_CLKCTRL_A_Type.h>
#include <SI32_PBSTD_A_Type.h>
#include <SI32_WDTIMER_A_Type.h>
// application
#include "myCpu.h"

//------------------------------------------------------------------------------
// This function is invoked by the CMSIS requires SysemInit() function in
// system_<device>.c.  SystemInit() is invoked by Reset_Handler() when the
// CPU boots.
void mySystemInit()
{
   // disable the watchdog timer to prevent device resets
   // any lines added to this function should be added below this line of code
   SI32_WDTIMER_A_stop_counter (SI32_WDTIMER_0);

   // the following lines of code are necessary to enable debug printf
   // note that this pin changes between the different package options

   // enable APB clock to the Port Bank module
   SI32_CLKCTRL_A_enable_apb_to_modules_0 (SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0CEN_MASK);
   // make the SWO pin (PB1.3) push-pull to enable SWV printf
   SI32_PBSTD_A_set_pins_push_pull_output (SI32_PBSTD_1, (1<<3));
}

//-eof--------------------------------------------------------------------------
