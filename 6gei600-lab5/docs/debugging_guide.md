# Debugging Guide for Bare-Metal Programming

This guide explains how to debug your bare-metal Raspberry Pi program, especially when things don't work as expected.

## Why Debugging is Different on Bare-Metal

When you write regular programs on your computer, you can use `printf()` to see what's happening. But on bare-metal (no operating system), there's no screen, no console, and no debugger by default. This makes finding bugs much harder!

**The challenge**: Your LED doesn't blink. Is the problem in:
- The hardware setup?
- The GPIO initialization?
- The timer configuration?
- The interrupt handling?
- Something else entirely?

Without any output, you have no idea where the problem is.

## Solution: UART Debugging

UART (Universal Asynchronous Receiver/Transmitter) is a simple serial communication protocol that lets your Pi "talk" to your computer. When running in QEMU (the emulator), UART output appears directly in your terminal. This is **incredibly useful** for debugging.

### What UART Debugging Looks Like

When you run the program in QEMU, you see messages like:
```
[GPIO] Initializing pin 17
[GPIO] Set pin 17 as OUTPUT
[GPIO] Pin 17 -> LOW
[GPIO] Pin 17 -> HIGH
[GPIO] Pin 17 -> LOW
```

Each message tells you exactly what the program is doing at that moment.

## When to Add UART Debug Messages

### Rule of Thumb
Add UART debug messages whenever you:
1. Initialize hardware (GPIO, timer, interrupts)
2. Enter a critical function (interrupt handlers, init functions)
3. Change hardware state (LED on/off, timer configuration)
4. Hit a point where things might fail

### Examples

#### ✅ Good: Adding Debug at Initialization
```c
void gpio_init(void) {
    uart_puts("[GPIO] Starting initialization\n");

    // Configure GPIO 17
    uint32_t ra = GPFSEL1;
    ra &= ~(7 << 21);
    ra |= (1 << 21);
    GPFSEL1 = ra;

    uart_puts("[GPIO] Pin 17 configured as OUTPUT\n");

    // Turn LED off initially
    GPIO_CLR0 = (1 << 17);

    uart_puts("[GPIO] Initialization complete\n");
}
```

**Why this helps**: If you see "Starting initialization" but not "Initialization complete", you know the function crashed somewhere in the middle.

#### ✅ Good: Adding Debug in Interrupt Handler
```c
void irq_handler(void) {
    uart_puts("[IRQ] Interrupt received\n");

    if (IRQ_PENDING1 & SYS_TIMER_MATCH1_IRQ) {
        uart_puts("[IRQ] Timer interrupt confirmed\n");

        // Handle the interrupt...
    } else {
        uart_puts("[IRQ] Unknown interrupt source!\n");
    }
}
```

**Why this helps**:
- If you never see "[IRQ] Interrupt received", interrupts aren't firing at all
- If you see "Unknown interrupt source", the wrong interrupt is triggering
- If you see messages appearing regularly, interrupts are working!

#### ✅ Good: Adding Debug for State Changes
```c
void gpio_set_red(bool state) {
    if (state) {
        GPIO_SET0 = (1 << 17);
        uart_puts("[GPIO] LED turned ON\n");
    } else {
        GPIO_CLR0 = (1 << 17);
        uart_puts("[GPIO] LED turned OFF\n");
    }
}
```

**Why this helps**: You can verify the LED state is changing in software, even if the physical LED isn't connected or broken.

#### ❌ Bad: Too Much Debug Output
```c
void irq_handler(void) {
    uart_puts("[IRQ] Start\n");

    if (IRQ_PENDING1 & SYS_TIMER_MATCH1_IRQ) {
        uart_puts("[IRQ] Timer check\n");
        if (TIMER_CS & CS_M1) {
            uart_puts("[IRQ] CS_M1 set\n");
            TIMER_CS = CS_M1;
            uart_puts("[IRQ] CS cleared\n");
            tick_count++;
            uart_puts("[IRQ] Tick: ");
            uart_put_number(tick_count);
            uart_puts("\n");
            // ... more messages for every single operation
        }
    }
}
```

**Why this is bad**:
- With a 10ms timer, this creates 100 messages per second!
- The terminal becomes unreadable
- UART transmission takes time and might interfere with timing

#### ✅ Better: Strategic Debug Output
```c
void irq_handler(void) {
    if (IRQ_PENDING1 & SYS_TIMER_MATCH1_IRQ) {
        if (TIMER_CS & CS_M1) {
            TIMER_CS = CS_M1;
            tick_count++;

            // Only print every 100 ticks (once per second)
            if (tick_count >= 100u) {
                tick_count = 0;
                led_state = !led_state;
                gpio_set_red(led_state);  // This function prints the LED state
            }

            timer_schedule_next();
        }
    }
}
```

## Debugging Common Problems

### Problem: Nothing appears on screen

**What to check**:
1. Did you call `uart_init()` first?
2. Are you running in QEMU with the `-serial stdio` flag?
3. Is UART configured correctly (pins, baud rate)?

**Quick test**: Add this as the very first line in `main()`:
```c
int main(void) {
    uart_init();
    uart_puts("=== PROGRAM STARTED ===\n");
    // ... rest of your code
}
```

