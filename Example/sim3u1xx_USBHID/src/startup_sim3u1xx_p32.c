//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Copyright (c) 2011 Code Red Technologies Ltd.
//
// Microcontroller Startup code for use with Red Suite
//
// Version : 110815
//
// Software License Agreement
//
// The software is owned by Code Red Technologies and/or its suppliers, and is
// protected under applicable copyright laws.  All rights are reserved.  Any
// use in violation of the foregoing restrictions may subject the user to criminal
// sanctions under applicable laws, as well as to civil liability for the breach
// of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD.
//
//*****************************************************************************
#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" {
	extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

// Code Red - if CMSIS is being used, then SystemInit() routine
// will be called by startup code rather than in application's main()
#if defined (__USE_CMSIS)
#include "sim3u1xx.h"
#endif

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
//
// Forward declaration of the default handlers. These are aliased.
// When the application defines a handler (with the same name), this will
// automatically take precedence over these weak definitions
//
//*****************************************************************************
     void ResetISR(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVCall_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//
//*****************************************************************************

void WDTIMER0_IRQHandler(void) ALIAS(IntDefaultHandler);
void PBEXT0_IRQHandler(void) ALIAS(IntDefaultHandler);
void PBEXT1_IRQHandler(void) ALIAS(IntDefaultHandler);
void RTC0ALRM_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH0_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH1_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH2_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH3_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH4_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH5_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH6_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH7_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH8_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH9_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH10_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH11_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH12_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH13_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH14_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMACH15_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER0L_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER0H_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER1L_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER1H_IRQHandler(void) ALIAS(IntDefaultHandler);
void EPCA0_IRQHandler(void) ALIAS(IntDefaultHandler);
void PCA0_IRQHandler(void) ALIAS(IntDefaultHandler);
void PCA1_IRQHandler(void) ALIAS(IntDefaultHandler);
void USART0_IRQHandler(void) ALIAS(IntDefaultHandler);
void USART1_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI0_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI1_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI2_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C0_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C1_IRQHandler(void) ALIAS(IntDefaultHandler);
void USB0_IRQHandler(void) ALIAS(IntDefaultHandler);
void SARADC0_IRQHandler(void) ALIAS(IntDefaultHandler);
void SARADC1_IRQHandler(void) ALIAS(IntDefaultHandler);
void CMP0_IRQHandler(void) ALIAS(IntDefaultHandler);
void CMP1_IRQHandler(void) ALIAS(IntDefaultHandler);
void CAPSENSE0_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2S0RX_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2S0TX_IRQHandler(void) ALIAS(IntDefaultHandler);
void AES0_IRQHandler(void) ALIAS(IntDefaultHandler);
void VDDLOW_IRQHandler(void) ALIAS(IntDefaultHandler);
void RTC0FAIL_IRQHandler(void) ALIAS(IntDefaultHandler);
void PMATCH_IRQHandler(void) ALIAS(IntDefaultHandler);
void UART0_IRQHandler(void) ALIAS(IntDefaultHandler);
void UART1_IRQHandler(void) ALIAS(IntDefaultHandler);
void IDAC0_IRQHandler(void) ALIAS(IntDefaultHandler);
void IDAC1_IRQHandler(void) ALIAS(IntDefaultHandler);
void LPTIMER0_IRQHandler(void) ALIAS(IntDefaultHandler);
void PLL0_IRQHandler(void) ALIAS(IntDefaultHandler);
void VBUSINVALID_IRQHandler(void) ALIAS(IntDefaultHandler);
void VREG0LOW_IRQHandler(void) ALIAS(IntDefaultHandler);


//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//
//*****************************************************************************
#if defined (__REDLIB__)
extern void __main(void);
#endif
extern int main(void);
//*****************************************************************************
//
// External declaration for the pointer to the stack top from the Linker Script
//
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif
//*****************************************************************************
//
// The vector table.
// This relies on the linker script to place at correct location in memory.
//
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
	// Core Level - CM3
	&_vStackTop, // The initial stack pointer
	ResetISR,								// The reset handler
	NMI_Handler,							// The NMI handler
	HardFault_Handler,						// The hard fault handler
	MemManage_Handler,						// The MPU fault handler
	BusFault_Handler,						// The bus fault handler
	UsageFault_Handler,						// The usage fault handler
	0,										// Reserved
	0,										// Reserved
	0,										// Reserved
	0,										// Reserved
	SVCall_Handler,							// SVCall handler
	DebugMon_Handler,						// Debug monitor handler
	0,										// Reserved
	PendSV_Handler,							// The PendSV handler
	SysTick_Handler,						// The SysTick handler

	// Chip Level - Si3MU1xx
	WDTIMER0_IRQHandler,     // Watchdog Timer 0
	PBEXT0_IRQHandler,       // External interrupt 0
	PBEXT1_IRQHandler,       // External interrupt 1
	RTC0ALRM_IRQHandler,     // RTC Alarm
	DMACH0_IRQHandler,       // DMA Channel 0
	DMACH1_IRQHandler,       // DMA Channel 1
	DMACH2_IRQHandler,       // DMA Channel 2
	DMACH3_IRQHandler,       // DMA Channel 3
	DMACH4_IRQHandler,       // DMA Channel 4
	DMACH5_IRQHandler,       // DMA Channel 5
	DMACH6_IRQHandler,       // DMA Channel 6
	DMACH7_IRQHandler,       // DMA Channel 7
	DMACH8_IRQHandler,       // DMA Channel 8
	DMACH9_IRQHandler,       // DMA Channel 9
	DMACH10_IRQHandler,      // DMA Channel 10
	DMACH11_IRQHandler,      // DMA Channel 11
	DMACH12_IRQHandler,      // DMA Channel 12
	DMACH13_IRQHandler,      // DMA Channel 13
	DMACH14_IRQHandler,      // DMA Channel 14
	DMACH15_IRQHandler,      // DMA Channel 15
	TIMER0L_IRQHandler,      // Timer 0 Low (16 bits only)
	TIMER0H_IRQHandler,      // Timer 0 High (16 or 32 bits)
	TIMER1L_IRQHandler,      // Timer 1 Low (16 bits only)
	TIMER1H_IRQHandler,      // Timer 1 High (16 or 32 bits)
	EPCA0_IRQHandler,        // EPCA 0 Capture Compare (any channel)
	PCA0_IRQHandler,         // PCA 0 Capture Compare (any channel)
	PCA1_IRQHandler,         // PCA 1 Capture Compare (any channel)
	USART0_IRQHandler,       // USART 0
	USART1_IRQHandler,       // USART 1
	SPI0_IRQHandler,         // SPI 0
	SPI1_IRQHandler,         // SPI 1
	SPI2_IRQHandler,         // SPI 2
	I2C0_IRQHandler,         // I2C 0
	I2C1_IRQHandler,         // I2C 1
	USB0_IRQHandler,         // USB 0
	SARADC0_IRQHandler,      // ADC 0
	SARADC1_IRQHandler,      // ADC 1
	CMP0_IRQHandler,         // Comparator 0
	CMP1_IRQHandler,         // Comparator 1
	CAPSENSE0_IRQHandler,    // CapSense 0
	I2S0RX_IRQHandler,       // I2S 0 Receive
	I2S0TX_IRQHandler,       // I2S 0 Transmit
	AES0_IRQHandler,         // AES 0
	VDDLOW_IRQHandler,       // VDD low
	RTC0FAIL_IRQHandler,     // RTC failure
	PMATCH_IRQHandler,       // Port Match
	UART0_IRQHandler,        // UART 0
	UART1_IRQHandler,        // UART 1
	IDAC0_IRQHandler,        // IDAC 0
	IDAC1_IRQHandler,        // IDAC 1
	LPTIMER0_IRQHandler,     // Low-Power Timer
	PLL0_IRQHandler,         // Phase-Locked Loop
	VBUSINVALID_IRQHandler,  // VBUS Invalid
	VREG0LOW_IRQHandler,     // Voltage Regulator / 4
};

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int *pulSrc = (unsigned int*) romstart;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors")))
void bss_init(unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = 0;
}

