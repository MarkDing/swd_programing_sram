//
// Copyright (c) 2013 SILICON LABORATORIES, INC.
//
// FILE NAME    : dp_swd.c
// TARGET MCU   : C8051F380
// DESCRIPTION  : ARM CoreSight SW-DP Interface
//
// This file implements an interface to the ARM CoreSight Serial Wire Debug
// (SWD) - Debug Port (DP).
//
#include <compiler_defs.h>
#include "32bit_prog_defs.h"

//-----------------------------------------------------------------------------
// Project Variables
//-----------------------------------------------------------------------------

// Holds the last acknowledge error from the start of a move command.
// Also used by the Serial Wire module.
U8 idata ack_error;

#if __C51__
// Note how the bit addresses are arranged to provide an endian swap.
// io_word is stored BE (matches the Keil C compliler), while the bit addresses
// are LE (matches the wire interface).
SEGMENT_VARIABLE (io_word, UU32, SEG_BDATA);

// Used to provide bit addressable data for 8-bit and smaller shift routines.
// Also used by the Serial Wire module.
SEGMENT_VARIABLE (io_byte, U8, SEG_BDATA);

SBIT (iow_0, io_word.U32, 24);
SBIT (iow_1, io_word.U32, 25);
SBIT (iow_2, io_word.U32, 26);
SBIT (iow_3, io_word.U32, 27);
SBIT (iow_4, io_word.U32, 28);
SBIT (iow_5, io_word.U32, 29);
SBIT (iow_6, io_word.U32, 30);
SBIT (iow_7, io_word.U32, 31);

SBIT (iow_8, io_word.U32, 16);
SBIT (iow_9, io_word.U32, 17);
SBIT (iow_10, io_word.U32, 18);
SBIT (iow_11, io_word.U32, 19);
SBIT (iow_12, io_word.U32, 20);
SBIT (iow_13, io_word.U32, 21);
SBIT (iow_14, io_word.U32, 22);
SBIT (iow_15, io_word.U32, 23);

SBIT (iow_16, io_word.U32, 8);
SBIT (iow_17, io_word.U32, 9);
SBIT (iow_18, io_word.U32, 10);
SBIT (iow_19, io_word.U32, 11);
SBIT (iow_20, io_word.U32, 12);
SBIT (iow_21, io_word.U32, 13);
SBIT (iow_22, io_word.U32, 14);
SBIT (iow_23, io_word.U32, 15);

SBIT (iow_24, io_word.U32, 0);
SBIT (iow_25, io_word.U32, 1);
SBIT (iow_26, io_word.U32, 2);
SBIT (iow_27, io_word.U32, 3);
SBIT (iow_28, io_word.U32, 4);
SBIT (iow_29, io_word.U32, 5);
SBIT (iow_30, io_word.U32, 6);
SBIT (iow_31, io_word.U32, 7);

SBIT (iob_0, io_byte, 0);
SBIT (iob_1, io_byte, 1);
SBIT (iob_2, io_byte, 2);
SBIT (iob_3, io_byte, 3);
SBIT (iob_4, io_byte, 4);
SBIT (iob_5, io_byte, 5);
SBIT (iob_6, io_byte, 6);
SBIT (iob_7, io_byte, 7);
#else
UU32 io_word;
volatile U8 bdata io_byte;
#endif

//-----------------------------------------------------------------------------
// Variables Declarations
//-----------------------------------------------------------------------------

// Controls SW connection sequence. 0=SW-DP, 1=SWJ-DP (use switch sequence)
U8 idata swj_dp_type;

// Even parity lookup table, holds even parity result for a 4-bit value.
const U8 code even_parity[] =
{
    0x00, 0x10, 0x10, 0x00,
    0x10, 0x00, 0x00, 0x10,
    0x10, 0x00, 0x00, 0x10,
    0x00, 0x10, 0x10, 0x00
};


//-----------------------------------------------------------------------------
// SWD Host Command Handlers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// StrobeSWCLK
//-----------------------------------------------------------------------------
//
// Pulls SWCLK high then low.
//
#define _StrobeSWCLK  { SWCLK_Out = 1; SWCLK_Out = 0; }


//-----------------------------------------------------------------------------
// SWD_Initialize
//-----------------------------------------------------------------------------
//
// Initializes SWD configuration data at startup.
//
void SWD_Initialize(void)
{
    swj_dp_type = FALSE;    // Default DP type is DP-SW
}

