# Troubleshooting

Quick checks if the LED does not toggle as expected.

## Nothing happens at power on
- SD card missing Pi firmware files (`bootcode.bin`, `start.elf`, etc.)
- Wrong kernel name in `config.txt`
  - Must include:
    ```
    arm_64bit=1
    kernel=kernel8.img
    ```
- `kernel8.img` not on the boot partition
- Using wrong Pi model (addresses assume Raspberry Pi 3B)

## Builds but no LED activity

### Common Issues
- Wiring mismatch: Confirm LED is on GPIO 17 (pin 11) with proper resistor and ground
- `gpio_init()` must run before interrupts; check call order in `main`
- Compare interval too long/short: `tick_interval_us` in `src/timer.c`
- Ensure interrupts are enabled in `interrupts_init()` and that `timer_init()` sets the first compare

### Interrupt-Specific Issues

**If interrupts never fire:**

1. **Vector table alignment**: The exception vector table requires proper alignment. Each vector entry must be at a 128-byte boundary (`.align 7`). Missing these directives will cause the CPU to jump to incorrect addresses when an interrupt occurs.

2. **Exception level configuration**: On QEMU and most hardware, the system boots at EL2 (Hypervisor mode). Interrupts must be properly routed to EL1:
   - Check that the startup code detects the current exception level
   - Ensure `HCR_EL2` is configured to run EL1 in AArch64 mode
   - Verify `SPSR_EL2` is set correctly (0x5 for EL1h with interrupts unmasked)
   - Confirm the code uses `eret` to drop from EL2 to EL1

3. **IRQ masking**: Even after setting up the vector table, IRQs must be explicitly unmasked using `msr DAIFClr, #2`

## Interrupt fires only once
- In `irq_handler()`, the timer match flag must be cleared:
  - `TIMER_CS = CS_M1;` must execute before scheduling next compare

## Porting to a different Pi SoC
- Update `PERIPH_BASE` addresses in `gpio.c`, `timer.c`, `interrupts.c`
- On Pi 4, base is typically `0xFE000000` (not covered here)

## UART debugging
This project includes UART debugging support for development in QEMU. See [uart_debugging.md](uart_debugging.md) for details.

When running in QEMU with `-serial stdio`, you'll see debug output like:
```
[GPIO] Initializing pin 17
[GPIO] Set pin 17 as OUTPUT
[GPIO] Pin 17 -> LOW
[GPIO] Pin 17 -> HIGH
[GPIO] Pin 17 -> LOW
...
```

This output confirms that:
- Initialization is completing successfully
- Timer interrupts are firing
- GPIO state changes are occurring

