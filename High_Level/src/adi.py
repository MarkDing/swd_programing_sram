
## Copyright (c) 2012-2013 by Silicon Laboratories.
## All rights reserved. This program and the accompanying materials
## are made available under the terms of the Silicon Laboratories End User
## License Agreement which accompanies this distribution, and is available at
## http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
## Original content and implementation provided by Silicon Laboratories.

"""
Python wrapper for Silabs 32-bit debug adapter library (SLAB_ADI.dll).

Documentation for the library is provided by the help file SLAB_ADI.chm.

ADI - ARM Debug Interface.
"""

import ctypes

__version__ = "0.0.4"
__date__ = "28 Nov 2012"

__all__ = ['ADI_VID', 'ADI_PID',
    'ADI_DEVICE_MODE', 'ADI_DAP', 'ADI_PROP_ID', 'ADI_STATUS_DESC',
    'AdiDevice', 'AdiError', 'GetNumDevices', 'GetSerial', 'GetAttributes',
    'GetHidLibraryVersion', 'GetLibraryVersion', 'IsAvailable',
    'GetDeviceFilter', 'SetDeviceFilter',
    'GetDeviceFilterEnable', 'SetDeviceFilterEnable']


#==============================================================================
# Constants
#==============================================================================

class ADI_VID:
    SLAB = 0x10C4

class ADI_PID:
    UDA = 0x8045
    TS = 0x8253

class ADI_DEVICE_MODE:
    BOOTLOAD = 0x01
    DEBUG_8051 = 0x02
    DEBUG_ARM = 0x03

class ADI_PROP_ID:
    RST = 0x01
    LED = 0x02

class ADI_DAP:
    IDCODE = 0x00
    CTRLSTAT = 0x04
    SELECT = 0x08
    CSW = 0x01
    TAR = 0x05
    DRW = 0x0D
    BD0 = 0x01
    BD1 = 0x05
    BD2 = 0x09
    BD3 = 0x0D


#==============================================================================
# Error Handling
#==============================================================================

ADI_STATUS_DESC = {
    0x01 : "ADI_STATUS_PROT_INVALID_COMMAND",
    0x02 : "ADI_STATUS_PROT_COMMAND_FAILED",
    0x03 : "ADI_STATUS_PROT_AP_TIMEOUT",
    0x04 : "ADI_STATUS_PROT_WIRE_ERROR",
    0x05 : "ADI_STATUS_PROT_ACK_FAULT",
    0x06 : "ADI_STATUS_PROT_DP_NOT_CONNECTED",
    0x40 : "ADI_STATUS_API_INVALID_PARAMETER",
    0x41 : "ADI_STATUS_API_INVALID_BUFFER_SIZE",
    0x42 : "ADI_STATUS_API_NOT_SUPPORTED",
    0x43 : "ADI_STATUS_API_NOT_IN_BOOTLOAD_MODE",
    0x44 : "ADI_STATUS_API_NOT_IN_DEBUG_MODE",
    0x45 : "ADI_STATUS_API_NOT_IN_ARM_DEBUG_MODE",
    0x46 : "ADI_STATUS_API_FAILED_TO_ENTER_MODE",
    0x47 : "ADI_STATUS_API_INVALID_TRANSFER",
    0x48 : "ADI_STATUS_API_INVALID_HEX_CHECKSUM",
    0x49 : "ADI_STATUS_API_INVALID_HEX_RECORD",
    0x4A : "ADI_STATUS_API_INVALID_HEX_FILE",
    0x4B : "ADI_STATUS_API_INVALID_DEVICE_OBJECT",
    0x4C : "ADI_STATUS_API_FATAL_ERROR",
    0x4D : "ADI_STATUS_API_ABORTED",
    0x4E : "ADI_STATUS_API_FW_UPGRADE_REQUIRED",
    0x4F : "ADI_STATUS_API_NOT_CONNECTED_TO_TARGET",
    0x50 : "ADI_STATUS_API_TARGET_MUST_BE_HALTED",
    0x51 : "ADI_STATUS_API_FLASH_VERIFY_FAILED",
    0x80 : "ADI_STATUS_HWIF_DEVICE_NOT_FOUND",
    0x81 : "ADI_STATUS_HWIF_DEVICE_NOT_OPENED",
    0x82 : "ADI_STATUS_HWIF_DEVICE_ERROR",
    0x83 : "ADI_STATUS_HWIF_TRANSFER_ERROR",
    0x84 : "ADI_STATUS_HWIF_TRANSFER_TIMEOUT",
    0xC0 : "ADI_STATUS_SYS_INVALID_RESPONSE",
    0xC1 : "ADI_STATUS_SYS_FLASH_WRITE_CRC_ERROR",
    0xC2 : "ADI_STATUS_SYS_RE_ENUMERATE_DEVICE",
}

