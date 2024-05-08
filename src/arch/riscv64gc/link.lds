OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  ram  (wxa) : ORIGIN = 0x84000000, LENGTH = 10M
}

PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
  bss PT_LOAD;
}


SECTIONS
{
	. = 0x84000000;

	.text.low : {
		. = ALIGN(4);
		*(.text.low)
		*(.text.clss)
		*(.text.low.*)
	}>ram AT>ram :text

	PROVIDE(_lglobal_pointer = .);

	.rodata.low : {
		. = ALIGN(4);
		*(.rodata.low) *(.rodata.low.*)
	}>ram AT>ram :text

	.data.low : {
		*(.data.low) *(.data.low.*)
		*(.sdata.low) *(.sdata.low.*)
	}>ram AT>ram :data

	.bss.low : {
		PROVIDE(_lbss_begin = .);
		*(.bss.low) *(.bss.low.*)
		*(.sbss.low) *(.sbss.low.*)
		PROVIDE(_lbss_end = .);
	}>ram AT>ram :bss

	. = ALIGN(16);

	PROVIDE(_lstack_begin = .);
	PROVIDE(_lstack_end = _lstack_begin + 0x80000);

	. += 0xFFFFFFFFBFFFFFFF;
	/* Add a symbol that indicates the start address of the kernel. */
	.text ALIGN (4K) : AT (ADDR (.text) - 0xFFFFFFFFBFFFFFFF)
	{
		PROVIDE(_kernel_begin = .);
  		PROVIDE(_text_begin = .);
  	  	*(.text.init) *(.text .text.*)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xFFFFFFFFBFFFFFFF)
	{
		PROVIDE(_text_end = .);
		PROVIDE(_global_pointer = .);
		PROVIDE(_rodata_begin = .);
    	PROVIDE(_driverinfo_begin = .);
		*(.driverinfo .driverinfo.*)
		PROVIDE(_driverinfo_end = .);
    	*(.rodata .rodata.*)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - 0xFFFFFFFFBFFFFFFF)
	{
		PROVIDE(_rodata_end = .);
    	PROVIDE(_data_begin = .);
    	*(.sdata .sdata.*) *(.data .data.*)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xFFFFFFFFBFFFFFFF)
	{
		PROVIDE(_data_end = .);
    	PROVIDE(_bss_begin = .);
    	*(.sbss .sbss.*) *(.bss .bss.*)
		. = ALIGN (4K);
		PROVIDE(_bss_end = .);
	}
	/* Add a symbol that indicates the end address of the kernel. */
	PROVIDE(_stack_begin = _bss_end);
  	PROVIDE(_stack_end = _stack_begin + 0x80000);
  	PROVIDE(_kernel_end = .);
}

