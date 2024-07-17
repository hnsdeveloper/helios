OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  ram  (wxa) : ORIGIN = RISCV_MEM_ORIGIN, LENGTH = 1024M
}

/*
PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
  bss PT_LOAD;
}
*/

SECTIONS
{
	PROVIDE(_load_address = RISCV_MEM_ORIGIN);

	.text.low : {
		. = ALIGN(4);
        *(.text.start)
		*(.text.low)
		*(.text.clss)
		*(.text.low.*)
	}

	PROVIDE(_lglobal_pointer = .);

	.rodata.low : {
		. = ALIGN(4);
		*(.rodata.low) *(.rodata.low.*)
	}

	.data.low : {
		*(.data.low) *(.data.low.*)
		*(.sdata.low) *(.sdata.low.*)
	}

	. = ALIGN(4K);

	.bss.low (NOLOAD) : {
		PROVIDE(_lbss_begin = .);
		*(.bss.low) *(.bss.low.*)
		*(.sbss.low) *(.sbss.low.*)
		PROVIDE(_lbss_end = .);
	}

	. = ALIGN(16);
	PROVIDE(_bootstack_size = 0x1000);
	PROVIDE(_lstack_begin = .);
	PROVIDE(_lstack_end = _lstack_begin + _bootstack_size);

	/* With this symbol we know where to start mapping the kernel */
	PROVIDE(_kload_begin = _lstack_end);
	/* Add a symbol that indicates the start address of the kernel. */
	.text  0xFFFFFFFFC0000000 : AT(_lstack_end)
	{
		PROVIDE(_kernel_begin = .);
  		PROVIDE(_text_begin = .);
  	  	*(.text.init) *(.text .text.*)
	}

	.rodata ALIGN (4K) : AT(LOADADDR(.text) + SIZEOF(.text) + (4096 - (SIZEOF(.text) % 4096)))
	{
		PROVIDE(_text_end = .);
		PROVIDE(_global_pointer = .);
		PROVIDE(_rodata_begin = .);
    	PROVIDE(_driverinfo_begin = .);
		*(.driverinfo .driverinfo.*)
		PROVIDE(_driverinfo_end = .);
    	*(.rodata)
		*(.rodata.*)
	}

	.data ALIGN (4K) : AT(LOADADDR(.rodata) + SIZEOF(.rodata) + (4096 - (SIZEOF(.rodata) % 4096)))
	{
		PROVIDE(_rodata_end = .);
    	PROVIDE(_data_begin = .);
    	*(.sdata .sdata.*) *(.data .data.*)
	}
	
	.bss ALIGN (4K) (NOLOAD) : AT(LOADADDR(.data) + SIZEOF(.data) + (4096 - (SIZEOF(.data) % 4096)))
	{
		PROVIDE(_data_end = .);
    	PROVIDE(_bss_begin = .);
    	*(.sbss)
		*(.sbss.*) 
		*(.bss)
		*(.bss.*)
		. = ALIGN (4K);
		PROVIDE(_bss_end = .);
	}

	/* With this symbol we know where to stop mapping, and we also get to know the size of the kernel */
	PROVIDE(_kload_end = LOADADDR(.bss) + SIZEOF(.bss) + (4096 - SIZEOF(.bss) % 4096));
	PROVIDE(_stack_begin = _bss_end);
  	PROVIDE(_stack_end = _stack_begin + 0x80000);
  	PROVIDE(_kernel_end = .);
	PROVIDE(_kphysical_end = LOADADDR(.bss) + SIZEOF(.bss) + (4096 - SIZEOF(.bss) % 4096) + 0x80000);

	
}