class AdiError(Exception):
    def __init__(self, status):
        self.status = status
        try:
            self.name = ADI_STATUS_DESC[status]
        except:
            self.name = "ADI_STATUS_UNKNOWN: " + hex(status)
    def __str__(self):
        return self.name

def adi_errcheck(result, func, args):
    if result != 0:
        raise AdiError(result)


#==============================================================================
# ARM Debug Interface DLL
#==============================================================================

_DLL = ctypes.windll.LoadLibrary("SLAB_ADI.dll")

_DLL.ADI_IsOpened.restype = ctypes.c_bool
_DLL.ADI_DBG_IsConnected.restype = ctypes.c_bool

for adi_function in ["ADI_GetNumDevices",
    "ADI_GetAttributes", "ADI_GetSerial",
    "ADI_GetDeviceFilter", "ADI_SetDeviceFilter",
    "ADI_GetDeviceFilterEnable", "ADI_SetDeviceFilterEnable",
    "ADI_GetHidLibraryVersion", "ADI_GetLibraryVersion",
    "ADI_GetBootloaderVersion", "ADI_DBG_GetDebugVersion",
    "ADI_OpenByIndex", "ADI_Close",
    "ADI_GetOpenedAttributes", "ADI_GetOpenedSerial",
    "ADI_DBG_ConnectJtag", "ADI_DBG_ConnectSwd", "ADI_DBG_Disconnect",
    "ADI_GetDeviceMode", "ADI_SetDeviceMode",
    "ADI_DBG_GetProperty", "ADI_DBG_SetProperty",
    "ADI_DBG_ClearErrors", "ADI_DBG_LineReset",
    "ADI_DBG_QueueRead", "ADI_DBG_QueueWrite",
    "ADI_DBG_RepeatRead", "ADI_DBG_RepeatWrite",
    "ADI_DBG_StartTransfers", "ADI_BL_DownloadHexFile"]:
    fnc = getattr(_DLL, adi_function)
    fnc.restype = ctypes.c_ubyte
    fnc.errcheck = adi_errcheck


#==============================================================================
# Library Functions
#==============================================================================

def GetNumDevices():
    """Returns the number of debug adapters connected to the host."""
    cnt = ctypes.c_ulong()
    _DLL.ADI_GetNumDevices(ctypes.byref(cnt))
    return cnt.value

def GetSerial(index=0):
    """Returns the serial number string for the debug adapter selected by index.
    Throws an error if the selected index is already open.
    """
    buf = ctypes.create_string_buffer(512)
    _DLL.ADI_GetSerial(index, buf)
    return buf.value.decode()

def GetAttributes(index=0):
    """Returns VID, PID and release number for the debug adapter selected by index.
    Throws an error if the selected index is already open.
    """
    vid = ctypes.c_ulong()
    pid = ctypes.c_ulong()
    rel = ctypes.c_ulong()
    _DLL.ADI_GetAttributes(index, ctypes.byref(vid), ctypes.byref(pid), ctypes.byref(rel))
    return tuple([vid.value, pid.value, rel.value])

def IsAvailable(index=0, arm_only=True):
    """Checks if the debug adapter selected by index is available.

    :param arm_only: if True, only checks for 32-bit adapters
    """
    result = True
    handle = ctypes.c_int(0)
    try:
        _DLL.ADI_OpenByIndex(ctypes.byref(handle), index)
        if arm_only:
            _DLL.ADI_SetDeviceMode(handle, ADI_DEVICE_MODE.DEBUG_ARM)
    except AdiError:
        result = False
    if handle.value:
        _DLL.ADI_Close(handle)
    return result

