# Memory Layout and Linker Script

The linker script places code and data at addresses expected by the Raspberry Pi firmware and our startup code.

## Load address and sections

```1:30:./Lab5/linker.ld
ENTRY(_start)

SECTIONS
{
    . = 0x80000;

    .text : {
        KEEP(*(.text.boot))
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    _stack_top = . + 0x10000;
}
```

- Image base at `0x80000` (where Pi firmware loads 64‑bit kernels by default)
- `.text.boot` is kept first to ensure `_start` is at the beginning of `.text`
- `.bss` boundaries (`__bss_start`, `__bss_end`) are exported so `start.S` can zero them
- `_stack_top` is exported and used by `start.S` to set `sp`

## Stack
- The stack grows down from `_stack_top`
- Size is `0x10000` bytes by convention here; adjust if your program needs more stack

## Peripherals
- This code assumes Raspberry Pi 3 peripheral base `0x3F000000`
- System Timer at `0x3F003000` and GPIO at `0x3F200000` are memory‑mapped

## Why zero `.bss`?
- C expects static/global variables without initializers to start at zero
- `start.S` clears `.bss` before calling into C to maintain this guarantee

