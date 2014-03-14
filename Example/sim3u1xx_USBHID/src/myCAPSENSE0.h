//------------------------------------------------------------------------------
// Copyright (c) 2012 by Silicon Laboratories. 
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Silicon Laboratories End User 
// License Agreement which accompanies this distribution, and is available at
// http://developer.silabs.com/legal/version/v10/License_Agreement_v10.htm
// Original content and implementation provided by Silicon Laboratories.
//------------------------------------------------------------------------------

#ifndef __MYCAPSENSE_H__
#define __MYCAPSENSE_H__

#include <stdbool.h>

// INCLUDE GENERATED CONTENT
#include "gCAPSENSE0.h"


// Starts a scan of the slider. This results in multiple convert complete
// interrupts. When the scan is complete the touch position is calculated
void scan_slider(void);

// Perform initial capsense baselining
void calibrate_capsense(void);


//==============================================================================
//EXPORT PUBLIC VARS FOR OTHER MODULES
//==============================================================================
// holds position(virtual) of last detected touch
extern uint8_t CapsenseTouchPos;

//0 if no touch seen durring last poll, else 1
extern bool CapsenseTouch;

#endif //__MYCAPSENSE_H__

//-eof--------------------------------------------------------------------------
