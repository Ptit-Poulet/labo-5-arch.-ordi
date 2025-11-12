#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

// Initialize UART
void uart_init(void);

// Low-level character output
void uart_putc(char c);

// String output
void uart_puts(const char* str);

// Number output (decimal)
void uart_put_number(int num);

// Hexadecimal output
void uart_put_hex(uint32_t num);

// GPIO debug helpers
void uart_debug_gpio_init(uint32_t pin);
void uart_debug_gpio_write(uint32_t pin, bool state);
void uart_debug_gpio_set_output(uint32_t pin);

#endif // UART_H
