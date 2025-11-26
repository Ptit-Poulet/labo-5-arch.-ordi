#include <stdint.h>
/* Inclusion de la bibliothèque stdbool.h
   Cette bibliothèque définit le type booléen et les macros TRUE / FALSE,
   permettant ainsi d'utiliser des valeurs booléennes (ce qui n'est pas natif en C). */
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

// Durée d'allumage pour chaque couleur (en ticks)
#define GREEN_TICKS   10000u   
#define YELLOW_TICKS  3000u    
#define RED_TICKS     15000u   

// Définition de l'énumération des états possibles du feu tricolore
typedef enum { STATE_GREEN, STATE_YELLOW, STATE_RED } light_state_t;
static light_state_t current_state;       // Variable qui contient l'état courant du feu

static uint32_t tick_count = 0;           // Compteur de ticks depuis le dernier changement d'état

// Nombre de ticks à attendre dans l'état courant (initialement vert)
static uint32_t target_ticks;

// Fonction qui contrôle les sorties GPIO en fonction de l'état du feu
static inline void drive_outputs(light_state_t s) {
    switch (s) {
        case STATE_GREEN:
         // Allume le pin vert, éteint les autres
            gpio_set_green(true);
            gpio_set_yellow(false);
            gpio_set_red(false);
            break;
        case STATE_YELLOW:
        // Allume le pin jaune, éteint les autres
            gpio_set_green(false);
            gpio_set_yellow(true);
            gpio_set_red(false);
            break;
        case STATE_RED:
        // Allume le pin rouge, éteint les autres
            gpio_set_green(false);
            gpio_set_yellow(false);
            gpio_set_red(true);
            break;
    }
}

// Fonction pour faire passer le feu à l'état suivant
static inline void advance_state(void) {
    
     // Passage d'un état à l'autre (transition du feu)
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
    drive_outputs(current_state);  // Mise à jour des sorties GPIO
    tick_count = 0;                // Réinitialisation du compteur de ticks

}

void interrupts_init(void) {
    IRQ_ENABLE1 = SYS_TIMER_MATCH1_IRQ;  
  
    // Initialisation de l'état du feu au démarrage
    current_state = STATE_GREEN;
    target_ticks = GREEN_TICKS;
    tick_count = 0;
    drive_outputs(current_state);
}

// Gestionnaire d'interruption
void irq_handler(void) {
    if (IRQ_PENDING1 & SYS_TIMER_MATCH1_IRQ) {
        if (TIMER_CS & CS_M1) {
            TIMER_CS = CS_M1;      
            tick_count++;         
            // Si le nombre de ticks est atteint ou dépassé pour l'état courant
            if (tick_count >= target_ticks) {
                advance_state();  // Passer  à l'état suivant du feu 
            }
            timer_schedule_next(); // Planifie la prochaine interruption du timer
        }
        return;
    }
}