//-----------------------------------------------------------------------------
// (0x20) SWD_Configure
//-----------------------------------------------------------------------------
//
// Sets the debug port (DP) type to either Serial Wire only or Serial Wire JTAG.
// The firmware needs to know this because the connection sequence is different
// depending on the DP type.
//
// Parameters:
//    1. DP_Type - Debug Port type. 0=SW, 1=SWJ
//
// Returns:
//    1. HOST_COMMAND_OK
//
STATUS SWD_Configure(U8 dp_type)
{
    swj_dp_type = dp_type;

    return HOST_COMMAND_OK;
}

//-----------------------------------------------------------------------------
// (0x21) SWD_Connect
//-----------------------------------------------------------------------------
//
// Sets the target device for Serial Wire communication and returns the
// 32-bit ID code. Must be called before performing any SWD commands.
//
// Returns:
//  1-4. IDCODE - Value read from the IDCODE register (32-bit).
//    5. Response code.
//
STATUS SWD_Connect(void)
{
    U8 rtn;

    // Initialize IO pins for SWD interface
    _SetSWPinsIdle;

    // Select the Serial Wire Debug Port
    // Skip this switch sequence if the device does not have the swj_dp port
    // Serial Wire + JTAG
    SW_ShiftReset();
    SW_ShiftByteOut(0x9E);
    SW_ShiftByteOut(0xE7);

    // Reset the line and return the 32-bit ID code
    rtn = SWD_LineReset();
    //SendLongToHost(io_word.U32);

    return rtn;
}

//-----------------------------------------------------------------------------
// (0x30) SWD_Disconnect
//-----------------------------------------------------------------------------
//
// Switches the debug interface to JTAG communication and disconnects pins.
//
// Returns:
//    1. HOST_COMMAND_OK
//
STATUS SWD_Disconnect(void)
{
    // Initialize IO pins for SWD interface
    _SetSWPinsIdle;

    // Select the JTAG Debug Port
    // Skip this switch sequence if the device does not have the swj_dp port
    // Serial Wire + JTAG
    SW_ShiftReset();
    SW_ShiftByteOut(0x3C);
    SW_ShiftByteOut(0xE7);

    // Release debug interface pins except nSRST
    _ResetDebugPins;

    return HOST_COMMAND_OK;
}

//-----------------------------------------------------------------------------
// (0x31) SWD_LineReset
//-----------------------------------------------------------------------------
//
// Performs a line reset on the Serial Wire interface.
//
// Returns:
//    1. Response code.
//
STATUS SWD_LineReset(void)
{
    U8 ack;

    // Complete SWD reset sequence (50 cycles high followed by 2 or more idle cycles)
    SW_ShiftReset();
    SW_ShiftByteOut(0);

    // Now read the DPIDR register to move the SWD out of reset
    ack = SW_ShiftPacket(SW_IDCODE_RD, 1);
    SW_ShiftByteOut(0);

    return SW_Response(ack);
}

//-----------------------------------------------------------------------------
// (0x32) SWD_ClearErrors
//-----------------------------------------------------------------------------
//
// Clears all the error/sticky bits in the DP Control/Status register.
//
// Returns:
//  1-4. DP_CSR - DP_CTRLSTAT register value before the clear (32-bit, LE).
//  5-8. DP_CSR - DP_CTRLSTAT register value after the clear (32-bit, LE).
//    9. Response code.
//
STATUS SWD_ClearErrors(void)
{
    U8 ack;

    // First read the DP-CSR register and send the value to the host.
    SW_ShiftPacket(SW_CTRLSTAT_RD, 1);
    //SendLongToHost(io_word.U32);

    // Clear all error/sticky bits by writing to the abort register.
    io_word.U32 = 0x1E;
    SW_ShiftPacket(SW_ABORT_WR, 1);

    // Read the DP-CSR register again and send the results to the host.
    ack = SW_ShiftPacket(SW_CTRLSTAT_RD, 1);
    SW_ShiftByteOut(0);
    //SendLongToHost(io_word.U32);

    return SW_Response(ack);
}

