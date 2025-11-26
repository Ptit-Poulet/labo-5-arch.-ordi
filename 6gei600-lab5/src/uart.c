#include <stdint.h>
#include <stdbool.h>
#include "uart.h"

#define PERIPH_BASE     0x3F000000UL
#define UART0_BASE      (PERIPH_BASE + 0x201000)

#define UART0_DR        (*(volatile uint32_t*)(UART0_BASE + 0x00))
#define UART0_FR        (*(volatile uint32_t*)(UART0_BASE + 0x18))
#define UART0_IBRD      (*(volatile uint32_t*)(UART0_BASE + 0x24))
#define UART0_FBRD      (*(volatile uint32_t*)(UART0_BASE + 0x28))
#define UART0_LCRH      (*(volatile uint32_t*)(UART0_BASE + 0x2C))
#define UART0_CR        (*(volatile uint32_t*)(UART0_BASE + 0x30))
#define UART0_ICR       (*(volatile uint32_t*)(UART0_BASE + 0x44))

#define GPIO_BASE       (PERIPH_BASE + 0x200000)
#define GPFSEL1         (*(volatile uint32_t*)(GPIO_BASE + 0x04))
#define GPPUD           (*(volatile uint32_t*)(GPIO_BASE + 0x94))
#define GPPUDCLK0       (*(volatile uint32_t*)(GPIO_BASE + 0x98))

#define UART0_FR_TXFF   (1 << 5)
#define UART0_FR_RXFE   (1 << 4)

static void delay(int32_t count) {
    while (count-- > 0) {
        __asm__ volatile("nop");
    }
}

void uart_init(void) {
    UART0_CR = 0;

    uint32_t ra = GPFSEL1;
    ra &= ~((7 << 12) | (7 << 15));
    ra |= (4 << 12) | (4 << 15);
    GPFSEL1 = ra;

    GPPUD = 0;
    delay(150);
    GPPUDCLK0 = (1 << 14) | (1 << 15);
    delay(150);
    GPPUDCLK0 = 0;

    UART0_ICR = 0x7FF;

    UART0_IBRD = 26;
    UART0_FBRD = 3;

    UART0_LCRH = (1 << 4) | (1 << 5) | (1 << 6);

    UART0_CR = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_putc(char c) {
    while (UART0_FR & UART0_FR_TXFF) {
    }
    UART0_DR = c;
}

void uart_puts(const char* str) {
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');
        }
        uart_putc(*str++);
    }
}

void uart_put_number(int num) {
    if (num < 0) {
        uart_putc('-');
        num = -num;
    }

    if (num == 0) {
        uart_putc('0');
        return;
    }

    char buffer[12];
    int i = 0;

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0) {
        uart_putc(buffer[--i]);
    }
}

void uart_put_hex(uint32_t num) {
    uart_puts("0x");

    const char hex_chars[] = "0123456789ABCDEF";
    bool started = false;

    for (int i = 28; i >= 0; i -= 4) {
        uint32_t digit = (num >> i) & 0xF;
        if (digit != 0 || started || i == 0) {
            uart_putc(hex_chars[digit]);
            started = true;
        }
    }
}

void uart_debug_gpio_init(uint32_t pin) {
    uart_puts("[GPIO] Initializing pin ");
    uart_put_number(pin);
    uart_puts("\n");
}

void uart_debug_gpio_write(uint32_t pin, bool state) {
    uart_puts("[GPIO] Pin ");
    uart_put_number(pin);
    uart_puts(state ? " -> HIGH\n" : " -> LOW\n");
}

void uart_debug_gpio_set_output(uint32_t pin) {
    uart_puts("[GPIO] Set pin ");
    uart_put_number(pin);
    uart_puts(" as OUTPUT\n");
}
