//-----------------------------------------------------------------------------
// Init.c
//-----------------------------------------------------------------------------
// Copyright 2013 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains routines for initialization functions.
//
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include "Init.h"
#include "32bit_prog_defs.h"

//-----------------------------------------------------------------------------
// Internal Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

void WDT_Init (void);
void SYSCLK_Init (void);
void PORT_Init (void);
void Timer0_Init (void);
void PCA0_Init (void);

//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Internal prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Internal global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Initialization Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// WDT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine disables the WDT.
//
//-----------------------------------------------------------------------------
void WDT_Init (void)
{
   PCA0MD &= ~0x40;
}

//-----------------------------------------------------------------------------
// Oscillator_Init 12 MHz intosc * 4 = 48 MHz
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This selects the clock multiplier as the system clock source.  The input
// to the multiplier is the 12 MHz internal oscillator.
// Also sets Flash read timing for 50 MHz operation.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
    U16 i = 0;

    FLSCL     = 0x90;                  // Set Flash read timing for 50 MHz

    OSCICN    = 0x83;                  // Enable intosc for div 1 mode

    CLKMUL    = 0x80;                  // Enable clkmul

    for (i = 0; i < 20; i++);          // Wait at least 5us for init

    CLKMUL    |= 0xC0;

    while ((CLKMUL & 0x20) == 0);

    CLKSEL    = 0x03;                  // Enable CLKMUL as sysclk
}

//-----------------------------------------------------------------------------
// Port_Init UART0
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure ports as follows:
// P0.4 - TX0 (push-pull)   -- UART0 TX pin
// P0.5 - RX0 (open-drain)  -- UART0 RX pin
//
// P2.2 - LED0 (push-pull)  -- status indicator
// P2.3 - LED1 (push-pull)  -- status indicator
//
//
// The pinout is optimized to map the standard Port I/O header to the
// standard 10-pin debug connector, as follows:
// Pin 1: VREF         P1.0
// Pin 2: SWDIO/TMS    P1.1
// Pin 3: ground       P1.2
// Pin 4: SWCLK/TCK    P1.3
// Pin 5: ground       P1.4
// Pin 6: SWO/TDO      P1.5
// Pin 7: NC           P1.6
// Pin 8: TDI          P1.7
// Pin 9: ground       GND
// Pin 10: RESETB      P2.1
//
//-----------------------------------------------------------------------------
void Port_Init(void)
{
  XBR1 &= ~0x40;  // Disable the crossbar

                  //                                                         nSRST_In/
  //////////////////                   UART0 RX, UART TX,                    nSRST_Out
  P0 = 0xFF;      //                    1        1                            1
  P0MDOUT = 0x12; //                    OD       PP                           PP
  P0MDIN = 0xFF;  //                    D        D                            D
  P0SKIP = 0x02;  //                    RX0      TX0                          x
                  //                                     SWCLK_Out/          SWDIO_In/
  ////////////////// TDI_Out,          TDO_IN,  ground,   TCK_Out,  ground,  TMS_Out, VREF
  P1 = 0xEB;      //  1        1        1        0          1        0        1        1
  P1MDOUT = 0x88; //  PP       OD       OD       OD         PP       OD       OD       OD
  P1MDIN = 0xFE;  //  D        D        D        D          D        D        D        A
  P1SKIP = 0xFF;  //  x        x        x        x          x        x        x        x
  //////////////////                                       LED1,    LED0,
  P2 = 0xFF;      //                                        1        1
  P2MDOUT = 0x0C; //                                        PP       PP
  P2MDIN = 0xFF;  //                                        D        D
  P2SKIP = 0x0C;  //                                        x        x

  XBR0 = 0x00;                         // Enable UART0 on the crossbar
  XBR1 = 0x40;                         // Enable the crossbar, which also
                                       // enables port outputs
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

