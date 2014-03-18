//
// Copyright (c) 2013 SILICON LABORATORIES, INC.
//
// FILE NAME   : main.c
// TARGET MCU  : C8051F380
// DESCRIPTION : ARM Debug Interface for a 32-bit Programmer
//
// This program demonstrates the basic Serial Wire interface functionality.
// It does not include the procedures to write, erase, or read flash.
//
// NOTES:
// 1) Remove J15 from the 'F38x Target Board.
// 2) Connect SWDIO (P1.1), SWCLK (P1.3), and ground to the 10-pin CoreSight
//    connector of an SiM3U/C/L.
// 3) Run the code and observe that transfer_data contains the correct IDCODE
//    to validate the SW interface.
//
//

#include <compiler_defs.h>
#include <C8051F380_defs.h>
#include "32bit_prog_defs.h"
#include "Init.h"
#include "bin_array.h"
//-----------------------------------------------------------------------------
// Variables Declarations
//-----------------------------------------------------------------------------

// Debug Port (DP) type. Tracks the type of the current DP connection.
U8 DP_Type;

// Possible values for DP_Type.
enum { DP_TYPE_NONE, DP_TYPE_SWD };


// Cortex M3 Debug Registers (AHB addresses)
#define DDFSR   0xE000ED30      // Debug Fault StatusRegister
#define DHCSR   0xE000EDF0      // Debug Halting Control and Status Register
#define DCRSR   0xE000EDF4      // Debug Core Register Selector Register
#define DCRDR   0xE000EDF8      // Debug Core Register Data Register
#define DEMCR   0xE000EDFC      // Debug Exception and Monitor Control Register
#define AIRCR   0xE000ED0C      // The Application Interrupt and Reset Control Register

//  Cortex M3 Memory Access Port
#define MEMAP_BANK_0  0x00000000       // BANK 0 => CSW, TAR, Reserved, DRW
#define MEMAP_BANK_1  0x00000010       // BANK 1 => BD0, BD1, BD2, BD3

// SiM3 Chip Access Port (SiLabs specific Debug Access Port)
#define CHIPAP_BANK_0  0x0A000000      // BANK 0 => CTRL1, CTRL2, LOCK, CRC
#define CHIPAP_BANK_1  0x0A000010      // BANK 1 => INIT_STAT, DAP_IN, DAP_OUT, None
#define CHIPAP_BANK_F  0x0A0000F0      // BANK F => None, None, None, ID

// MEMAP register addresses
#define MEMAP_CSW  0x01
#define MEMAP_TAR  0x05
#define MEMAP_DRW_WR  0x0D
#define MEMAP_DRW_RD  0x0F

// CHIPAP register addresses
#define CHIPAP_CTRL1_WR     0x01
#define CHIPAP_CTRL2_WR     0x05
#define CHIPAP_ID_WR        0x0D
#define CHIPAP_ID_RD        0x0F

#ifdef SRAM_PROGRAMMING
void connect_and_halt_core()
{
    U32 rw_data;

    rw_data = CHIPAP_BANK_F;
    SWD_DAP_Move(0, DAP_SELECT_WR, &rw_data);
    SWD_DAP_Move(0, CHIPAP_ID_RD, &rw_data);
    if (rw_data != 0x2430002) {
        return;
    }

    // CTRL1.core_reset_ap = 1
    rw_data = CHIPAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &rw_data);
    rw_data = 0x08;
    SWD_DAP_Move(0, CHIPAP_CTRL1_WR, &rw_data);

    // Select MEM BANK 0
    rw_data = MEMAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &rw_data);

    // 32 bit memory access, auto increment
    rw_data = 0x23000002;
    SWD_DAP_Move(0, MEMAP_CSW, &rw_data);

    // DHCSR.C_DEBUGEN = 1
    rw_data = DHCSR;
    SWD_DAP_Move(0, MEMAP_TAR, &rw_data);
    rw_data = 0xA05F0001;
    SWD_DAP_Move(0, MEMAP_DRW_WR, &rw_data);

    // DEMCR.VC_CORERESET = 1
    rw_data = DEMCR;
    SWD_DAP_Move(0, MEMAP_TAR, &rw_data);
    rw_data = 0x1;
    SWD_DAP_Move(0, MEMAP_DRW_WR, &rw_data);

    // reset the core
    rw_data = AIRCR;
    SWD_DAP_Move(0, MEMAP_TAR, &rw_data);
    rw_data = 0xFA050004;
    SWD_DAP_Move(0, MEMAP_DRW_WR, &rw_data);

    // CTRL1.core_reset_ap = 0
    rw_data = CHIPAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &rw_data);
    rw_data = 0;
    SWD_DAP_Move(0, CHIPAP_CTRL1_WR, &rw_data);

    // Select MEM BANK 0
    rw_data = MEMAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &rw_data);
}

