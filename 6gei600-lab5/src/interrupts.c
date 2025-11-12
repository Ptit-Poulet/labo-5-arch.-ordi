#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "gpio.h"

#define PERIPH_BASE        0x3F000000UL
#define INTC_BASE         (PERIPH_BASE + 0x00B200)

#define IRQ_BASIC_PENDING (*(volatile uint32_t*)(INTC_BASE + 0x00))
#define IRQ_PENDING1      (*(volatile uint32_t*)(INTC_BASE + 0x04))
#define IRQ_PENDING2      (*(volatile uint32_t*)(INTC_BASE + 0x08))
#define IRQ_ENABLE1       (*(volatile uint32_t*)(INTC_BASE + 0x10))
#define IRQ_ENABLE2       (*(volatile uint32_t*)(INTC_BASE + 0x14))
#define IRQ_ENABLE_BASIC  (*(volatile uint32_t*)(INTC_BASE + 0x18))
#define IRQ_DISABLE1      (*(volatile uint32_t*)(INTC_BASE + 0x1C))

#define SYS_TIMER_MATCH1_IRQ (1u << 1)

#define SYS_TIMER_BASE    (PERIPH_BASE + 0x003000)
#define TIMER_CS          (*(volatile uint32_t*)(SYS_TIMER_BASE + 0x00))
#define CS_M1             (1u << 1)

/* ====== Set durations here (in ticks) ======
   If your timer tick is 10 ms, use:
     10 s -> 1000, 3 s -> 300, 15 s -> 1500
   If your tick is 1 ms, multiply by 1000 instead, etc.
*/
#define GREEN_TICKS   1000u   /* ~10 s */
#define YELLOW_TICKS  300u    /* ~3 s  */
#define RED_TICKS     15000u   /* ~15 s */

/* ====== minimal additions: enum + targets ====== */
typedef enum { STATE_GREEN, STATE_YELLOW, STATE_RED } light_state_t;
static light_state_t current_state = STATE_GREEN;

static uint32_t tick_count = 0;
static uint32_t target_ticks = GREEN_TICKS;

static inline void drive_outputs(light_state_t s) {
    switch (s) {
        case STATE_GREEN:
            gpio_set_green(true);
            gpio_set_yellow(false);
            gpio_set_red(false);
            break;
        case STATE_YELLOW:
            gpio_set_green(false);
            gpio_set_yellow(true);
            gpio_set_red(false);
            break;
        case STATE_RED:
            gpio_set_green(false);
            gpio_set_yellow(false);
            gpio_set_red(true);
            break;
    }
}

static inline void advance_state(void) {
    switch (current_state) {
        case STATE_GREEN:
            current_state = STATE_YELLOW;
            target_ticks = YELLOW_TICKS;
            break;
        case STATE_YELLOW:
            current_state = STATE_RED;
            target_ticks = RED_TICKS;
            break;
        case STATE_RED:
            current_state = STATE_GREEN;
            target_ticks = GREEN_TICKS;
            break;
    }
    drive_outputs(current_state);
    tick_count = 0; 
}

void interrupts_init(void) {
    IRQ_ENABLE1 = SYS_TIMER_MATCH1_IRQ;  
  
    current_state = STATE_GREEN;
    target_ticks = GREEN_TICKS;
    tick_count = 0;
    drive_outputs(current_state);
}

void irq_handler(void) {
    if (IRQ_PENDING1 & SYS_TIMER_MATCH1_IRQ) {
        if (TIMER_CS & CS_M1) {
            TIMER_CS = CS_M1;      
            tick_count++;         
            if (tick_count >= target_ticks) {
                advance_state();   
            }
            timer_schedule_next(); 
        }
        return;
    }
}