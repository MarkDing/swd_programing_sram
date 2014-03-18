
## Copyright (c) 2012-2013 by Silicon Laboratories.
## All rights reserved. This program and the accompanying materials
## are made available under the terms of the Silicon Laboratories End User
## License Agreement which accompanies this distribution, and is available at
## http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
## Original content and implementation provided by Silicon Laboratories.

"""
This example Python script uses the Silicon Labs USB Debug Adapter to erase,
program, or read SiM3 on-chip flash.  The goal is to demonstrate the required
debug port and device register sequences for each of these operations.
"""

import adi
import sys

#------------------------------------------------------------------------------
# DAP Constants
#------------------------------------------------------------------------------

# Cortex M3 Debug Registers (AHB addresses)
DDFSR = 0xE000ED30		# Debug Fault StatusRegister
DHCSR = 0xE000EDF0      # Debug Halting Control and Status Register
DCRSR = 0xE000EDF4      # Debug Core Register Selector Register
DCRDR = 0xE000EDF8      # Debug Core Register Data Register
DEMCR = 0xE000EDFC      # Debug Exception and Monitor Control Register
AIRCR = 0xE000ED0C      # The Application Interrupt and Reset Control Register
# DP register addresses
DP_IDCODE = 0x00
DP_CTRLSTAT = 0x04
DP_SELECT = 0x08

# Cortex M3 Memory Access Port
MEMAP_BANK_0 = 0x00000000       # BANK 0 => CSW, TAR, Reserved, DRW
MEMAP_BANK_1 = 0x00000010       # BANK 1 => BD0, BD1, BD2, BD3

# MEMAP register addresses
MEMAP_CSW = 0x01
MEMAP_TAR = 0x05
MEMAP_DRW = 0x0D

# CSW.ADDR_INC Values
VALUE_CSW_ADDR_INC_OFF = 0x00000000     # Auto-increment off
VALUE_CSW_ADDR_INC_SINGLE = 0x00000010  # Increment single
VALUE_CSW_ADDR_INC_PACKED = 0x00000020  # Increment packed

# CSW.SIZE Values
VALUE_CSW_SIZE_8_BITS = 0x00000000      # 8-bit memory size (optional)
VALUE_CSW_SIZE_16_BITS = 0x00000001     # 16-bit memory size (optional)
VALUE_CSW_SIZE_32_BITS = 0x00000002     # 32-bit memory size (required)

# SiM3 Chip Access Port (SiLabs specific Debug Access Port)
CHIPAP_BANK_0 = 0x0A000000      # BANK 0 => CTRL1, CTRL2, LOCK, CRC
CHIPAP_BANK_1 = 0x0A000010      # BANK 1 => INIT_STAT, DAP_IN, DAP_OUT, None
CHIPAP_BANK_F = 0x0A0000F0      # BANK F => None, None, None, ID

# CHIPAP register addresses
CHIPAP_CTRL1 = 0x01
CHIPAP_CTRL2 = 0x05
CHIPAP_ID = 0x0D

# SRAM and Flash address
SRAM_ADDR = 0x20000000
FLASH_ADDR = 0
#------------------------------------------------------------------------------
# FLASHCTRL Register Definitions
#------------------------------------------------------------------------------

# Note: For each register, there are 4 ws (16 bytes), including:
# Register         - Normal read/write access to the register
# Set Register     - Sets the bits in the register as masked by the write value
# Clear Register   - Clears the bits in the register as masked by the write value
# Mask Register    - ?
#
# The Set/Clear/Mask registers are not implemented for all registers

# Base address for the FLASHCTRL module
FLASHCTRL_BASE_ADDRESS = 0x4002E000

# Flash Register Address Offsets
# (Register Address = Peripheral Base Address + Offset)
OFF_FLASH_CONFIG = 0x00000000                   # Configure flash writes
OFF_FLASH_CONFIG_SET = 0x00000004               # Set register: sets bits to 1 (masked by value)
OFF_FLASH_CONFIG_CLR = 0x00000008               # Clear register: clears bits to 0 (masked by value)
OFF_FLASH_WRITE_ADDRESS = 0x000000A0            # Flash address
OFF_FLASH_WRITE_DATA = 0x000000B0               # Flash data
OFF_FLASH_WRITE_KEY = 0x000000C0                # Flash key

