#include <stdint.h>
#include "timer.h"

#define PERIPH_BASE       0x3F000000UL
#define SYS_TIMER_BASE   (PERIPH_BASE + 0x003000)

#define TIMER_CS         (*(volatile uint32_t*)(SYS_TIMER_BASE + 0x00))
#define TIMER_CLO        (*(volatile uint32_t*)(SYS_TIMER_BASE + 0x04))
#define TIMER_C1         (*(volatile uint32_t*)(SYS_TIMER_BASE + 0x10))

#define CS_M1            (1u << 1)

/* Changement de la variable tick_interval_us de 10000u -> 1000u 
   Ce changement permet de définir un intervalle d'environ 1000 microsecondes 
   (1 milliseconde) entre chaque événement du timer. (u = unsigned) */
static const uint32_t tick_interval_us = 1000u; // Intervalle du timer en microsecondes (1ms)

void timer_init(void) {
    uint32_t now = TIMER_CLO;
    TIMER_C1 = now + tick_interval_us;
    TIMER_CS = CS_M1;
}

void timer_schedule_next(void) {
    TIMER_C1 = TIMER_CLO + tick_interval_us;
}