If you don't see "PROGRAM STARTED", UART itself isn't working.

### Problem: Program hangs after initialization

**Add debug before and after suspicious code**:
```c
void timer_init(void) {
    uart_puts("[TIMER] Reading current time\n");
    uint32_t now = TIMER_CLO;

    uart_puts("[TIMER] Setting compare value\n");
    TIMER_C1 = now + tick_interval_us;

    uart_puts("[TIMER] Clearing status\n");
    TIMER_CS = CS_M1;

    uart_puts("[TIMER] Init complete\n");
}
```

If you see messages stopping at "Reading current time", the problem is reading `TIMER_CLO`.

### Problem: Interrupts never fire

**Add debug in multiple places**:

```c
// In main():
int main(void) {
    uart_init();
    uart_puts("[MAIN] Starting initialization\n");

    gpio_init();
    uart_puts("[MAIN] GPIO initialized\n");

    interrupts_init();
    uart_puts("[MAIN] Interrupts enabled\n");

    timer_init();
    uart_puts("[MAIN] Timer initialized\n");

    uart_puts("[MAIN] Entering main loop\n");
    for (;;) {
        __asm__ volatile ("wfi");
    }
}

// In irq_handler():
void irq_handler(void) {
    uart_puts("[IRQ] Got interrupt!\n");
    // ... rest of handler
}
```

**What the output tells you**:
- All init messages appear, but no "[IRQ]" messages → Interrupts not configured correctly
- No messages at all → Program crashed during initialization
- Init messages appear, then silence → Main loop is running, but interrupts aren't firing

## Advanced: Printing Values for Debugging

### Printing Numbers
```c
uart_puts("Tick count: ");
uart_put_number(tick_count);
uart_puts("\n");
```

### Printing Hexadecimal (for register values)
```c
uart_puts("TIMER_CLO = ");
uart_put_hex(TIMER_CLO);
uart_puts("\n");
```

### Checking Register Values
```c
void timer_init(void) {
    uint32_t now = TIMER_CLO;
    uart_puts("[TIMER] Current time: ");
    uart_put_hex(now);
    uart_puts("\n");

    TIMER_C1 = now + tick_interval_us;
    uart_puts("[TIMER] Compare value: ");
    uart_put_hex(TIMER_C1);
    uart_puts("\n");
}
```

This helps verify the timer is actually running and values are reasonable.

## Tips for Effective Debugging

### 1. Start with "Checkpoint" Messages
Add simple messages to confirm each major step completes:
```c
uart_puts("[CHECKPOINT] GPIO init done\n");
uart_puts("[CHECKPOINT] Interrupts enabled\n");
uart_puts("[CHECKPOINT] Timer started\n");
```

### 2. Use Consistent Prefixes
Use tags like `[GPIO]`, `[TIMER]`, `[IRQ]` to quickly identify where messages come from.

### 3. Add Debug Early, Remove It Late
When developing:
- Add lots of debug messages while building your code
- Once everything works, remove or comment out most messages
- Keep important ones (like initialization confirmation)

### 4. Debug in Layers
Start from the basics and work up:
1. First: Verify UART works (`uart_puts("Hello\n")`)
2. Then: Verify initialization completes
3. Then: Check if interrupts fire
4. Finally: Verify logic is correct

### 5. Binary Search for Crashes
If your program crashes but you don't know where:
```c
uart_puts("A\n");
// some code
uart_puts("B\n");
// more code
uart_puts("C\n");
// even more code
uart_puts("D\n");
```

If you see "A" and "B" but not "C", the crash is between "B" and "C". Add more messages in that section to narrow it down.

## When to Remove Debug Messages

### Keep these messages:
- Major initialization confirmations
- Error conditions
- Critical state changes (in moderation)

### Remove these messages:
- Messages in high-frequency code (interrupt handlers that run 100x/sec)
- Verbose step-by-step messages once code is working
- Temporary debugging checkpoints

### Example: Production-Ready Debug Level
```c
void uart_init(void) {
    // Initialization code...
    // NO debug message needed once working
}

void gpio_init(void) {
    // Configuration code...
    uart_puts("[GPIO] Initialization complete\n");  // Keep: confirms it ran
}

void irq_handler(void) {
    // NO debug here - runs too frequently
    if (IRQ_PENDING1 & SYS_TIMER_MATCH1_IRQ) {
        if (TIMER_CS & CS_M1) {
            TIMER_CS = CS_M1;
            tick_count++;
            if (tick_count >= 100u) {
                tick_count = 0;
                led_state = !led_state;
                gpio_set_red(led_state);  // This prints LED state changes
            }
            timer_schedule_next();
        }
    }
}
```

## Summary

**Golden Rules**:
1. Always initialize UART first thing in `main()`
2. Add debug messages at every initialization step
3. Add debug messages when entering critical functions
4. Print state changes (LED on/off, etc.)
5. Remove high-frequency messages once code works
6. Use descriptive prefixes like `[GPIO]`, `[TIMER]`, `[IRQ]`
7. When stuck, add more messages to narrow down the problem

**Remember**: On bare-metal, UART debugging is your best friend. Without it, you're flying blind! Don't hesitate to add messages when debugging - you can always remove them later.
