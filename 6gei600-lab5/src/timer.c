#include <stdint.h>
#include "timer.h"

#define PERIPH_BASE       0x3F000000UL
#define SYS_TIMER_BASE   (PERIPH_BASE + 0x003000)

#define TIMER_CS         (*(volatile uint32_t*)(SYS_TIMER_BASE + 0x00))
#define TIMER_CLO        (*(volatile uint32_t*)(SYS_TIMER_BASE + 0x04))
#define TIMER_C1         (*(volatile uint32_t*)(SYS_TIMER_BASE + 0x10))

#define CS_M1            (1u << 1)

static const uint32_t tick_interval_us = 10000u;

void timer_init(void) {
    uint32_t now = TIMER_CLO;
    TIMER_C1 = now + tick_interval_us;
    TIMER_CS = CS_M1;
}

void timer_schedule_next(void) {
    TIMER_C1 = TIMER_CLO + tick_interval_us;
}