# FLASH.CONFIG Masks
MASK_FLASH_CONFIG_BUSY = 0x00100000             # Indicates that flash writes/erases are in progress
MASK_FLASH_CONFIG_DATA_BUSY = 0x00080000        # Indicates that the write data buffer is full
MASK_FLASH_CONFIG_ERASE_ENABLE = 0x00040000     # 0 - Enable flash writes
												# 1 - Enable flash page erases
MASK_FLASH_CONFIG_SEQUENTIAL = 0x00010000       # 0 - Independent flash writes
												# 1 - Sequential double-buffered flash writes

# FLASH.KEY Masks
MASK_FLASH_KEY_KEY = 0x000000FF                 # Write 0xA5, 0xF1 to unlock the next flash write/erase
												# Write 0xA5, 0xF2 to unlock all flash writes/erases
												# Write 0x5A to lock all flash writes/erases

#------------------------------------------------------------------------------
# CLKCTRL Register Definitions
#------------------------------------------------------------------------------

# Base address for the CLKCTRL module
CLKCTRL_BASE_ADDRESS = 0x4002D000

# Clock Control Register Address Offsets
# (Register Address = Peripheral Base Address + Offset)
OFF_CLKCTRL_APBCLKG0 = 0x00000020               # APB Clock Gate 0 Register
OFF_CLKCTRL_APBCLKG0_SET = 0x00000024           # Set register: sets bits to 1 (masked by value)
OFF_CLKCTRL_APBCLKG0_CLR = 0x00000028           # Clear register: clears bits to 0 (masked by value)

# CLKCTRL.APBCLKG0 Masks
MASK_APBCLKG0_ENABLE_ALL_CLOCKS = 0xFFFFFFFF    # Enable FLASHCTRL APB clock
												# The FLASHCTRL clock enable bit is in a different
												# location on SiM3L1xx and SiM3U/C1xx devices, so
												# enable clocks to all peripherals

#------------------------------------------------------------------------------
# DAP (Debug Access Port) Access Functions
#------------------------------------------------------------------------------

def read_DAP(uda, select, address):
	"""Read one 32-bit DAP register.
	:param select: DP.SELECT value selects the AP and AP bank
	:param address: AP register address within the selected AP bank
	"""
	uda.QueueWrite(DP_SELECT, select)
	uda.QueueRead(address)
	return uda.StartTransfers()[0]

def write_DAP(uda, select, address, data):
	"""Write one 32-bit DAP register.
	:param select: DP.SELECT value selects the AP and AP bank
	:param address: AP register address within the selected AP bank
	:param data: 32-bit value to write
	"""
	uda.QueueWrite(DP_SELECT, select)
	uda.QueueWrite(address, data)
	uda.StartTransfers()

def read_AHB(uda, address):
	"""Use MEMAP to read one 32-bit word on the AHB bus.
	:param address: AHB address to read
	"""
	uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
	uda.QueueWrite(MEMAP_CSW, 0x23000002)
	uda.QueueWrite(MEMAP_TAR, address)
	uda.QueueRead(MEMAP_DRW)
	return uda.StartTransfers()[0]

def write_AHB(uda, address, data):
	"""Use MEMAP to write one 32-bit word on the AHB bus.
	:param address: AHB address to write
	:param data: 32-bit value to write
	"""
	uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
	uda.QueueWrite(MEMAP_CSW, 0x23000002)
	uda.QueueWrite(MEMAP_TAR, address)
	uda.QueueWrite(MEMAP_DRW, data)
	uda.StartTransfers()


#------------------------------------------------------------------------------
# Flash Programming Functions
#------------------------------------------------------------------------------

