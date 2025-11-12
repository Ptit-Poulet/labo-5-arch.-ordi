#pragma once
#include <stdbool.h>

void gpio_init(void);
void gpio_set_red(bool on);
void gpio_set_green(bool on);
void gpio_set_yellow(bool on);

