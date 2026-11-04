OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
	/* ARM PA 0x4a000-0x61fff; BL31 occupies 0x45000-0x48fff and 0x62000+. */
	sram (rwx) : ORIGIN = 0x4000a000, LENGTH = 92K
}

SECTIONS
{
	. = ORIGIN(sram);

	.text : {
		KEEP(*(.text.init))
		*(.text .text.*)
		*(.rodata .rodata.*)
		*(.srodata .srodata.*)
	} > sram

	. = ALIGN(8);
	.resource_table : {
		KEEP(*(.resource_table))
	} > sram

	. = ALIGN(8);
	_edata_rom = .;

	.data : {
		_data_start = .;
		*(.data .data.*)
		*(.sdata .sdata.*)
		_data_end = .;
	} > sram

	. = ALIGN(8);
	.bss (NOLOAD) : {
		_bss_start = .;
		*(.bss .bss.*)
		*(.sbss .sbss.*)
		*(COMMON)
		_bss_end = .;
	} > sram

	. = ALIGN(16);
	_heap_start = .;
	_heap_end = ORIGIN(sram) + LENGTH(sram) - 4K;

	ASSERT(_bss_end + 4K <= ORIGIN(sram) + LENGTH(sram), "SRAM stack overlaps BSS")
	_stack_top = ORIGIN(sram) + LENGTH(sram);
	__global_pointer$ = _data_start + 0x800;
}
