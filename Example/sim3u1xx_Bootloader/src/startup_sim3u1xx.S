#include "config.h"
    .syntax unified
    .thumb

//*****************************************************************************
//
// The vector table.
// This relies on the linker script to place at correct location in memory.
//
//*****************************************************************************
    .section .isr_vector
Vectors:
    .word   0x20008000                      // The initial stack pointer
    .word   ResetISR //- 0x20000000           // The reset handler
    .word   NMI_Handler                     // The NMI handler
    .word   HardFault_Handler               // The hard fault handler
    .word   Default_Handler           	    // The MPU fault handler
    .word   Default_Handler          	    // The bus fault handler
    .word   Default_Handler         	    // The usage fault handler
    .word   0                               // Reserved
    .word   0                               // Reserved
    .word   0                               // Reserved
    .word   0                               // Reserved
    .word   Default_Handler                 // SVCall handler
    .word   Default_Handler                 // Debug monitor handler
    .word   0                               // Reserved
    .word   Default_Handler                 // The PendSV handler
    .extern SysTick_Handler
    .word   SysTick_Handler                 // The SysTick handler

    .text
    .thumb_func
CopyCode2SRAM:
    // Copy the text and data sections from flash to SRAM.
    movs    r0, #0x00000000
    ldr     r1, =0x20000000
    .extern _bss
    ldr     r2, =_bss
copy_loop:
    ldr     r3, [r0], #4
    str     r3, [r1], #4
    cmp     r1, r2
    blt     copy_loop

    // Zero fill the bss segment
    movs    r0, #0x00000000
    .extern _ebss
    ldr     r2, =_ebss
zero_loop:
    str     r0, [r1], #4
    cmp     r1, r2
    blt     zero_loop

    // Set the vector table pointer to SRAM.
    ldr     r0, =0xe000ed08
    ldr     r1, =0x20000000
    str     r1, [r0]
    // set return address to SRAM and return
    orr     lr, lr, #0x20000000
    bx      lr

//*****************************************************************************
//
// The reset handler, which gets called when the processor starts.
//
//*****************************************************************************
    .globl  ResetISR
    .thumb_func
ResetISR:
	// Copy code from flash to SRAM
//    bl      CopyCode2SRAM
    .extern SystemInit
    bl SystemInit

    .extern main
    bl main

    .thumb_func
RunUserCode:
    // Set the vector table address to user code address.
    ldr     r0, =USER_CODE_ADDRESS
    ldr     r1, =0xe000ed08
    str     r0, [r1]

    // Update stack pointer from user code vector table
    ldr     r1, [r0]
    mov     sp, r1

    // Load user code reset handler and jump to the user code
    ldr     r0, [r0, #4]
    bx      r0

    .thumb_func
NMI_Handler:
    b       .

    .thumb_func
HardFault_Handler:
    b       .

    .thumb_func
Default_Handler:
    b       .

    .end