def GetLibraryVersion():
    """Returns the SLAB_ADI library version number as a string."""
    major = ctypes.c_ulong()
    minor = ctypes.c_ulong()
    release = ctypes.c_ulong()
    _DLL.ADI_GetLibraryVersion(ctypes.byref(major), ctypes.byref(minor), ctypes.byref(release))
    return "{}.{}.{}".format(major.value, minor.value, release.value)

def GetHidLibraryVersion():
    """Returns the SLABHIDDevice library version number as a string."""
    major = ctypes.c_ulong()
    minor = ctypes.c_ulong()
    release = ctypes.c_ulong()
    _DLL.ADI_GetHidLibraryVersion(ctypes.byref(major), ctypes.byref(minor), ctypes.byref(release))
    return "{}.{}.{}".format(major.value, minor.value, release.value)

def GetDeviceFilter():
    """Returns (VID, PID) used when the device filter is enabled."""
    vid = ctypes.c_ulong()
    pid = ctypes.c_ulong()
    _DLL.ADI_GetDeviceFilter(ctypes.byref(vid), ctypes.byref(pid))
    return tuple([vid.value, pid.value])

def SetDeviceFilter(vid=ADI_VID.SLAB, pid=ADI_PID.UDA):
    """Sets (VID, PID) used when the device filter is enabled."""
    _DLL.ADI_SetDeviceFilter(vid, pid)

def GetDeviceFilterEnable():
    """Returns enable status of the device filter."""
    filter = ctypes.c_bool()
    _DLL.ADI_GetDeviceFilterEnable(ctypes.byref(filter))
    return filter.value

def SetDeviceFilterEnable(filter=True):
    """Enables or disables the device filter."""
    _DLL.ADI_SetDeviceFilterEnable(filter)


#==============================================================================
# Debug Adapter Class
#==============================================================================

