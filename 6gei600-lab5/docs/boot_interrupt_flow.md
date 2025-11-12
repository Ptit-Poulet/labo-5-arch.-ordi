# Boot and Interrupt Flow

This project runs with no operating system (bare‑metal). The Raspberry Pi firmware loads `kernel8.img` into memory and jumps to `_start`.

## 1) Reset entry: `_start` (AArch64)
Key steps performed in `asm/start.S`:

- Stack is set to `_stack_top` from `linker.ld`.
- `.bss` is zero‑initialized.
- Exception level is checked; if at EL2, system drops to EL1 with proper configuration.
- Vector Base Address Register (`VBAR_EL1`) is set to our vector table symbol `vectors`.
- IRQs are enabled (`DAIFClr, #2`).
- C entry point `main()` is called.

### Exception Level Handling

When running on QEMU (or real hardware), the system typically starts at EL2 (Hypervisor mode). The startup code performs the following:

1. **Check current exception level**: Read `CurrentEL` register to determine the current EL.
2. **If at EL2, configure and drop to EL1**:
   - Set up `SP_EL1` with stack pointer for EL1.
   - Configure `HCR_EL2` to ensure EL1 runs in AArch64 mode.
   - Set `SPSR_EL2` to 0x5 (EL1h mode with all interrupts unmasked).
   - Use `eret` to return to EL1 at the `el1_entry` label.
3. **At EL1**: Set up the vector table and unmask IRQs.

This ensures that interrupts are properly routed to EL1 and can be handled by our interrupt handlers.

## 2) Vector table and IRQ entry
The vector table is defined in `asm/vectors.S`. For IRQs at EL1, control flows to `irq_el1h` which saves registers, calls `irq_handler` in C, then returns with `eret`.

### Vector Table Alignment

The ARM AArch64 architecture requires the exception vector table to be aligned to 2048 bytes (`.align 11`), and each individual vector entry must be aligned to 128 bytes (`.align 7`). The vector table contains 16 entries total, organized in 4 groups of 4 vectors each:

1. **Current EL with SP_EL0**: Synchronous, IRQ, FIQ, SError
2. **Current EL with SP_ELx** (x > 0): Synchronous, IRQ, FIQ, SError
3. **Lower EL (AArch64)**: Synchronous, IRQ, FIQ, SError
4. **Lower EL (AArch32)**: Synchronous, IRQ, FIQ, SError

Since we run at EL1 using SP_EL1, IRQs arrive at the second group's IRQ entry, which branches to `irq_el1h`. The `.align 7` directives between entries are **critical** - without them, the CPU won't correctly dispatch to the handlers.

## 3) C initialization and main loop
`main()` initializes peripherals and then waits for interrupts:

- `uart_init()` initializes UART for debug output.
- `gpio_init()` prepares the LED GPIO as output and turns it off.
- `interrupts_init()` enables the system timer match‑1 interrupt line in the interrupt controller.
- `timer_init()` arms the first timer compare for a future tick.
- The CPU executes `wfi` (wait‑for‑interrupt) in a loop to save power.

## 4) Timer IRQ handling path
When the system timer reaches the compare value, an interrupt is raised:

- Hardware vectors to `irq_el1h` → calls `irq_handler()`.
- In `irq_handler()` we:
  - Verify the pending source is system timer match‑1.
  - Acknowledge/clear the timer match in `TIMER_CS`.
  - Update our tick counter and toggle the LED every 100 ticks (1 second at 10ms per tick).
  - Schedule the next tick by writing a new compare value.

The timer uses a 10ms interval (10,000 microseconds), and after 100 ticks (1 second), the LED state toggles.

That's the full loop: boot → check EL → drop to EL1 if needed → set vectors → init → sleep → timer IRQ → handler → schedule next.