def connect_and_halt_core(uda):
	"""Connect the Serial Wire Debug Port (DP-SWD) and halt the device."""

	# Confirm the Chip_AP ID is what we expect
	chipap_id = read_DAP(uda, CHIPAP_BANK_F, CHIPAP_ID)
	if chipap_id != 0x2430002:
		# Not a SiLabs Chip_AP block
		return

	# Connect and halt the core using the Chip_AP
	write_DAP(uda, CHIPAP_BANK_0, CHIPAP_CTRL1, 0x8)    # CTRL1.core_reset_ap = 1
	write_AHB(uda, DHCSR, 0xA05F0001)                   # DHCSR.C_DEBUGEN = 1
	write_AHB(uda, DEMCR, 0x1)                          # DEMCR.VC_CORERESET = 1
	write_AHB(uda, AIRCR, 0x05FA0004)                   # reset the core
	write_DAP(uda, CHIPAP_BANK_0, CHIPAP_CTRL1, 0x0)    # CTRL1.core_reset_ap = 0

def enable_flashctrl_clock(uda):
	"""Enable the APB clock to the FLASHCTRL module registers."""

	write_AHB(uda, CLKCTRL_BASE_ADDRESS + OFF_CLKCTRL_APBCLKG0_SET, MASK_APBCLKG0_ENABLE_ALL_CLOCKS)

def device_erase(uda):
	"""Bulk erase the device flash using the Silicon Labs Chip_AP.
	The device must already be halted."""

	# Bulk erase all non-reserved flash
	write_DAP(uda, CHIPAP_BANK_0, CHIPAP_CTRL1, 0x1)    # CTRL1.user_erase = 1
	chipap_ctrl1 = read_DAP(uda, CHIPAP_BANK_0, CHIPAP_CTRL1)
	while chipap_ctrl1 & 0x1:
		chipap_ctrl1 = read_DAP(uda, CHIPAP_BANK_0, CHIPAP_CTRL1)

	# Pulse sysreset so the device unlocks
	write_DAP(uda, CHIPAP_BANK_0, CHIPAP_CTRL1, 0x4)    # CTRL1.sysreset_req_ap = 1
	write_DAP(uda, CHIPAP_BANK_0, CHIPAP_CTRL1, 0x0)    # CTRL1.sysreset_req_ap = 0


def write_sequential_words(uda, address, data_words, length):
	"""Write words in an array (list) to flash.
	Clocks must already be enabled and the device must be halted."""

	# If data_halfwords is an int, cast it as a list
	if isinstance(data_words, int):
		data_words = [data_words]

	# Write the flash word

	# Disable flash page erases
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_CONFIG_CLR, MASK_FLASH_CONFIG_ERASE_ENABLE)

	# Set the address to write
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_ADDRESS, address)

	# Set up the flash controller for sequential writes
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_CONFIG_SET, MASK_FLASH_CONFIG_SEQUENTIAL)

	# Unlock the FLASHCTRL interface for writes/erases
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_KEY, 0xA5)
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_KEY, 0xF2)

	# Set up the MEMAP to write the data to the FLASHCTRL write data register
	# Non-incrementing mode
	uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
	uda.QueueWrite(MEMAP_CSW, 0x23000002)
	uda.QueueWrite(MEMAP_TAR, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_DATA)
	uda.StartTransfers()

	# Write all the words to flash
	# Intended to be simple, this implementation writes single commands across the USB HID
	# interface for each 16 bits of data, which is very slow and inefficient.  In an actual
	# programmer implementation, this process should write data to the serial wire debug port
	# as efficiently and quickly as possible.
	for x in range (0, length):

		# Writes are 16-bits
		uda.QueueWrite(MEMAP_DRW, 0x0000FFFF & data_words[x])
		uda.QueueWrite(MEMAP_DRW, (0xFFFF0000 & data_words[x]) >> 16)
		uda.StartTransfers()

	# Clean up after the write operations

	# Lock flash writes/erases
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_KEY, 0x5A)


def read_sequential_words(uda, address, length):
	"""Read words in an array (list) from flash.
	The device must already be halted."""

	data_words = []

	# Auto increment addresses
	uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
	uda.QueueWrite(MEMAP_CSW, 0x23000012)
	uda.QueueWrite(MEMAP_TAR, address)

	# length is given in word addresses, so translate these to device
	# addresses in bytes
	for x in range (0, length):
		uda.QueueRead(MEMAP_DRW)
		data_words.append(uda.StartTransfers()[0])

	return data_words