//-----------------------------------------------------------------------------
// (0x34) SWD_DAP_Move
//-----------------------------------------------------------------------------
//
// Reads or writes one Debug/Access Port address one or more times.
//
// Parameters:
//    1. Count - Number of words to transfer minus one (i.e. 0=move 1 word).
//    2. DAP_Addr - The DAP register address to transfer one or more times.
//  3-n. Words[] - Array of 32-bit LE words (write transfers only).
//
// Returns:
//  1-n. Words[] - Array of 32-bit LE words (read transfers only).
//  n+1. Response code.
//
// Uses:
//    ack_error - Resets error accumulator.
//
STATUS SWD_DAP_Move(U8 cnt, U8 dap, U32 * transfer_data)
{
    // Reset global error accumulator
    ack_error = SW_ACK_OK;

    // Determine if this is a read or write transfer
    if (dap & DAP_CMD_RnW)
    {
        // Perform the requested number of reads
        SW_DAP_Read(cnt, dap, transfer_data);
    }
    else
    {
        SW_DAP_Write(cnt, dap, transfer_data, TRUE);
    }

    // Finish with idle cycles
    SW_ShiftByteOut(0);

    // Return the accumulated error result
    return SW_Response(ack_error);
}

//-----------------------------------------------------------------------------
// SWD Helper Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SW_Response
//-----------------------------------------------------------------------------
//
// Converts SWD acknowledge value into a host response code.
//
// Parameters:
//    SW_Ack - 3-bit SWD acknowledge code.
//
// Returns:
//    8-bit host response code.
//
STATUS SW_Response(U8 SW_Ack)
{
    switch (SW_Ack)
    {
    case SW_ACK_OK:     return HOST_COMMAND_OK;
    case SW_ACK_WAIT:   return HOST_AP_TIMEOUT;
    case SW_ACK_FAULT:  return HOST_ACK_FAULT;
    default:            return HOST_WIRE_ERROR;
    }
}

//-----------------------------------------------------------------------------
// SW_DAP_Read
//-----------------------------------------------------------------------------
//
// Does one or more reads of one Debug/Access Port register and returns each
// 32-bit word to the host.
//
// Parameters:
//    cnt - Number of words to read minus one (i.e. 0=read 1 word).
//    DAP_Addr - The DAP register address to read one or more times.
//
void SW_DAP_Read(U8 cnt, U8 DAP_Addr, U32 * read_data)
{
    U8 req;

    // Format the packet request header
    req = SW_Request(DAP_Addr);

    // Shift the first packet and if DP access, send the results
    SW_ShiftPacket(req, 0);
    if (!(req & SW_REQ_APnDP))
    {
        *read_data = io_word.U32;
        read_data++;
    }

    // Perform the requested number of reads
    for (; cnt != 0; cnt--)
    {
        SW_ShiftPacket(req, 0);
        *read_data = io_word.U32;
        read_data++;
    }

    // For AP access, get and send results of the last read
    if (req & SW_REQ_APnDP)
    {
        SW_ShiftPacket(SW_RDBUFF_RD, 0);
        *read_data = io_word.U32;
        read_data++;
    }
}

//-----------------------------------------------------------------------------
// SW_DAP_Write
//-----------------------------------------------------------------------------
//
// Does one or more writes to one Debug/Access Port register getting each
// 32-bit word from the host.
//
// Parameters:
//    cnt - Number of words to write minus one (ie 0=write 1 word).
//    DAP_Addr - The DAP register address to write one or more times.
//    final - True if this is the final transfer of a move sequence.
//
void SW_DAP_Write(U8 cnt, U8 DAP_Addr, U32 * write_data, BOOL final)
{
    U8 req;

    // Format the packet request header
    req = SW_Request(DAP_Addr);

    // Perform the requested number of writes
    do
    {
        io_word.U32 = *write_data;
        write_data++;
#if 0
    	// Clear the upper half word for 16-bit packed writes
   		io_word.U16[MSB] = 0;

        // For packed transfers, write 16-bits at a time
        if (DAP_Addr & DAP_CMD_PACKED)
        {
            SW_ShiftPacket(req, 0);
            io_word.U32 = *write_data;
            write_data++;

    		// Clear the upper half word for 16-bit packed writes
   			io_word.U16[MSB] = 0;
        }
        else
        {
            io_word.U16[MSB] = (U16) *write_data;
            write_data++;
        }
#endif
        SW_ShiftPacket(req, 0);
    }
    while (cnt-- != 0);

    // For AP access, check results of last write (use default retry count
    // because previous write may need time to complete)
    if (final && (req & SW_REQ_APnDP))
    {
        SW_ShiftPacket(SW_RDBUFF_RD, 0);
    }
}

