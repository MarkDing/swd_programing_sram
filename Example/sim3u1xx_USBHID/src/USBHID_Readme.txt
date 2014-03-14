--------------------------------------------------------------------------------
 Copyright (c) 2012 by Silicon Laboratories.
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Silicon Laboratories End User
 License Agreement which accompanies this distribution, and is available at
 http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
 Original content and implementation provided by Silicon Laboratories.
--------------------------------------------------------------------------------

Program Description:
--------------------------------------------------------------------------------

This example code senses the raw position detected on the capsense slider and
translates it into a HID Keypad key press event.  If the device is connected to
a USB host, it will interpret the event as a keyboard input, and type the character
to Notepad or a command line console.

The code also illuminates LEDs DS3 and DS4 when the slider is touched.
It will adjust the brightness on DS3 depending on where the slider is touched.
The code will turn off DS4 when the slider is no longer senses a touch, but
DS3 will remain at the illumination it last sensed.

Resources Used:
--------------------------------------------------------------------------------
CAPSENSE0 module
PB0.1  (capsense input)
PB0.2  (capsense input)
PB0.3  (capsense input)
PB0.4  (capsense input)
PB1.7  (capsense input)
PB1.8  (capsense input)
PB1.3  (debug printf)
PB2.10 (led)
PB2.11 (led)
USB0 module
USBEP0 (default endpoint)
USBEP1 (hid interrupt endpoint)

Notes On Example and Modes:
--------------------------------------------------------------------------------
Default Mode:
   AHB 20 MHz
   APB 20 MHz
   CAPSENSE module takes measurement on each channel and then loops
   YEllow LED (DS4) will illuminate when the slider is touched
   Red LED (DS3) will illuminate with a brightness level from 1-10 depending on
      where the slider is touched (position A is the brightest and B is the
	  dimmest).
   The position where the touch was detected is output via USBHID reports.


How to Use:
--------------------------------------------------------------------------------
1) Download the code to a SiM3U1xx device on an SiM3U1xx MCU Card
2) Power the MCU Card board. The easiest way to do this is by connecting a USB
   cable to J13 ("Device USB" connector).
3) In the IDE, open the debug printf viewer and then run the code.
   In uVision, the debug printf viewer can be opened by starting a debug session
   and then clicking View -> Serial Windows -> Debug (printf) Viewer.
4) Set the USER CONTROL switch to ON.
5) Open up Notepad on the Host, and ensure that it has the focus for keyboard
   input.
6) Run the code: The yellow LED (DS4) will illuminate when the slider is touched
   and the red LED (DS3) will illuminate with a brightness level relative to
   where the slider is touched.

   The computer will type out a number from 0 through 9, depending on where the
   capsense detected a touch.

Note: The PB1.8 pin is tied to the UDP bus in addition to a CAPSENSE channel.
   This causes the PB1.8 pin to measure higher than the other pins.  Remove R50
   to improve the performance of this pin. This resistor will have to be readded
   to enable some UDP EBID functionality.

Note2: We recommend not using the CAPSENSE on the SiM3U1xx or SiM3C1xx MCU cards
   while connected to the UDP motherboard because of the way the pins are shared
   between CAPSENSE and other functions.



Revision History:
--------------------------------------------------------------------------------
Release 1.2
    -Swapped last two channels and added a note about removing R50 for increased
     performance.
    -2 MAY 12
Release 1.1
    -Update conversion rate parameter in default configuration (ES)
    -11 APR 12
Release 1.0
    -Initial Revision (ES)
    -31 JAN 12

---eof--------------------------------------------------------------------------