//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

__attribute__ ((section(".after_vectors")))
void
ResetISR(void) {

    //
    // Copy the data sections from flash to SRAM.
    //
	unsigned int LoadAddr, ExeAddr, SectionLen;
	unsigned int *SectionTableAddr;

	// Load base address of Global Section Table
	SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
	while (SectionTableAddr < &__data_section_table_end) {
		LoadAddr = *SectionTableAddr++;
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
//		data_init(LoadAddr, ExeAddr, SectionLen);
	}
	// At this point, SectionTableAddr = &__bss_section_table;
	// Zero fill the bss segment
	while (SectionTableAddr < &__bss_section_table_end) {
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		bss_init(ExeAddr, SectionLen);
	}

#ifdef __USE_CMSIS
	SystemInit();
#endif

#if defined (__cplusplus)
	//
	// Call C++ library initialisation
	//
	__libc_init_array();
#endif

#if defined (__REDLIB__)
	// Call the Redlib library, which in turn calls main()
	__main() ;
#else
	main();
#endif

	//
	// main() shouldn't return, but if it does, we'll just enter an infinite loop
	//
	while (1) {
		;
	}
}

//*****************************************************************************
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void NMI_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void HardFault_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void MemManage_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void BusFault_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void UsageFault_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void SVCall_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void DebugMon_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void PendSV_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void SysTick_Handler(void)
{
    while(1)
    {
    }
}

//*****************************************************************************
//
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
    while(1)
    {
    }
}
