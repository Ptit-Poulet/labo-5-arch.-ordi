#include <stdint.h>
#include <stdbool.h> // Inclusion de la librairie de Booléenne
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

/*Durée d'allumage pour chaques couleurs*/
#define GREEN_TICKS   10000u   
#define YELLOW_TICKS  3000u    
#define RED_TICKS     15000u   

/*Définition d'énumeration des états possible*/
typedef enum { STATE_GREEN, STATE_YELLOW, STATE_RED } light_state_t;
static light_state_t current_state;       // Introduction de la variable d'état courant 

static uint32_t tick_count = 0;

/*Nombre de tick a attendre dans l'état courant (initialement vert)*/
static uint32_t target_ticks;

/*Choix du pin a allumé*/
static inline void drive_outputs(light_state_t s) {
    switch (s) {
        case STATE_GREEN:
         // Allume le pin jaune, éteint les autres
            gpio_set_green(true);
            gpio_set_yellow(false);
            gpio_set_red(false);
            break;
        case STATE_YELLOW:
        // Allume le pin rouge, éteint les autres
            gpio_set_green(false);
            gpio_set_yellow(true);
            gpio_set_red(false);
            break;
        case STATE_RED:
        // Allume le pin vert, éteint les autres
            gpio_set_green(false);
            gpio_set_yellow(false);
            gpio_set_red(true);
            break;
    }
}

/*Fonction pour faire passer le feu à l'état suivant*/
static inline void advance_state(void) {
    
    /*Selon le cas courant, déterminer le prochain cas*/
    switch (current_state) { 
        case STATE_GREEN:
            current_state = STATE_YELLOW; //Changement de l'état
            target_ticks = YELLOW_TICKS; // Changement du nombre de ticks à attendre
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
  
    /*Initialisation de l'état du feu au démarrage*/
    current_state = STATE_GREEN;
    target_ticks = GREEN_TICKS;
    tick_count = 0;
    drive_outputs(current_state);
}

/*Gestionnaire d'interruption*/
void irq_handler(void) {
    if (IRQ_PENDING1 & SYS_TIMER_MATCH1_IRQ) {
        if (TIMER_CS & CS_M1) {
            TIMER_CS = CS_M1;      
            tick_count++;         
            //Si on a atteint ou passé le nombre de ticks prévu pour l'état courant du LED
            if (tick_count >= target_ticks) {
                advance_state();  // Passer  à l'état suivant du feu 
            }
            timer_schedule_next(); //Programmer la prochaine interruption
        }
        return;
    }
}