void write_sequential_words(U32 addr, U32 len, U32 *rw_data)
{
    U32 i, tmp;
    U32 *buf = rw_data;

    tmp = MEMAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &tmp);
    // 32 bit memory access, auto increment
    tmp = 0x23000012;
    SWD_DAP_Move(0, MEMAP_CSW, &tmp);

    SWD_DAP_Move(0, MEMAP_TAR, &addr);
    for (i = 0; i < len; i++) {
        SWD_DAP_Move(0, MEMAP_DRW_WR, buf++);
    }
}

void read_sequential_words(U32 addr, U32 len, U32 *rw_data)
{
    U32 i, tmp;
    U32 *buf = rw_data;

    tmp = MEMAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &tmp);
    // 32 bit memory access, auto increment
    tmp = 0x23000012;
    SWD_DAP_Move(0, MEMAP_CSW, &tmp);

    SWD_DAP_Move(0, MEMAP_TAR, &addr);
    for (i = 0; i < len; i++) {
        SWD_DAP_Move(0, MEMAP_DRW_RD, buf++);
    }
}


void swd_write_core_register(U32 n, U32 *rw_data)
{
    U32 tmp;
    tmp = MEMAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &tmp);
    tmp = 0x23000002;
    SWD_DAP_Move(0, MEMAP_CSW, &tmp);
    tmp = DCRDR;
    SWD_DAP_Move(0, MEMAP_TAR, &tmp);
    SWD_DAP_Move(0, MEMAP_DRW_WR, rw_data);

    tmp = DCRSR;
    SWD_DAP_Move(0, MEMAP_TAR, &tmp);
    n = n | (0x10000);
    SWD_DAP_Move(0, MEMAP_DRW_WR, &n);
}

void swd_read_core_register(U32 n, U32 *rw_data)
{
    U32 tmp;
    tmp = MEMAP_BANK_0;
    SWD_DAP_Move(0, DAP_SELECT_WR, &tmp);
    tmp = 0x23000002;
    SWD_DAP_Move(0, MEMAP_CSW, &tmp);
    tmp = DCRSR;
    SWD_DAP_Move(0, MEMAP_TAR, &tmp);
    SWD_DAP_Move(0, MEMAP_DRW_WR, &n);

    tmp = DCRDR;
    SWD_DAP_Move(0, MEMAP_TAR, &tmp);
    SWD_DAP_Move(0, MEMAP_DRW_RD, rw_data);
}

void programming_sram()
{
    U32 i, size, count, addr = 0x20000000;

    size = sizeof(binraw) / 4;


    for (i = 0; i < size; i += 1024) {
        if ((i + 1024) < size) {
            count = 1024;
        } else {
            count = size - i;
        }
        write_sequential_words(addr + i * 4, count, &binraw[i]);
    }

    write_sequential_words(0xe000ed08, 1, &addr);
    addr = binraw[1] & 0xFFFFFFFE;
    swd_write_core_register(15, &addr);
    addr = binraw[0];
    swd_write_core_register(13, &addr);
    addr = 0xA05F0000;
    write_sequential_words(DHCSR, 1, &addr);
}
#endif

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
void main(void)
{
    U32 transfer_data;

    WDT_Init();
    Oscillator_Init();
    Port_Init();

    // These pins are grounded on the CoreSight debug connector
    P1_4 = 0;
    P1_2 = 0;

    // There is no debug port connection at this point
    DP_Type = DP_TYPE_NONE;

    SWD_Initialize();
    SWD_Configure(DP_TYPE_SWD);
    SWD_Connect();

    transfer_data = 0x00000000;

    // Read the IDCODE from the connected device
    SWD_DAP_Move(0, DAP_IDCODE_RD, &transfer_data);

    // The return value from DAP_IDCODE_RD for SiM3U1xx devices is 0x2BA01477

    // Write the CTRLSTAT register to enable the debug hardware
    transfer_data = 0x50000000;
    SWD_DAP_Move(0, DAP_CTRLSTAT_WR, &transfer_data);
    SWD_ClearErrors();
    connect_and_halt_core();
    programming_sram();

    transfer_data = 0x00000000;
    SWD_DAP_Move(0, DAP_CTRLSTAT_WR, &transfer_data);
    SWD_Disconnect();

    while (1)
    {
    }
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DBG_Reset
//-----------------------------------------------------------------------------
//
// This function returns all debug pins to a neutral state:
//   1. Disconnects from the target.
//   2. Resets all I/O pins.
//
//
void DBG_Reset(void)
{
    SWD_Connect();

    // Reset all I/O ports
    Port_Init();

    // We are disconnecting, so release nSRST
    _ReleaseTargetReset;
}