def swd_write_mem(uda, address, data_ws, length):
	"""Write ws in an array (list) to SRAM.
	Clocks must already be enabled and the device must be halted."""

	# Auto increment addresses
	uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
	uda.QueueWrite(MEMAP_CSW, 0x23000012)

	uda.QueueWrite(MEMAP_TAR, address)
	for x in range (0, length):
		uda.QueueWrite(MEMAP_DRW, data_ws[x])
		uda.StartTransfers()

def swd_read_mem(uda, address, length):
	"""Read words in an array (list) from SRAM.
	The device must already be halted."""

	data_words = []

	# Auto increment addresses
	uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
	uda.QueueWrite(MEMAP_CSW, 0x23000012)
	uda.QueueWrite(MEMAP_TAR, address)

	# length is given in word addresses, so translate these to device
	# addresses in bytes
	for x in range (0, length):
		uda.QueueRead(MEMAP_DRW)
		data_words.append(uda.StartTransfers()[0])

	return data_words

def swd_write_core_register(uda, n, val):
	"""Write vlaue to Cortez M3 core register."""

	uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
	uda.QueueWrite(MEMAP_CSW, 0x23000002)
	uda.QueueWrite(MEMAP_TAR, DCRDR)
	uda.QueueWrite(MEMAP_DRW, val)
	uda.StartTransfers()

	uda.QueueWrite(MEMAP_TAR, DCRSR)
	uda.QueueWrite(MEMAP_DRW, n | (1 << 16))
	uda.StartTransfers()


def swd_read_core_register(uda, n):
	"""Read Cortez M3 core register value in an array (list)."""
	uda.QueueWrite(DP_SELECT, MEMAP_BANK_0)
	uda.QueueWrite(MEMAP_CSW, 0x23000002)
	uda.QueueWrite(MEMAP_TAR, DCRSR)
	uda.QueueWrite(MEMAP_DRW, n)
	uda.StartTransfers()

	uda.QueueWrite(MEMAP_TAR, DCRDR)
	uda.QueueRead(MEMAP_DRW)
	val = uda.StartTransfers()[0]

	return val


def erase_page(uda, address):
	"""Erase the flash page that contains the address specified.
	Clocks must already be enabled and the device must be halted."""

	# Erase the flash page

	# Enable flash page erases
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_CONFIG_SET, MASK_FLASH_CONFIG_ERASE_ENABLE)

	# Set the flash page to erase
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_ADDRESS, address)

	# Unlock the FLASHCTRL interface for writes/erases
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_KEY, 0xA5)
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_KEY, 0xF2)

	# Erase the flash page using a dummy data byte
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_DATA, 0x00)

	# Wait for the flash busy bit to clear
	flash_config = read_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_CONFIG)
	while flash_config & MASK_FLASH_CONFIG_BUSY:
		flash_config = read_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_CONFIG)

	# Clean up after the erase

	# Disable flash page erases
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_CONFIG_CLR, MASK_FLASH_CONFIG_ERASE_ENABLE)

	# Lock flash writes/erases
	write_AHB(uda, FLASHCTRL_BASE_ADDRESS + OFF_FLASH_WRITE_KEY, 0x5A)

