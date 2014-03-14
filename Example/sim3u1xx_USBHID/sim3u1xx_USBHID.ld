GROUP(
 libcr_semihost.a
 libcr_c.a
 libcr_eabihelpers.a
)
MEMORY
{
  /* Define each memory region */
  MFlash256 (rx) : ORIGIN = 0x0, LENGTH = 0x3fffc /* 255k */
  Ram32 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32k */
}
  /* Define a symbol for the top of each memory region */
  __top_MFlash256 = 0x0 + 0x3fffc;
  __top_Ram32 = 0x20000000 + 0x8000;

/*
 * GENERATED FILE - DO NOT EDIT
 * (C) Code Red Technologies Ltd, 2008-2014
 * Generated linker script file for SiM3U167
 * Created from generic_c.ld (vPrecision32 IDE v4.2 (1 [Build 73] [08/03/2012] ))
 * By Precision32 IDE v4.2.1 [Build 73] [08/03/2012]  on Fri Mar 07 13:52:39 CST 2014
 */


ENTRY(ResetISR)

SECTIONS
{

  /* MAIN TEXT SECTION */
  .text : ALIGN(4)
  {
    FILL(0xff)
    KEEP(*(.isr_vector))

    /* Global Section Table */
    . = ALIGN(4) ;
    __section_table_start = .;
    __data_section_table = .;
    LONG(LOADADDR(.data));
    LONG(    ADDR(.data)) ;
    LONG(  SIZEOF(.data));
    __data_section_table_end = .;
    __bss_section_table = .;
    LONG(    ADDR(.bss));
    LONG(  SIZEOF(.bss));
    __bss_section_table_end = .;
    __section_table_end = . ;
    /* End of Global Section Table */


    *(.after_vectors*)

    *(.text*)
    *(.rodata .rodata.*)
    . = ALIGN(4);

  } > Ram32

  /*
   * for exception handling/unwind - some Newlib functions (in common
   * with C++ and STDC++) use this.
   */
  .ARM.extab : ALIGN(4)
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > Ram32
  __exidx_start = .;

  .ARM.exidx : ALIGN(4)
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > Ram32
  __exidx_end = .;

  _etext = .;


  /* MAIN DATA SECTION */

  .uninit_RESERVED : ALIGN(4)
  {
    KEEP(*(.bss.$RESERVED*))
  } > Ram32

  .data : ALIGN(4)
  {
    FILL(0xff)
    _data = .;
    *(vtable)
    *(.data*)
    . = ALIGN(4) ;
    _edata = .;
  } > Ram32 AT>Ram32


  /* MAIN BSS SECTION */
  .bss : ALIGN(4)
  {
    _bss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4) ;
    _ebss = .;
    PROVIDE(end = .);
  } > Ram32

  PROVIDE(_pvHeapStart = .);
  PROVIDE(_vStackTop = __top_Ram32 - 0);
}
