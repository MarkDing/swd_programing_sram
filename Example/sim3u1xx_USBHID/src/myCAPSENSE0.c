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
#include <si32_device.h>
#include <SI32_CAPSENSE_A_Type.h>
// application
#include "myCAPSENSE0.h"

#define printf(...)

// PUBLIC VARIABLES
uint8_t CapsenseTouchPos;
bool CapsenseTouch;

// CAPSENS CALCULATION and INFO STRUCT
struct capsensTracking_struct
{
  bool scanning;
  uint32_t chan;
  uint32_t channels[6];
  uint32_t readings[6];
  uint32_t baseline[6];
} CS_info;

//==============================================================================
//2nd LEVEL INTERRUPT HANDLERS
//==============================================================================
void calculate_position(uint8_t); //defined below
void my_convert_complete_handler(void)
{
    // SAVE READING
    CS_info.readings[CS_info.chan] = SI32_CAPSENSE_A_read_data(SI32_CAPSENSE_0);

    // CLEAR INTERRUPT AND DISABLE MODULE
    SI32_CAPSENSE_A_clear_conversion_complete_interrupt(SI32_CAPSENSE_0);
    SI32_CAPSENSE_A_disable_module(SI32_CAPSENSE_0);

    // IF WE HAVE CHANNELS LEFT TO SCAN
    if (CS_info.chan < 5)
    {
       // POINT TO NEXT CHANNEL
       SI32_CAPSENSE_A_write_mux(SI32_CAPSENSE_0, CS_info.channels[++CS_info.chan]);
       SI32_CAPSENSE_A_connect_capsense_channel(SI32_CAPSENSE_0);

       // AND START CONVERSION
       SI32_CAPSENSE_A_enable_module(SI32_CAPSENSE_0);
       SI32_CAPSENSE_A_start_manual_conversion(SI32_CAPSENSE_0);

    }
    else
    {
      // PRINT CARRAGE RETURN
      printf("\n");

      // DISCONNECT FROM PINS
      SI32_CAPSENSE_A_connect_capsense_channel(SI32_CAPSENSE_0);

      // AND CALCULATE POSITION (interpolate 5 codes per segment)
      calculate_position(2);
    }
}

//------------------------------------------------------------------------------
void my_scan_complete_handler(void)
{
  /*DO NOTHING*/
}

//------------------------------------------------------------------------------
void scan_slider(void)
{
   // RESET CHANNEL POINTER
   CS_info.chan = 0;

   // CONNECT TO FIRST CHANNEL
   SI32_CAPSENSE_A_write_mux(SI32_CAPSENSE_0, 0);
   SI32_CAPSENSE_A_connect_capsense_channel(SI32_CAPSENSE_0);

   // START CONVERSION
   SI32_CAPSENSE_A_enable_module(SI32_CAPSENSE_0);
   SI32_CAPSENSE_A_start_manual_conversion(SI32_CAPSENSE_0);
}

//------------------------------------------------------------------------------
void calculate_position(uint8_t scale)
{
  uint32_t samples = sizeof(CS_info.readings) / 4;
  uint32_t maxVal = 0;
  uint32_t maxPos = 0xFF;
  uint32_t pos;
  uint32_t range = (samples - 1)*scale;
  uint32_t top, bot, tscale, bscale;
  uint32_t sum;

  // For each virtual position
  for (pos=0; pos<range; pos++)
  {
    // Calculate positions and interpolation fraction
    bot = pos/scale;
    top = bot + 1;
    tscale = (pos % scale) + 1;
    bscale = scale - (pos % scale);

    // Calculate code for interpolated virtual position
    sum = (CS_info.readings[bot] - CS_info.baseline[bot]) * bscale
        + (CS_info.readings[top] - CS_info.baseline[top]) * tscale;


    // update touch if this is the highest code measured
    // and it is high enough above baseline
    if ((sum > maxVal)
        && (CS_info.readings[bot] >  ((CS_info.baseline[bot] + 0x100) * 1.20)
            || CS_info.readings[top] >  ((CS_info.baseline[top] + 0x100) * 1.20)))
    {
      maxVal = sum;
      maxPos = pos;
    }

  }// foreach virtual positon

  // IF VALID TOUCH SENSED
  if (maxPos <0x80)
  {
     // MARK TOUCH SENSED AND SAVE SGMENT ACTIVE
     CapsenseTouch = 1;
     CapsenseTouchPos = maxPos;

     // PRINT MEASURED SEGMENT TO IDE
     printf("0x%d\n", CapsenseTouchPos);
  }
  else
  {
     // MARK TOUCH NOT SENSED
     CapsenseTouch = 0;
  }
}

//------------------------------------------------------------------------------
void calibrate_capsense(void)
{
   uint32_t i;

   //DISABLE INTERUPTS
   NVIC_DisableIRQ(CAPSENSE0_IRQn);

   //INIT MANUAL SCAN STRUCTURE
   CS_info.chan = 0;
   CS_info.channels[0] = 0x0;
   CS_info.channels[1] = 0x1;
   CS_info.channels[2] = 0x2;
   CS_info.channels[3] = 0x3;
   CS_info.channels[4] = 0x8;
   CS_info.channels[5] = 0x9;

   //CREATE INITIAL BASELINE (manual scan)
   for (i=0; i<sizeof(CS_info.channels) / 4; i++)
   {
       //CONNECT TO CHANNELS AND START CONVERSION
       SI32_CAPSENSE_A_write_mux(SI32_CAPSENSE_0, CS_info.channels[i]);
       SI32_CAPSENSE_A_connect_capsense_channel(SI32_CAPSENSE_0);
       SI32_CAPSENSE_A_enable_module(SI32_CAPSENSE_0);
       SI32_CAPSENSE_A_start_manual_conversion(SI32_CAPSENSE_0);

       //WAIT FOR CONVERT DONE
       while(SI32_CAPSENSE_A_is_conversion_in_progress(SI32_CAPSENSE_0));
       SI32_CAPSENSE_A_disable_module(SI32_CAPSENSE_0);

       //SET INITIAL BASELINE TO MEASURED - 0x100 MARGIN
       CS_info.baseline[i] = SI32_CAPSENSE_A_read_data(SI32_CAPSENSE_0) - 0x100;
   }

   //ENABLE INTERRUPTS
   NVIC_ClearPendingIRQ(CAPSENSE0_IRQn);
   NVIC_EnableIRQ(CAPSENSE0_IRQn);
}

//-eof--------------------------------------------------------------------------
