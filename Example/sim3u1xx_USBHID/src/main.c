//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------
// library
#include <stdio.h>
// hal
#include <si32_device.h>
#include <SI32_PBSTD_A_Type.h>
// application
#include "gModes.h"
#include "myCapsense0.h"
#include "myCpu.h"
#include "myUsbDevice.h"
#include "myUSB0.h"

//==============================================================================
// myApplication.
//==============================================================================
int main()
{
   // msTicks increments every 1ms (1Khz). Driven by boot osc (myCpu.c)
   // _last variables store the last seen state of variables so we know when they have changed
   uint32_t msTicks_last=0xffffffff;
   volatile int transmitKeyLast=-1;
  
   // increments every main loop;
   uint32_t count=0;

  // Enter the default operating mode for this application
  enter_default_mode_from_reset();

  //Run initial capsens basline caibration
  calibrate_capsense();

  // Connect to USB
  USB0_connect();

   //PERFORM THE FOLLOWING TASKS FOREVER
   while (1)
   {
      // if msTicks has changed
      if (msTicks != msTicks_last)
      {
         // SAMPLE CAPOUCH EVERY .1S
         if (!(msTicks % 100))
         {
            // TRIGGER CAPSENSE
            scan_slider();
            if( CapsenseTouch && (transmitKeyLast!=CapsenseTouchPos))
            {
              // key-down event.
              transmitKeyLast = CapsenseTouchPos ;
              myHidTransmitKey(transmitKeyLast);
            }
            else if ( (transmitKeyLast!=-1) && (!CapsenseTouch) )
            {
              // key-up event.
              transmitKeyLast = -1;
              myHidTransmitKey(16) ;
            }
         }           


         // UPDATE STATUS OF TOUCH DETECTED LED
         SI32_PBSTD_A_write_pins_masked(SI32_PBSTD_2,
                                                 (!CapsenseTouch) << 0xB,
                                                 0x800);

         // save current msTicks value as last seen
        msTicks_last = msTicks;
     }// if msTicks changed

     // EVERY 2^10 counts
     if (!(count % (1024)))
     {
        // TURN ON LED driver (P2.10)
        SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x400);
     }

     // 2^CapsenseTouchPos counts after led driver on
     if ((count % 1024) == (1 << CapsenseTouchPos))
     {
        // TURN OFF LED driver (P2.10)
        SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x400);
        // Led brightness will now be 1 of 10 levels determined by the
        // captouch slider (time on doubles betweeneach level)
     }

     count++;
   }//while(1)
}

//-eof--------------------------------------------------------------------------

