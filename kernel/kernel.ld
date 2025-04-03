/* The entry point of the kernel is the boot function. */
ENTRY(boot)

SECTIONS {
    /**
    * Since OpenSBI simply jumps to 0x80200000 without knowing the entry point, the boot function needs to be placed at 0x80200000.
    */
    . = 0x80200000;

    /* This section contains the code of the program. */
    .text :{
        /* The .text.boot section is always placed at the beginning. */
        KEEP(*(.text.boot));
        *(.text .text.*);
    }

    /* This section contains constant data that is read-only. */
    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    /* This section contains read/write data. */
    .data : ALIGN(4) {
        *(.data .data.*);
    }

    /* This section contains read/write data with an initial value of zero. */
    .bss : ALIGN(4) {
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        __bss_end = .;
    }

    /* The kernel stack comes after the .bss section, and its size is 128KB. */
    . = ALIGN(4);
    . += 128 * 1024; /* 128KB */
    __stack_top = .;

    /* ram area is 4KB = 4096 = 0x1000 (hexadecimal) aligned. Thus, page-aligned addresses look nicely aligned in hexadecimal. */
        . = ALIGN(4096);
    __free_ram = .;
    . += 64 * 1024 * 1024; /* 64MB */
    __free_ram_end = .;
}