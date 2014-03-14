GROUP(libcr_nohost.a libcr_c.a libcr_eabihelpers.a)
MEMORY
{
  /* Define each memory region */
  MFlash256 (rx) : ORIGIN = 0x0, LENGTH = 0x3fffc /* 255k */
  StandardRam28 (rwx) : ORIGIN = 0x20001000, LENGTH = 0x7000 /* 28k */
  RetentionRam4 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x1000 /* 4k */

}
  /* Define a symbol for the top of each memory region */
  __top_MFlash256 = 0x0 + 0x3fffc;
  __top_StandardRam28 = 0x20001000 + 0x7000;
  __top_RetentionRam4 = 0x20000000 + 0x1000;

ENTRY(ResetISR)
SECTIONS
{
    .text 0x20000000 :  AT (0x00000000)
    {
    	. = ALIGN(4);
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    }

    .data 0x20000000 + SIZEOF(.text) : AT (LOADADDR(.text) + SIZEOF(.text))
    {
        _data = .;
        *(.data*)
        _edata = .;
    }

    .bss 0x20000000 + SIZEOF(.text) + SIZEOF(.data) : AT (LOADADDR(.data) + SIZEOF(.data))
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    }
    
	PROVIDE(_pvHeapStart = .);
	PROVIDE(_vStackTop = __top_StandardRam28 - 0);
}
