#include <stdint.h>
#include <stdbool.h>

void uart_init(void);
void gpio_init(void);
void timer_init(void);
void interrupts_init(void);

int main(void) {
    uart_init();
    gpio_init();
    interrupts_init();
    timer_init();

    for (;;) {
        __asm__ volatile ("wfi");
    }
}