//-----------------------------------------------------------------------------
// SW_Request
//-----------------------------------------------------------------------------
//
// Converts DAP address into SWD packet request value.
//
// Parameters:
//    DAP_Addr - 4-bit DAP address (A3:A2:RnW:APnDP).
//
// Returns:
//    Complete 8-bit packet request value. Includes parity, start, etc.
//
U8 SW_Request(U8 DAP_Addr)
{
    U8 req;

    // Convert the DAP address into a SWD packet request value
    req = DAP_Addr & DAP_CMD_MASK;      // mask off the bank select bits
    req = req | even_parity[req];       // compute and add parity bit
    req = req << 1;                     // move address/parity bits
    req = req | SW_REQ_PARK_START;      // add start and park bits
    return req;
}

//-----------------------------------------------------------------------------
// SW_CalcDataParity
//-----------------------------------------------------------------------------
//
// Calculates even parity over the 32-bit value in io_word.U32. Contents of
// io_word are not changed.
//
// Returns:
//    1-bit even parity.
//
// Uses:
//    io_word - Holds 32-bit value to compute parity on.
//
bit SW_CalcDataParity(void)
{
    U8 parity;

    // Calculate column parity, reducing down to 4 columns
    parity  = io_word.U8[b0];
    parity ^= io_word.U8[b1];
    parity ^= io_word.U8[b2];
    parity ^= io_word.U8[b3];
    parity ^= parity >> 4;

    // Use lookup table to get parity on 4 remaining bits. The cast (bit)
    // converts any non-zero value to 1.
    return (bit)even_parity[parity & 0xF];
}

//-----------------------------------------------------------------------------
// SW_ShiftReset
//-----------------------------------------------------------------------------
//
// Puts the SWD into the reset state by clocking 64 times with SWDIO high.
// Leaves SWDIO an output and high.
//
void SW_ShiftReset(void)
{
    U8 i;

    // Drive SWDIO high
    SWDIO_Out = 1;
    _SetSWDIOasOutput;

    // Complete 64 SWCLK cycles
    for (i = 64; i != 0; i--)
    {
        _StrobeSWCLK;
    }
}

