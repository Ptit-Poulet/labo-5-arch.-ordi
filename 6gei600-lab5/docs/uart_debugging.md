# UART Debugging Guide

## Overview

This project includes a UART (Universal Asynchronous Receiver/Transmitter) serial communication module for debugging purposes. UART allows the bare-metal kernel to send text output to your console through the serial interface.

## Why UART for Debugging?

When running bare-metal code on QEMU's Raspberry Pi 3B emulation, you face a fundamental problem: **QEMU does not fully emulate GPIO peripherals**. This means:

- GPIO register writes succeed but don't produce visible effects
- You cannot observe LED state changes in the emulator
- Hardware-specific features like pull-up/pull-down resistors are not simulated

UART solves this by providing a **reliable communication channel** that QEMU fully supports. When you connect QEMU's serial output to your terminal (via `-serial stdio`), any text sent through UART appears directly in your console.

## How It Works

### Hardware Setup

The Raspberry Pi 3B has a PL011 UART controller (UART0) mapped to:
- **Base address**: `0x3F201000`
- **TX pin**: GPIO 14 (transmit)
- **RX pin**: GPIO 15 (receive)

### Initialization Sequence

1. Disable UART0
2. Configure GPIO 14 and 15 for ALT0 function (UART mode)
3. Disable pull-up/pull-down resistors on UART pins
4. Set baud rate to 115200 (standard serial speed)
5. Configure 8-bit data, no parity, 1 stop bit
6. Enable UART transmit and receive

### Key Registers

- `UART0_DR` (Data Register): Write characters here to transmit
- `UART0_FR` (Flag Register): Check bit 5 (TXFF) to see if transmit FIFO is full
- `UART0_CR` (Control Register): Enable/disable UART and its features
- `UART0_IBRD/FBRD` (Baud Rate Divisors): Set communication speed

## Usage in Code

### Basic Output

```c
#include "uart.h"

// Print a simple message
uart_puts("Hello from bare-metal!\n");

// Print numbers
uart_puts("Tick count: ");
uart_put_number(tick_count);
uart_puts("\n");

// Print hexadecimal values
uart_puts("Register value: ");
uart_put_hex(0xDEADBEEF);
uart_puts("\n");
```

### GPIO Debugging

The UART module includes specialized GPIO debug functions:

```c
// Automatically called by gpio.c functions:
uart_debug_gpio_init(pin);         // "[GPIO] Initializing pin 17"
uart_debug_gpio_set_output(pin);   // "[GPIO] Set pin 17 as OUTPUT"
uart_debug_gpio_write(pin, true);  // "[GPIO] Pin 17 -> HIGH"
```

These are already integrated into `src/gpio.c`, so GPIO state changes automatically print to the console.

### Expected Console Output

When you run the kernel in QEMU, you should see:

```
[GPIO] Initializing pin 17
[GPIO] Set pin 17 as OUTPUT
[GPIO] Pin 17 -> LOW
[GPIO] Pin 17 -> HIGH
[GPIO] Pin 17 -> LOW
...
```

Each line appears as your interrupt handler toggles the LED state.

## QEMU Serial Connection

Your QEMU command already includes the correct flags:

```bash
qemu-system-aarch64 ... -serial stdio -nographic
```

- `-serial stdio`: Connects UART0 to standard input/output
- `-nographic`: Disables graphical window, shows only serial console

## Debugging Tips

### Add Custom Debug Points

Insert debug statements anywhere in your code:

```c
void timer_schedule_next(void) {
    uint32_t now = TIMER_CLO;
    uart_puts("[TIMER] Scheduling next at: ");
    uart_put_hex(now + tick_interval_us);
    uart_putc('\n');

    TIMER_C1 = now + tick_interval_us;
}
```

### Track Interrupt Flow

```c
void irq_handler(void) {
    uart_puts("[IRQ] Interrupt triggered\n");

    if (INTERRUPT_PENDING_1 & (1u << 1u)) {
        uart_puts("[IRQ] Timer interrupt confirmed\n");
        // ... handle interrupt
    }
}
```

### Debug Memory Reads

```c
uint32_t value = *(volatile uint32_t*)0x3F200000;
uart_puts("GPIO register: ");
uart_put_hex(value);
uart_putc('\n');
```

## Performance Considerations

UART is relatively slow (115200 baud ≈ 11,520 bytes/second). For high-frequency operations:

- Avoid printing every single interrupt tick
- Use counters and print periodically (e.g., every 100 ticks)
- Disable verbose debug output after initial testing

## Real Hardware vs. QEMU

On actual Raspberry Pi 3B hardware:
- UART output appears on the serial console (if connected)
- You can also see real GPIO LED changes
- UART remains useful for detailed timing and state information

In QEMU:
- UART is your primary feedback mechanism
- GPIO state is invisible without UART debug output
- Consider testing on real hardware for final verification

## API Reference

### Core Functions

| Function | Purpose | Example |
|----------|---------|---------|
| `uart_init()` | Initialize UART0 at 115200 baud | Called once in `main()` |
| `uart_putc(char)` | Output single character | `uart_putc('A')` |
| `uart_puts(str)` | Output null-terminated string | `uart_puts("Debug\n")` |
| `uart_put_number(int)` | Output decimal integer | `uart_put_number(42)` |
| `uart_put_hex(uint32_t)` | Output hexadecimal with 0x prefix | `uart_put_hex(0xFF)` |

### GPIO Debug Helpers

| Function | Output Format |
|----------|---------------|
| `uart_debug_gpio_init(pin)` | `[GPIO] Initializing pin N` |
| `uart_debug_gpio_set_output(pin)` | `[GPIO] Set pin N as OUTPUT` |
| `uart_debug_gpio_write(pin, state)` | `[GPIO] Pin N -> HIGH/LOW` |

## Implementation Files

- **Header**: `include/uart.h`
- **Implementation**: `src/uart.c`
- **Integration**: `src/gpio.c` (automatic GPIO debug calls)
- **Initialization**: `src/main.c` (calls `uart_init()` first)

## Further Reading

- [BCM2837 ARM Peripherals Manual](https://datasheets.raspberrypi.com/bcm2835/bcm2835-peripherals.pdf) - Section 13 (UART)
- [PL011 Technical Reference Manual](https://developer.arm.com/documentation/ddi0183/latest/) - Full UART controller specification
