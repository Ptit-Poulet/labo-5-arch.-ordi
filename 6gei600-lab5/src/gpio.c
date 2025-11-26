#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"

#define PERIPH_BASE      0x3F000000UL
#define GPIO_BASE       (PERIPH_BASE + 0x200000)

#define GPFSEL0         (*(volatile uint32_t*)(GPIO_BASE + 0x00))
#define GPFSEL1         (*(volatile uint32_t*)(GPIO_BASE + 0x04))
#define GPFSEL2         (*(volatile uint32_t*)(GPIO_BASE + 0x08))
#define GPSET0          (*(volatile uint32_t*)(GPIO_BASE + 0x1C))
#define GPCLR0          (*(volatile uint32_t*)(GPIO_BASE + 0x28))

#define PIN_RED         17u

/* Ajout des pins supplémentaires pour contrôler les LEDs avec le Raspberry Pi */
#define PIN_YELLOW      27u 
#define PIN_GREEN       22u 

static inline void gpio_set_output(uint32_t pin) {
    volatile uint32_t* gpfsel;
    uint32_t shift = (pin % 10u) * 3u;
    if (pin < 10u) gpfsel = &GPFSEL0;
    else if (pin < 20u) gpfsel = &GPFSEL1;
    else gpfsel = &GPFSEL2;
    uint32_t val = *gpfsel;
    val &= ~(0x7u << shift);
    val |=  (0x1u << shift);
    *gpfsel = val;

    uart_debug_gpio_set_output(pin);
}

static inline void gpio_write(uint32_t pin, bool on) {
    if (on) GPSET0 = (1u << pin);
    else    GPCLR0 = (1u << pin);

    uart_debug_gpio_write(pin, on);
}

void gpio_init(void) {
    uart_debug_gpio_init(PIN_RED);
    gpio_set_output(PIN_RED);
    gpio_write(PIN_RED, false);

    /* Initialisation des nouveaux pins */
    uart_debug_gpio_init(PIN_YELLOW);   // Initialisation du pin avec UART pour débogage
    gpio_set_output(PIN_YELLOW);        // Configuration du pin en sortie
    gpio_write(PIN_YELLOW, false);      // Éteint la LED (pin jaune éteint)

    uart_debug_gpio_init(PIN_GREEN);
    gpio_set_output(PIN_GREEN);
    gpio_write(PIN_GREEN, false);
}

void gpio_set_red(bool on)   { gpio_write(PIN_RED, on); }

/* Configuration de l'état des pins et écrire leur état dans la console*/
void gpio_set_yellow(bool on)   { gpio_write(PIN_YELLOW, on); }  // Écrit l'état 'on' sur le pin correspondant à la LED 
void gpio_set_green(bool on)   { gpio_write(PIN_GREEN, on); }

