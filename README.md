Programming internal SRAM over SWD
===========================
## 1. Introduction
This documentation describe how to programming ARM Cortex M3 internal SRAM over SWD(Serial Wire Debug) interface. For this purpose, something we too know.

* SWD communication protocol.
We need to know Low level timing requirement, that is  foundation to exchange command and data between debugger and target MCU.
* Read/write data from/to internal SRAM.
We need to know how to program firmware into desired address, like SRAM(0x20000000). That needs us to know SW-DAP registers usage. Pass address and data over those registers and then into internal SRAM.
* Make code running from SRAM.
We need to change vector table entry from internal flash to SRAM; And SP and PC also needed change to SRAM location. That need us to know the cortex M3 debug and system registers usage.

We choose Silabs SiM3U167 as target MCU in this implementation. We implement a high level protocol with python script, it calls Silabs provided DLL file interface to access Silabs USB Debug Adapter. Also, we provide a firmware running from C8051F380, it contains a full implementation on both low level communicating timing and high level programming SRAM protocol.

## 2. Serial Wire Debug overview
Serial Wire Debug (SWD) is a 2-pin (SWDIO/SWCLK) electrical alternative JTAG interface that has the same JTAG protocol on top. SWD uses an ARM CPU standard bi-directional wire protocol, defined in the ARM Debug Interface v5. This enables the debugger to become another AMBA bus master for access to system memory and peripheral or debug registers.

The Debug Access Port(DAP) is split into two main control units.  the Debug Port (DP) and the Access Port (AP), and the physical connection to the debugger is part of the DP. The DAP supports two types of access, Debug Port (DP) accesses and Access Port (AP)accesses. External device to communicate directly with Serial Wire Debug Port(SW-DP) over SWDIO/SCLK pins. And SW-DP in turn can access one or several Access Ports(APs) the give access to the rest of the system. The MEM-AP is important AP which provide a way to access all memory and peripheral registers residing on the internal AHB/APB buses.

__Figure 1 ARM Debug Interface MEM-AP Implementation__

![ADI MEM-AP][ADI_MEM-AP]

One of the four registers within the DP is the AP Select Register, SELECT. This register specifies a particular Access Port, and a bank of four 32-bit words within the register map of that AP. It enables up to 256 Access Ports to be implemented, and gives access to any one of 16 four-word banks of registers on the selected AP.


## 3. SWD protocol
This section gives an overview of the bi-directional operation of the protocol. It illustrates each of the possible sequences of operations on the Serial Wire Debug interface data connection.

### 3.1. Connection and line reset sequence
The SW-DP must use connection sequence of 50 clock cycles with data = 1. And this sequence is also used as a line reset sequence which requires 50 consecutive 1s on the data input.

__JTAG to SWD switching__

SWJ-DP enables either an SWD or JTAG protocol to be used on the debug port. To do
this, it implements a watcher circuit that detects a specific 16-bit selection sequence on the SWDIOTMSpin:

The 16-bit JTAG-to-SWD select sequence is defined to be 0b0111100111100111, MSB first. This can be represented as 16'h79E7 if transmitted MSB first or 16'hE79E if transmitted LSB first.

The host must read IDCODE register after line request sequence. This requirement gives confirmation that correct packet frame alignment has been achieved.

### 3.2. Successful transaction operation

A successful transaction contains three phases.

- Host sending an eight-bit write packet request to target.
- Target response with a three-bit OK acknowledge to host.
- A 33-bit data phase, either from host or target depends on write/read operation.

__Figure 2 Serial Wire Debug successful write operation__

![SWD write operation][SWD-WRITE]


### 3.2.1. Packet request phase
The request phase consists of 8 bits. The meaning of each bit in the request is illustrated below.