class AdiDevice:
    """
    AdiDevice instances are used to work with a specific debug adapter.

    For documentation on the wrapped functions, refer to the help file SLAB_ADI.chm.
    """

    def __init__(self, index=None):
        self.handle = ctypes.c_int(0)
        self.idcode = 0
        GetNumDevices()

    def __str__(self):
        return "AdiDevice Hnd:"+hex(self.handle.value)+" Id:"+hex(self.idcode)

    @property
    def vid_pid(self):
        vid = ctypes.c_ulong(0)
        pid = ctypes.c_ulong(0)
        rel = ctypes.c_ulong(0)
        if self.IsOpened():
            _DLL.ADI_GetOpenedAttributes(self.handle, ctypes.byref(vid), ctypes.byref(pid), ctypes.byref(rel))
        return tuple([vid.value, pid.value])

    @property
    def serial_number(self):
        buf = ctypes.create_string_buffer(512)
        if self.IsOpened():
            _DLL.ADI_GetOpenedSerial(self.handle, buf)
        return buf.value.decode()

    @property
    def bootload_version(self):
        version = ctypes.c_ulong(0)
        if self.IsOpened():
            _DLL.ADI_GetBootloaderVersion(self.handle, ctypes.byref(version))
        return version.value

    @property
    def firmware_version(self):
        version = ctypes.c_ulong(0)
        try:
            _DLL.ADI_DBG_GetDebugVersion(self.handle, ctypes.byref(version))
        except AdiError:
            pass
        return version.value

    def OpenByIndex(self, index, debug=True):
        """Trys to open adapter selected by the driver index.

        :param debug: if True, starts ARM debug firmware
        """
        if self.IsOpened():
            self.Close()
        try:
            # throws error if index is already open
            _DLL.ADI_OpenByIndex(ctypes.byref(self.handle), index)
            if debug:
                # throws error if device has 8-bit debug firmware
                _DLL.ADI_SetDeviceMode(self.handle, ADI_DEVICE_MODE.DEBUG_ARM)
        except AdiError:
            if self.IsOpened():
                _DLL.ADI_Close(self.handle)
                self.handle.value = 0
            raise

    def OpenBySerial(self, serial, debug=True):
        """Trys to open adapter with the specified serial number.

        :param debug: if True, starts ARM debug firmware
        """
        for i in range(GetNumDevices()):
            try:
                sn = GetSerial(i)
            except AdiError:
                continue
            if sn == serial:
                self.OpenByIndex(i, debug)
                return
        # 0x80 : "ADI_STATUS_HWIF_DEVICE_NOT_FOUND"
        raise AdiError(0x80)

    def Open(self):
        """Opens the first available 32-bit adapter."""
        for i in range(GetNumDevices()):
            try:
                self.OpenByIndex(i)
                return
            except AdiError:
                continue
        # 0x80 : "ADI_STATUS_HWIF_DEVICE_NOT_FOUND"
        raise AdiError(0x80)

    def Close(self):
        if self.handle.value != 0:
            self.Disconnect()
            _DLL.ADI_Close(self.handle)
            self.handle.value = 0

    def IsOpened(self):
        return _DLL.ADI_IsOpened(self.handle)

    def GetDeviceMode(self):
        mode = ctypes.c_ulong()
        _DLL.ADI_GetDeviceMode(self.handle, ctypes.byref(mode))
        return mode.value

    def SetDeviceMode(self, mode):
        _DLL.ADI_SetDeviceMode(self.handle, mode)

    def GetProperty(self, prop_id):
        value = ctypes.c_ulong()
        _DLL.ADI_DBG_GetProperty(self.handle, prop_id, ctypes.byref(value))
        return value.value

    def SetProperty(self, prop_id, value):
        _DLL.ADI_DBG_SetProperty(self.handle, prop_id, value)

    def ConnectJTAG(self):
        id_code = ctypes.c_ulong()
        _DLL.ADI_DBG_ConnectJtag(self.handle, 0, 0, 0, 0, ctypes.byref(id_code))
        self.idcode = id_code.value
        self.ClearErrors()
        return id_code.value

    def ConnectSWD(self, swj=1, baud=0):
        id_code = ctypes.c_ulong()
        _DLL.ADI_DBG_ConnectSwd(self.handle, swj, (0 != baud), baud, ctypes.byref(id_code))
        self.idcode = id_code.value
        self.ClearErrors()
        return id_code.value

    def Disconnect(self):
        self.idcode = 0
        if self.IsConnected():
            _DLL.ADI_DBG_Disconnect(self.handle)

    def IsConnected(self):
        if self.handle.value != 0:
            return _DLL.ADI_DBG_IsConnected(self.handle)
        else:
            return False

    def ClearErrors(self):
        before = ctypes.c_ulong()
        after = ctypes.c_ulong()
        _DLL.ADI_DBG_ClearErrors(self.handle, ctypes.byref(before), ctypes.byref(after))
        return tuple([before.value, after.value])

    def LineReset(self):
        _DLL.ADI_DBG_LineReset(self.handle)

    def QueueRead(self, address):
        _DLL.ADI_DBG_QueueRead(self.handle, (address | 0x02))

    def QueueWrite(self, address, data):
        _DLL.ADI_DBG_QueueWrite(self.handle, address, data)

    def RepeatRead(self, count=1, address=ADI_DAP.DRW):
        words = (ctypes.c_ulong * count)()
        _DLL.ADI_DBG_RepeatRead(self.handle, count, (address | 0x02), words)
        return list(words)

    def RepeatWrite(self, data, address=ADI_DAP.DRW):
        count = len(data)
        words = (ctypes.c_ulong * count)(*data)
        _DLL.ADI_DBG_RepeatWrite(self.handle, count, address, words)

    def StartTransfers(self):
        size = ctypes.c_ulong()
        _DLL.ADI_DBG_GetNumReads(self.handle, ctypes.byref(size))
        read = ctypes.c_ulong()
        words = (ctypes.c_ulong * size.value)()
        _DLL.ADI_DBG_StartTransfers(self.handle, words, size, ctypes.byref(read))
        return list(words)


if __name__ == "__main__":
    print('')
    print("     SLAB_ADI:", GetLibraryVersion())
    print("SLABHIDDevice:", GetHidLibraryVersion())
    print('')
    print("Available Adapters:")
    for i in range(GetNumDevices()):
        if IsAvailable(i):
            print(i, '-', GetSerial(i))
    try:
        uda = AdiDevice()
        uda.Open()
        print('')
        print("Serial Num:", uda.serial_number)
        print(" VID - PID:", hex(uda.vid_pid[0]), "-", hex(uda.vid_pid[1]))
        print("Bootloader:", hex(uda.bootload_version))
        print("  Firmware:", hex(uda.firmware_version))
    except AdiError as e:
        print("Device Error:", e, "-", hex(e.status))
    finally:
        uda.Close()
