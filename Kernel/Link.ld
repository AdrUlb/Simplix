OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(Start)

KERNEL_VMA = 0xFFFFFFFF80000000;

PHDRS
{
	text PT_LOAD;
	rodata PT_LOAD;
	data PT_LOAD;
}

SECTIONS
{
	. += KERNEL_VMA;

	. = ALIGN(CONSTANT(MAXPAGESIZE));
	KERNEL_SECT_TEXT_START = ABSOLUTE(.);

	.text :
	{
		*(.text .text.*)
	} : text

	. = ALIGN(CONSTANT(MAXPAGESIZE));
	KERNEL_SECT_TEXT_END = ABSOLUTE(.);
	KERNEL_SECT_RODATA_START = ABSOLUTE(.);

	.rodata :
	{
		*(.rodata .rodata.*)
	} : rodata

	.eh_frame :
	{
		*(.eh_frame)
	} : rodata

	.init_array :
	{
		INIT_ARRAY_START = ABSOLUTE(.);
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
		KEEP(*(.init_array))
		INIT_ARRAY_END = ABSOLUTE(.);
	} : rodata

	.fini_array :
	{
		FINI_ARRAY_START = ABSOLUTE(.);
		KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
		KEEP(*(.fini_array))
		FINI_ARRAY_END = ABSOLUTE(.);
	} : rodata

	.note.gnu.build-id :
	{
		*(.note.gnu.build-id)
	} : rodata

	. = ALIGN(CONSTANT(MAXPAGESIZE));
	KERNEL_SECT_RODATA_END = ABSOLUTE(.);
	KERNEL_SECT_DATA_START = ABSOLUTE(.);

	.data :
	{
		*(.data .data.*)
	} : data

	.limine_requests :
	{
		KEEP(*(.limine_requests_start))
		KEEP(*(.limine_requests))
		KEEP(*(.limine_requests_end))
	} : data

	. = ALIGN(CONSTANT(MAXPAGESIZE));
	KERNEL_SECT_DATA_END = ABSOLUTE(.);
	KERNEL_SECT_BSS_START = ABSOLUTE(.);

	.bss :
	{
		*(.bss .bss.*)
		*(COMMON)
	} : data
	. = ALIGN(CONSTANT(MAXPAGESIZE));
	KERNEL_SECT_BSS_END = ABSOLUTE(.);

	/DISCARD/ :
	{
		*(.note .note.*)
	}
}