* `Start` -- A single start bit, with value 1.
* `APnDP` -- A single bit, indicating whether the Debug Port or the Access Port Access Register is to be accessed. 1 for accessing AP.
* `RnW` -- A single bit, this bit is 0 for an write access, or 1 for a read access.
* `A[2:3]` -- Two bits, giving the A[3:2] address field for the DP or AP register address.
* `Parity` -- A single odd parity bit is made over the APnDP, RnW and A[2:3] bits. The number of bits set to 1 is odd, then the parity bit is set to 1.
* `Stop` -- A single stop bit. In the synchronousSWD protocol this is always 0.
* `Park` -- A single bit, with value 1.

### 3.2.2.  Acknowledge response phase
ACK phase is a three-bit(LSB-first) target to host response. Three type ACK response.

* `OK Response` -- It indicates successful operation, value is b001.
* `WAIT response` -- The host must retry the operation later, value is b010.
* `FAULT response` --  If the target responds with FAULT, an error has occurred
and one of the sticky bits in CTRL/STAT is set. The host can check the sticky error bits to see what kind of error has occurred. It must clear the sticky bits in  ABORT register before using any AP commands, because the target will always respond with FAULT as long as one of the sticky error bits are set. Value is b100.

### 3.2.3. Data transfer phase
It contains 32 data bits and 1 parity bit. The The parity check is made over the 32 data bits. The number of bits set to 1 is odd, then the parity bit is set to 1.

### 3.2.4. Turnaround period
From figure above, there is `Trn` -- Turnaround period between each phase. Every  time  the  SWDIO  changes  data  direction,  a  one-cycle  turnaround  period  is  inserted  which both sides should ignore. This means there is always a turnaround period between the request and acknowledge. On a write request, there is a turnaround period between acknowledge and the data phase. On a read request there is a a turnaround after the data phase.


## 4. Serial Write Debug Port(SW-DP) Registers
This section will give a brief overview over the SW-DP registers.

__Table 1 SW-DP registers__

| Address | Read      | Write     |
| ------- | ------    | -----     |
|   0x00  | IDCODE    | ABORT     |
|   0x04  | CTRL/STAT | CTRL/STAT |
|   0x08  | RESEND    | SELECT    |
|   0x0C  | RDBUFF    | N/A       |

### 4.1. The Identification Code Register, IDCODE
`IDCODE` register provides a identification information about SW-DP. On the EFM32 or SiM3U devices with a Cortex-M3 or Cortex-M4 core this register should read 0x2BA01477. For devices with a CortexM0+ core the register should read 0x0BC11477.

### 4.2. The AP Abort Register, ABORT
`ABORT` register main purpose is to force a DAP abort, and on a SW-DP it is also used to clear error and sticky flag conditions. Here are the bit assignments of AP Abort register.

__Table 2 ABORT register bit assignments__

| Bits   | Function | Description |
| -----  | -------- | ----------- |
| [31:5] |    -     | Reserved    |
| [4] | ORUNERRCLR | Write 1 to this bit to clear the STICKYORUN overrun error flag to 0. |
| [3] | WDERRCLR   | Write 1 to this bit to clear the WDATAERR write data error flag to 0|
| [2] | STKERRCLR | Write 1 to this bit to clear the STICKYERR sticky error flag to 0.|
| [1] | STKCMPCLRa | Write 1 to this bit to clear the STICKYCMP sticky compare flag to 0.|
| [0] | DAPABORT |Write 1 to this bit to generate a DAP abort. This aborts the current AP transaction. Do this only if the debugger has received WAIT responses over an extended period.|

You might want to find what caused the flag to be set to 1. Typically:

* For the `STICKYCMP` or `STICKYERR` flag, you must find which location was accessed to cause the flag to be set to 1.
* For the `WDATAERR` flag, you must resend the corrupted data.
* For the `STICKYORUN` flag, you must find which DP or AP transaction caused the overflow. You then have to repeat your transactions from that point.

### 4.3. The Control/Status Register, CTRL/STAT
`CTRL/STAT` register provides control of the DP and status information about the DP.

