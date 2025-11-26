# Labo 5 - Exploration Rasberry Pi 

## Table des matières
1. [Explications des changements](#explications)
2. [Crédits](#credits)
3. [Date de remise](#dateRemise)

## Explications des changements <a name="explications"></a>

Changements faits dans l'ordre des mofifications éffectuées : 
- gpio.c  
    Initialisation des pins vert et jaune pour controller les différents LED.  
    Configuration des pins en sortie et leur état dans la console. 


- interrupts.c  
    Premièrement, inclusion de <stdbool.h> afin d'utilier les valeurs booléennes. Celle-ci permettront ensuite d'allumer/d'éteindre les pins.
    Après, définition durée allumage de chaque couleur en ticks (vert = 10s, jaune = 3s et rouge = 15s).   
    Défini les états possibles pour le feu de circulation:   
    allumage pin vert; éteignement des autres pins  
    allumage pin jaune; éteignement des autres pins  
    allumage pin rouge; éteignement des autres pins    
    Configuration de la fonction de changement d'états dans le cycle d'allumage des pins:  
    Un switch qui permet de chagner l'état courant de chaques pins ainsi que le nombre de ticks à attendre pour l'étape suivante. Effectuer aussi cette mise à jour dans les sorties GPIO.  
    Certains changement sont fait dans la fonction "interrupts_ini()" pour permettre d'initialiser l'état du feu au démarrage.  
    Finalement, changement dans la fonction "irq_handler" (gestionnaire d'interruption) permet le programme de savoir quand il faut changer d'état et planifier la prochaine interruption du timer.

- timer.c  
    Changement de la variable tick_interval_us de 10000u -> 1000u. Tous les intervalles de temps sont proportionnels à des unités de temps bien définies ( milliseconde ).  
    Cela simplifie non seulement la compréhension du code, mais aussi les calculs associés.

** Ajout d'un .gitignore pour ignorer les fichiers *.o, .elf* et *.img*. Ce changement rend l'éxecution du programme plus fluide. 

**Sortie attendue :**
```
[GPIO] Initializing pin 17
[GPIO] Set pin 17 as OUTPUT
[GPIO] Pin 17 -> LOW
[GPIO] Initializing pin 27
[GPIO] Set pin 27 as OUTPUT
[GPIO] Pin 27 -> LOW
[GPIO] Initializing pin 22
[GPIO] Set pin 22 as OUTPUT
[GPIO] Pin 22 -> LOW
[GPIO] Pin 22 -> HIGH
[GPIO] Pin 27 -> LOW
[GPIO] Pin 17 -> LOW
[GPIO] Pin 22 -> LOW
[GPIO] Pin 27 -> HIGH
[GPIO] Pin 17 -> LOW
[GPIO] Pin 22 -> LOW
[GPIO] Pin 27 -> LOW
[GPIO] Pin 17 -> HIGH
...
```

## Crédits <a name="credits"></a>
Groupe B
- Mobina Shamsadini - SHAM13520600
- Samara Boudreault - BOUS08610400
- David Chalons - CHAD17070000

## Dates de remise <a name="dateRemise"></a>
- Date limite de remise - 10 Décembre