//-----------------------------------------------------------------------------
// SW_ShiftPacket
//-----------------------------------------------------------------------------
//
// Completes one serial wire packet transfer (read or write). Expects SWDIO to
// be an output on entry.
//
// Parameters:
//    request - Complete 8-bit packet request value. Includes parity, start, etc.
//    retry - Number of times to try the request while the target ack is WAIT.
//       0 = use the system default retry count
//       n = try the request upto n times
//
// Returns:
//    3-bit SWD acknowledge code.
//    Leaves SWDIO an output and low.
//
// Uses:
//    ack_error - Updated if there was a transfer error.
//    io_byte - Used for all transfers.
//    io_word - On entry, holds the 32-bit word data to transfer on writes.
//              On exit, holds the 32-bit word data transfered on reads.
//
U8 SW_ShiftPacket(U8 request, U8 retry)
{
    U8 ack, limit, i;

    // If retry parameter is zero, use the default value instead
    if (retry == 0)
    {
        retry = DAP_RETRY_COUNT;
    }
    limit = retry;

    // While waiting, do request phase (8-bit request, turnaround, 3-bit ack)
    do
    {
        // Turnaround or idle cycle, makes or keeps SWDIO an output
        SWDIO_Out = 0; _SetSWDIOasOutput; _StrobeSWCLK;

        // Shift out the 8-bit packet request
        SW_ShiftByteOut(request);

        // Turnaround cycle makes SWDIO an input
        _SetSWDIOasInput; _StrobeSWCLK;

        // Shift in the 3-bit acknowledge response
        io_byte = 0;
        iob_0 = SWDIO_In;  _StrobeSWCLK;
        iob_1 = SWDIO_In;  _StrobeSWCLK;
        iob_2 = SWDIO_In;  _StrobeSWCLK;
        ack = io_byte;

        // Check if we need to retry the request
        if ((ack == SW_ACK_WAIT) && --retry)
        {
            // Delay an increasing amount with each retry
            for (i=retry; i < limit; i++);
        }
        else
        {
            break;  // Request phase complete (or timeout)
        }
    }
    while (TRUE);

    // If the request was accepted, do the data transfer phase (turnaround if
    // writing, 32-bit data, and parity)
    if (ack == SW_ACK_OK)
    {
        if (request & SW_REQ_RnW)
        {
            // Swap endian order while shifting in 32-bits of data
            io_word.U8[b0] = SW_ShiftByteIn();
            io_word.U8[b1] = SW_ShiftByteIn();
            io_word.U8[b2] = SW_ShiftByteIn();
            io_word.U8[b3] = SW_ShiftByteIn();

            // Shift in the parity bit
            iob_0 = SWDIO_In; _StrobeSWCLK;

            // Check for parity error
            if (iob_0 ^ SW_CalcDataParity())
            {
                ack = SW_ACK_PARITY_ERR;
            }
        }
        else
        {
            // Turnaround cycle makes SWDIO an output
            _SetSWDIOasOutput; _StrobeSWCLK;

            // Swap endian order while shifting out 32-bits of data
            SW_ShiftByteOut(io_word.U8[b0]);
            SW_ShiftByteOut(io_word.U8[b1]);
            SW_ShiftByteOut(io_word.U8[b2]);
            SW_ShiftByteOut(io_word.U8[b3]);

            // Shift out the parity bit
            SWDIO_Out = SW_CalcDataParity(); _StrobeSWCLK;
        }
    }
    // TODO: Add error (FAULT, line, parity) handling here?  RESEND on parity error?

    // Turnaround or idle cycle, always leave SWDIO an output
    SWDIO_Out = 0; _SetSWDIOasOutput; _StrobeSWCLK;

    // Update the global error accumulator if there was an error
    if (ack != SW_ACK_OK)
    {
        ack_error = ack;
    }
    return ack;
}

//-----------------------------------------------------------------------------
// SW_ShiftByteOut
//-----------------------------------------------------------------------------
//
// Shifts an 8-bit byte out the SWDIO pin.
//
// Parameters:
//    byte - The 8-bit byte to shift out on SWDIO.
//
// Uses:
//    io_byte - Holds byte as it is shifted out.
//
#pragma OT(8, SPEED)
void SW_ShiftByteOut(U8 byte)
{
    // Make sure SWDIO is an output
    _SetSWDIOasOutput;

    // Copy data to bit addressable location
    io_byte = byte;

    // Shift 8-bits out on SWDIO
    SWDIO_Out = iob_0; _StrobeSWCLK;
    SWDIO_Out = iob_1; _StrobeSWCLK;
    SWDIO_Out = iob_2; _StrobeSWCLK;
    SWDIO_Out = iob_3; _StrobeSWCLK;
    SWDIO_Out = iob_4; _StrobeSWCLK;
    SWDIO_Out = iob_5; _StrobeSWCLK;
    SWDIO_Out = iob_6; _StrobeSWCLK;
    SWDIO_Out = iob_7; _StrobeSWCLK;
}

//-----------------------------------------------------------------------------
// SW_ShiftByteIn
//-----------------------------------------------------------------------------
//
// Shifts an 8-bit byte in from the SWDIO pin.
//
// Returns:
//    8-bit byte value shifted in on SWDIO.
//
// Uses:
//    io_byte - Holds byte as it is shifted in.
//
#pragma OT(8, SPEED)
U8 SW_ShiftByteIn(void)
{
    // Make sure SWDIO is an input
    _SetSWDIOasInput;

    // Shift 8-bits in on SWDIO
    iob_0 = SWDIO_In; _StrobeSWCLK;
    iob_1 = SWDIO_In; _StrobeSWCLK;
    iob_2 = SWDIO_In; _StrobeSWCLK;
    iob_3 = SWDIO_In; _StrobeSWCLK;
    iob_4 = SWDIO_In; _StrobeSWCLK;
    iob_5 = SWDIO_In; _StrobeSWCLK;
    iob_6 = SWDIO_In; _StrobeSWCLK;
    iob_7 = SWDIO_In; _StrobeSWCLK;

    // Return the byte that was shifted in
    return io_byte;
}