__Figure 3 Control/Status Register bit assignments__

![CTRL/STAT][CTRL-STAT]

* Bit [28] `CDBGPWRUPREQ` is the signal from the debug interface to the power controller, used to request the system power controller to fully power-up and enable clocks in the debug power
domain.
* Bit [30] `CSYSPWRUPREQ` is the signal from the debug interface to the power controller, used to request the system power controller to fully power-up and enable clocks in the system power domain.

The debugger must write a 1 to the CDBGPWRUPREQ and CSYSPWRUPREQ bits before using the AHB-AP. The STICKYERR bit is set if an error is returned by a AP transaction. While the STICKYERR bit is set any SWD request will return a FAULT response. To clear the STICKYERR bit, use the ABORT register.

### 4.4. The AP Select Register, SELECT
`SELECT` register specifies a particular Access Port, and a bank of four 32-bit words within the register map of that AP. It enables up to 256 Access Ports to be implemented, and gives access to any one of 16 four-word banks of registers on the
selected AP.

__Table 3 Bit assignments for the AP Select Register, SELECT__

| Bits | Function | Description |
| ---- | ---------| ----------- |
| [31:24] | APSEL | Selects the current AP. |
| [7:4] | APBANKSEL | Selects the active four-word register bank on the current AP |

### 4.5. The Read Buffer, RDBUFF
`RDBUFF` register return the result of a previous read, without initiating a new AP transaction.

If you require the value from an AP register read, that A read of the DP Read Buffer must be followed.


## 5. Memory Access Port(MEM-AP) Registers
A MEM-AP provides a DAP with access to a memory subsystem. Since memory, peripherals and debug components are all memory mapped, the MEM-AP can be used to both program and debug Cortex M3.

__Table 4 MEM-AP registers__

| Address | Bank | Function | Description |
| ------- | ---- | -------- | ----------- |
|  0x00   | 0x00 | CSW | Control/Status Word Register |
|  0x04   | 0x00 | TAR | Transfer Address Register |
|  0x0C   | 0x00 | DRW | Data Read/Write Register |
|  0xFC   | 0x0F | IDR | Identification Register |

### 5.1. Control/Status Word (CSW) Register
`CSW` register configures and controls accesses through the MEM-AP to or from a connect memory system.

__Table 5 Bit assignments for the MEM-AP Control/Status Word Register, CSW__

| Bits | Function | Description |
| ---- | -------- | ----------- |
| [30:24]| Prot | Bus access protection control. This field enables the debugger to specify protection flags for a debug access. |
| [5:4] | AddrInc | Address auto-increment and packing mode. |
| [2:0] | Size | b000: 8 bits; b001: 16 bits; b010: 32 bits |

### 5.2. The Transfer Address Register (TAR)
`TAR` register holds the memory address  to be accessed.
The contents of TAR can be incremented automatically on a successful DRW access with b01 set in `AddrInc` field of `CSW` register.

### 5.3. The Data Read/Write Register (DRW)
`DRW` register is used to either write or read from the address held in TAR. To write a value to an internal memory address, first write the address to the TAR register,
then write the value to DRW. To read a memory address, first write the address to TAR, then read the value in DRW.

### 5.4. The Identification Register, (IDR)
`IDR` register identifies the Access Port. To read this register, the APBANKSEL field should be set to 0xF. And then IDR register can be read with address 0x0C(ADDR[3:2] = b11). The IDR register should return the value 0x24770011 on devices with a Cortex-M3 or Cortex-M4 core. On devices with a Cortex-M0+ it should return 0x0477003

![IDR REG][IDR-REG]

## 6. Debug and system Registers
This section will go through the Cortex M3 Debug registers. With debug registers, we can set core into halt mode and can change core registers, like SP or PC.

__Table 6 Debug and system registers__

