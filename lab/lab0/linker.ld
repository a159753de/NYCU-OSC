SECTIONS
{
    . = 0x00200000;
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.sbss) *(.bss) }
}