def sram_programming(uda):
	filename = "sim3u1xx_Blinky.bin"
	# filename = "sim3u1xx_USBHID_ram.bin"
	print(sys.version_info)
	data = []
	f = open(filename,mode = 'rb')
	binraw = f.read()
	for i in range(0, len(binraw), 4):
		if (sys.version_info < (3, 0, 0)):
			b0 = ord(binraw[i])
			b1 = ord(binraw[i + 1])
			b2 = ord(binraw[i + 2])
			b3 = ord(binraw[i + 3])
		else:
			b0 = (binraw[i])
			b1 = (binraw[i + 1])
			b2 = (binraw[i + 2])
			b3 = (binraw[i + 3])
		tmp = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24)
		data.append(tmp)
	# print(','.join(hex(x) for x in data))
	f.close()
	# Start programing firmware into SRAM
	size = int(len(binraw) / 4)
	print('Size is %d'%size)
	recv = []
	for i in range(0, size, 1024):
		if (i + 1024) < size:
			count = 1024
		else:
			count = size - i

		print('Offset = %d'%(count+i))
		swd_write_mem(uda, SRAM_ADDR + i * 4, data[i:], count)
		recv[i:] = swd_read_mem(uda, SRAM_ADDR + i * 4, count)

	error = 0
	for i in range (0, size):
		if recv[i] != data[i]:
			error = error + 1
			if error < 100:
				print('0x%x, %d'%(recv[i], i))

	if error == 0:
		print('Data verified!')
	else:
		print('%d error happens'%error)

	# reset vector entry address
	rst_isr = (data[1] & 0xFFFFFFFE)
	# print(hex(rst_isr))
	data[0] = 0x20000000
	# update vector table and PC
	swd_write_mem(uda, 0xe000ed08, data, 1)
	# recv = swd_read_core_register(uda, 15)
	swd_write_core_register(uda, 15, rst_isr)
	recv = swd_read_core_register(uda, 15)
	# print(hex(recv))

	data[0] = 0x20008000
	swd_write_core_register(uda, 13, data[0])
	write_AHB(uda, DHCSR, 0xA05F0000)
	tmp = read_AHB(uda, DHCSR)
	# print(hex(tmp))


#------------------------------------------------------------------------------
# The Application
#------------------------------------------------------------------------------
# Open the first available debug adapter
uda = adi.AdiDevice()
uda.Open()

# Connect using Serial Wire and enable debug features
uda.ConnectSWD()
uda.LineReset()
write_DAP(uda, MEMAP_BANK_0, DP_CTRLSTAT, 0x50000000)

# connect_and_halt_core(uda)

# Bulk erase the entire user flash
print('Erasing all flash in the device...')
device_erase(uda)
print('done!')

connect_and_halt_core(uda)

# Clocks must be enabled to the flash controller to write/erase flash
enable_flashctrl_clock(uda)

# Write a set of halfwords to two pages
print('\nWriting test data to addresses 0x00000200 and 0x00000400...', end='')
write_data_words = [0xA5A50000, 0x88885A5A, 0x1111FFEE, 0x11FFEEEE]
write_sequential_words(uda, 0x00000200, write_data_words, 4)
write_data_words.reverse()
write_sequential_words(uda, 0x00000400, write_data_words, 4)
print(' done!')

# Read the data from flash
print('\nReading test data to address 0x00000200...', end='')
write_data_words.reverse()
read_data_words = read_sequential_words(uda, 0x00000200, 4)
if set(write_data_words) & set(read_data_words):
	print(' data verified!')
else:
	print(' error in data!')
print('Write: [', ', '.join([hex(i) for i in write_data_words]), ']')
print('Read: [', ', '.join([hex(i) for i in read_data_words]), ']')

print('\nReading test data to address 0x00000400...', end='')
write_data_words.reverse()
read_data_words = read_sequential_words(uda, 0x00000400, 4)
if set(write_data_words) & set(read_data_words):
	print(' data verified!')
else:
	print(' error in data!')
print('Write: [', ', '.join([hex(i) for i in write_data_words]), ']')
print('Read: [', ', '.join([hex(i) for i in read_data_words]), ']')

# Erase the 0x00001000 page of flash
print('\nErasing page 0x00000200...', end='')
erase_page(uda, 0x00000200)
print(' done!')

# Read the data from flash
print('\nReading test data to address 0x00000200...', end='')
data_words = [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF]
read_data_words = read_sequential_words(uda, 0x00000200, 4)
if set(data_words) & set(read_data_words):
	print(' data verified!')
else:
	print(' error in data!')
print('Erased data: [', ', '.join([hex(i) for i in data_words]), ']')
print('Read: [', ', '.join([hex(i) for i in read_data_words]), ']')

# SRAM programming test
print('\nStart SRAM programming')
sram_programming(uda)
print('\nSRAM programming done')


# Disable debug and disconnect before exiting
write_DAP(uda, MEMAP_BANK_0, DP_CTRLSTAT, 0x00000000)
uda.Disconnect()
uda.Close()