| Address | Name | Description |
| ------- | ---- | ----------- |
| 0xE000EDF0 | DHCSR | Debug Halting Control and Status Register |
| 0xE000EDF4 | DCRSR | Debug Core Register Selector Register |
| 0xE000EDF8 | DCRDR | Debug Core Register Data Register |
| 0xE000EDFC | DEMCR | Debug Exception and Monitor Control Register |
| 0xE000ED0C | AIRCR | Application Interrupt and Reset Control Register |


### 6.1. Debug Halting Control and Status Register, DHCSR
`DHCSR` register controls halting debug. Halting debug is enabled when C_DEBUGEN is set to 1.


__Table 7 DHCSR bit assignments__

| Bits | Name | Function |
| ------- | ---- | ----------- |
| [31:16] | DBGKEY | Debug key: A debugger must write 0xA05Fto this field to enable write accesses to bits [15:0], otherwise the processor ignores the write access. |
| [16] | S_REGRDY | A handshake flag for transfers through the DCRDR: 0 = There has been a write to the DCRDR, but the transfer is not complete; 1 = The transfer to or from the DCRDR is complete. |
| [0] | C_DEBUGEN | Halting debug enable bit: 1 = Enabled |

### 6.2. Debug Core Register Selector Register, DCRSR
`DCRSR` register provides debug access to the ARM core registers, special-purpose registers, and Floating-point extension registers. A write to DCRSR specifies the resister to transfer.

__Table 8 DCRSR bit assignments__

| Bits | Name | Function |
| ------- | ---- | ----------- |
| [16] | REGWnR | 0 = read; 1= write |
| [6:0] | REGSEL | Specifies the ARM core register, special-purpose register, or Floating-point extension register, to transfer: R0-R12, SP, LR, DebugReturnAddr, xPSR, MSP, PSP, etc. |

DebugReturnAddress is the address of the first instruction to be executed on exit from Debug state.

### 6.3. Debug Core Register Data Register, DCRDR
`DCRDR` register provides debug access to ARM core register, special-purpose registers,and Floating-point extension registers. The DCRDR is the data register for these accesses.

__Use of DCRSR and DCRDR__

- To transfer a data word to an ARM core register, special-purpose register, orFloating-point extension register, a debugger:
    - Writes the required word to DCRDR.
    - Writes to the DCRSR, with the REGSEL value indicating the required register, and the REGWnR bit as 1 to indicate a write access. This write clears the DHCSR S_REGRDY bit to 0.
    - If required, polls DHCSR until DHCSR.S_REGRDY reads-as-one. This shows that the processor has transferred the DCRDR value to the selected register.
- To transfer a data word from an ARM core register, special-purpose register, or Floating-point extension register, a debugger:
    - Writes to the DCRSR, with the REGSEL value indicating the required register, and the REGWnR bit as 0 to indicate a read access. This write clears the DHCSR.S_REGRDY bit to 0.
    - Polls DHCSR until DHCSR.S_REGRDY reads-as-one. This shows that the processor has transferred the value of the selected register to DCRDR.
    - Reads the required value from DCRDR.

### 6.4. Debug Exception and Monitor Control Register, DEMCR
`DEMCR` register manages vector catch behavior and DebugMonitor handling when debugging. Bits [23:16] provide DebugMonitor exception control. Bits [15:0] provide Debug state, halting debug, control.

__Table 9 DEMCR bit assignments__

| Bits | Name | Function |
| ------- | ---- | ----------- |
| [10] | VC_HARDERR | Enable halting debug trap on a HardFault exception |
| [9] | VC_INTERR | Enable halting debug trap on a fault occurring during exception entry or exception return. |
| [8] | VC_BUSERR | Enable halting debug trap on a BusFault exception. |
| [7] | VC_STATERR | Enable halting debug trap on a UsageFault exception caused by a state information error, for example an Undefined Instruction exception. |
| [6] | VC_CHKERR | Enable halting debug trap on a UsageFault exception caused by a checking error, for example an alignment check error. |
| [5] | VC_NOCPERR | Enable halting debug trap on a UsageFault caused by an access to a Coprocessor. |
| [4] | VC_MMERR | Enable halting debug trap on a MemManage exception. |
| [0] | VC_CORERESET | Enable Reset Vector Catch. This causes a Local reset to halt a running system. |

