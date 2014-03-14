//
// Copyright (c) 2013 SILICON LABORATORIES, INC.
//
// FILE NAME    : 32bit_prog_defs.h
// DESCRIPTION  : ARM Serial Wire debug interface header file
//

#ifndef _32BIT_PROG_DEFS_
#define _32BIT_PROG_DEFS_

#include <compiler_defs.h>
#include <C8051F380_defs.h>

//-----------------------------------------------------------------------------
// Project Constants
//-----------------------------------------------------------------------------

// System clock frequency in Hz
#define  SYSCLK                 48000000

#define BOOL bit
#define TRUE (1 == 1)
#define FALSE (!TRUE)

// Delay Times
#define  DELAY100mS             -( SYSCLK / 48 * 0.1 )
#define  DELAY20mS              -( SYSCLK / 48 * 0.02 )
#define  DELAY5mS               -( SYSCLK / 48 * 0.005 )
#define  DELAY1mS               -( SYSCLK / 48 * 0.001 )
#define  DELAY100uS             -( SYSCLK / 48 * 0.000100 )
#define  DELAY75uS              -( SYSCLK / 48 * 0.000075 )
#define  DELAY25uS              -( SYSCLK / 48 * 0.000025 )
#define  DELAY10uS              -( SYSCLK / 48 * 0.00001 )
#define  DELAY4_3uS             -( SYSCLK / 48 * 0.0000043 )
#define  DELAY3uS               -( SYSCLK / 48 * 0.000003 )
#define  DELAY2uS               -( SYSCLK / 48 * 0.000002 )

// Timeout Times
#define  TIMEOUT_2Sec           -( SYSCLK / 48 * 2 )

// Command Status Response Codes
#define HOST_COMMAND_OK         0x55
#define HOST_INVALID_COMMAND    0x80
#define HOST_COMMAND_FAILED     0x81
#define HOST_AP_TIMEOUT         0x82
#define HOST_WIRE_ERROR         0x83
#define HOST_ACK_FAULT          0x84
#define HOST_DP_NOT_CONNECTED   0x85

// Property SRST values
#define SRST_ASSERTED           0x1
#define SRST_DEASSERTED         0x0

#define DAP_RETRY_COUNT         255

//-----------------------------------------------------------------------------
// ARM Debug Interface Constants
//-----------------------------------------------------------------------------

// ARM CoreSight SWD-DP packet request values
#define SW_IDCODE_RD            0xA5
#define SW_ABORT_WR             0x81
#define SW_CTRLSTAT_RD          0x8D
#define SW_CTRLSTAT_WR          0xA9
#define SW_RESEND_RD            0x95
#define SW_SELECT_WR            0xB1
#define SW_RDBUFF_RD            0xBD

// ARM CoreSight SW-DP packet request masks
#define SW_REQ_PARK_START       0x81
#define SW_REQ_PARITY           0x20
#define SW_REQ_A32              0x18
#define SW_REQ_RnW              0x04
#define SW_REQ_APnDP            0x02

// ARM CoreSight SW-DP packet acknowledge values
#define SW_ACK_OK               0x1
#define SW_ACK_WAIT             0x2
#define SW_ACK_FAULT            0x4
#define SW_ACK_PARITY_ERR       0x8

// ARM CoreSight DAP command values
#define DAP_IDCODE_RD           0x02
#define DAP_ABORT_WR            0x00
#define DAP_CTRLSTAT_RD         0x06
#define DAP_CTRLSTAT_WR         0x04
#define DAP_SELECT_WR           0x08
#define DAP_RDBUFF_RD           0x0E

// ARM CoreSight DAP command masks
#define DAP_CMD_PACKED          0x80
#define DAP_CMD_A32             0x0C
#define DAP_CMD_RnW             0x02
#define DAP_CMD_APnDP           0x01
#define DAP_CMD_MASK            0x0F

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

// These pin assignments match the debug adapter cable
//
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


// LED Pin Definitions
SBIT(LED0, SFR_P2, 2);                 // Green LED
SBIT(LED1, SFR_P2, 3);                 // Green LED

// JTAG Pin Definitions
SBIT(TDO_In, SFR_P1, 5);               // TDO Input
SBIT(TDI_Out, SFR_P1, 7);              // TDI Output
SBIT(TMS_Out, SFR_P1, 1);              // TMS Output
SBIT(TCK_Out, SFR_P1, 3);              // TCK Output

// Serial Wire Pin Definitions
SBIT(SWDIO_Out, SFR_P1, 1);            // SWDIO Output
SBIT(SWDIO_In, SFR_P1, 1);             // SWDIO Input
SBIT(SWCLK_Out, SFR_P1, 3);            // SWCLK Output
// SWO on P0.5 with TDO

// Reset Pin Definitions
SBIT(nSRST_Out, SFR_P0, 1);            // nSRST Output
SBIT(nSRST_In, SFR_P0, 1);             // nSRST Input

// These pins are ground on the CoreSight debug connector
SBIT(P1_2, SFR_P1, 2);
SBIT(P1_4, SFR_P1, 4);

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

// Serial Wire Interface Macros
#define  _SetSWPinsIdle             { P1MDOUT |= 0x08; P1MDOUT &= ~0x22; P1 |= 0x2A; }
#define  _SetSWDIOasInput           { P1MDOUT &= ~0x02; P1 |= 0x02; }
#define  _SetSWDIOasOutput          P1MDOUT |= 0x02

// Set all debug pins xPIN_OUT to 1 (open drain outputs off)
#define  _ResetDebugPins            { P1MDOUT &= ~0x8A; P1 |= 0x8A; P0MDOUT &= ~0x02; P0 |= 0x02; }

// Target Reset Control Macros
#define  _AssertTargetReset         nSRST_Out = 0
#define  _ReleaseTargetReset        nSRST_Out = 1
#define  _IsTargetReset             (nSRST_In == 1)

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

typedef unsigned char STATUS;

//-----------------------------------------------------------------------------
// SWD-DP Interface Functions
//-----------------------------------------------------------------------------
void    SWD_Initialize (void);
STATUS  SWD_Configure (U8 dp_type);
STATUS  SWD_Connect (void);
STATUS  SWD_Disconnect (void);
STATUS  SWD_LineReset (void);
STATUS  SWD_ClearErrors (void);
STATUS  SWD_DAP_Move(U8, U8, U32 *);

STATUS  SW_Response (U8);
void    SW_DAP_Read(U8, U8, U32 *);
void    SW_DAP_Write(U8, U8, U32 *, BOOL);
U8      SW_Request(U8);
BOOL    SW_CalcDataParity(void);
U8      SW_ShiftPacket(U8, U8);
void    SW_ShiftByteOut(U8);
U8      SW_ShiftByteIn(void);
void    SW_ShiftReset(void);

#endif // _32BIT_PROG_DEFS