Vector catch is the mechanism for generating a debug event and entering Debug state when a particular exception occurs. Vector catching is only supported by halting debug.

### 6.5. Application Interrupt and Reset Control Register, AIRCR
`AIRCR` register sets or returns interrupt control data.

__Table 10 AIRCR bit assignments__

| Bits | Name | Function |
| ------- | ---- | ----------- |
| [31:16] | VECTKEY | Vector Key. Register writes must write 0x05FAto this field, otherwise the write is ignored.On reads, returns 0xFA05.|
| [15] | ENDIANNESS | 0 = Little endian; 1 = Big endian |
| [10:8] | PRIGROUP | Priority grouping |
| [2] | SYSRESETREQ | Writing 1 to this bit asserts a signal to the external system to request a Local reset. |
| [1] | VECTCLRACTIVE | Writing 1 to this bit clears all active state information for fixed and configurable exceptions. |
| [0] | VECTRESET | Writing 1 to this bit causes a local system reset. |

## 7. SRAM programing
   This section demonstrate step by step how to programming internal SRAM over SWD.

### 7.1. Initialization
Before suing the SW-DP an initialization sequence must be performed to establish communication and bring the SW-DP to a know state.

- Send more than 50 SWCLKTCK cycles with SWDIOTMS=1. This ensures that both SWD and JTAG are in their reset states.
- Send the 16-bit JTAG-to-SWD select sequence on SWDIOTMS
- Send more than 50 SWCLKTCK cycles with SWDIOTMS=1. This ensures that if SWJ-DP was already in SWD mode, before sending the select sequence, the SWD goes to line reset.
- Perform a READID to validate that SWJ-DP has switched to SWD operation.

### 7.2. Halt the core
Before programming internal SRAM, the ARM Cortex M3 should first be reset and halted. It gets the core and peripherals into a know state and prohibit the core from accidentally running partial code while writing the program.

There debug and system registers and one Silabs extent AP register CHIPAP_CTRL1 are used for this purpose.
CHIPAP_CTRL1 address = 0x1, APSEL = 0x0A. bit 3 core_reset_ap, To hold the CortexM3 core in reset this bit should be written to one.

The process is as follow:
- Write 0x08 to CHIPAP_CTRL1.
- Write 0xA05F0001 to DHCSR, which halting debug enabled.
- Write 0x01 to DEMCR. This enable Reset Vector Catch.
- Write 0xFA050004 to AIRCR. This reset the core.
- Write 0x00 to CHIPAP_CTRL1.

Now the core will be halted on first instruction and all peripherals and registers will have their reset value.

### 7.3. Memory and core register access.

### 7.3.1 Access memory

- Set 32 bit width and auto increment in CSW register.
- Write memory address in TAR register
- Access DRW register for read/write data.

Here is the python script example

```python
def swd_write_mem(uda, address, data_ws, length):
    # Auto increment addresses
    uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
    uda.QueueWrite(MEMAP_CSW, 0x23000012)
    uda.QueueWrite(MEMAP_TAR, address)
    for x in range (0, length):
        uda.QueueWrite(MEMAP_DRW, data_ws[x])
        uda.StartTransfers()

def swd_read_mem(uda, address, length):
    data_ws = []
    # Auto increment addresses
    uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
    uda.QueueWrite(MEMAP_CSW, 0x23000012)
    uda.QueueWrite(MEMAP_TAR, address)
    for x in range (0, length):
        uda.QueueRead(MEMAP_DRW)
        data_ws.append(uda.StartTransfers()[0])
    return data_ws
```

### 7.3.2 Access core registers

__Read:__

- Set 32 bit width in CSW register
- Write DCRSR address into TAR register.
- Write core register index Rn into DRW register.
- Write DCRDR address into TAR register.
- Read core register value from DRW register.

__Write:__

- Set 32 bit width in CSW register
- Write DCRDR address into TAR register.
- Write core value into DRW register.
- Write DCRSR address into TAR register.
- Write core register index Rn and REGWnR = 1 into DRW register.

Here is the python script example

```Python
def swd_write_core_register(uda, n, val):
    uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
    uda.QueueWrite(MEMAP_CSW, 0x23000002)
    uda.QueueWrite(MEMAP_TAR, DCRDR)
    uda.QueueWrite(MEMAP_DRW, val)
    uda.StartTransfers()
    uda.QueueWrite(MEMAP_TAR, DCRSR)
    uda.QueueWrite(MEMAP_DRW, n | (1 << 16))
    uda.StartTransfers()

def swd_read_core_register(uda, n):
    uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
    uda.QueueWrite(MEMAP_CSW, 0x23000002)
    uda.QueueWrite(MEMAP_TAR, DCRSR)
    uda.QueueWrite(MEMAP_DRW, n)
    uda.StartTransfers()
    uda.QueueWrite(MEMAP_TAR, DCRDR)
    uda.QueueRead(MEMAP_DRW)
    val = uda.StartTransfers()[0]
    return val
```

### 7.3.3 Programming firmware into internal SRAM
The process is followed

- Read firmware from disk.
- Write firmware into internal SRAM by swd_write_mem() function.
- Read back firmware by swd_read_mem() to verify the integrity.
- Update vector table entry in 0xe000ed08 to SRAM start position 0x20000000.
- Update R15(PC) with reset vector address. It locates at second word position in firmware.
- Update R13(SP) with stack address defined in first word in firmware.
- Exist debug mode by writing 0xA05F0000 into DHCSR register.

Here is the python script example

```Python
    f = open("sim3u1xx_USBHID_ram.bin",mode = 'rb')
    data = f.read()
    swd_write_mem(uda, 0x20000000, data, len)
    .
    .
    .
    swd_write_mem(uda, 0xe000ed08, 0x20000000, 1)
    swd_write_core_register(uda, 15, data[1]& 0xFFFFFFFE)
    swd_write_core_register(uda, 13, data[0])
    write_AHB(uda, DHCSR, 0xA05F0000)
```

Then we can see firmware runs from internal SRAM.

## 8. Source code
Source code can be found at https://github.com/MarkDing/swd_programing_sram

## 9. Reference

* `Adi5` ARM Debug Interface v5 Architecture Specification.
* `Adi51` ARM Debug Interface v5 Architecture Specification ADIv5.1 Supplement.
* `Cm3trm` Cortex-M3 Revision r2p1 Technical Reference Manual
* `ARMv7-M` ARMv7-M Architecture Reference Manual
* `CoreSight DAP-Lite` CoreSight DAP-Lite Technical Reference Manual

## LICENSE
The MIT License (MIT)

Copyright (c) 2014 Mark Ding(mark.ding@hotmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sub-license, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[ADI_MEM-AP]:https://raw.github.com/MarkDing/swd_programing_sram/master/images/adi_mem-ap.png "ARM Debug Interface MEM-AP Implementation"
[SWD-WRITE]: https://raw.github.com/MarkDing/swd_programing_sram/master/images/swd-write-portocol.png "Serial Wire Debug successful write operation"
[CTRL-STAT]: https://raw.github.com/MarkDing/swd_programing_sram/master/images/ctrl-stat-reg.png "Control/Status Register bit assignments"
[IDR-REG]: https://raw.github.com/MarkDing/swd_programing_sram/master/images/idr-reg.png "The AP Identification Register, IDR"
