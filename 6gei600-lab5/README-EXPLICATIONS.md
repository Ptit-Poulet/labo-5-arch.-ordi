# Labo 5 - Rasberry Pi exploration

## Table des matières
1. [Explications des changements](#explication)
2. [Crédits](#credits)
3. [Date de remise](#dateRemise)

## Explications des changements <a name="explication"></a>
Changements faits: 
- gpio.c
Initialisation des pins en plus 

- interrupts.c
déroulemetn du cycle entre les trois pins

- timer.c
changemtn tick pour correspondre au changemtn sfait dasn interrupts.c

** add a .gitignore to remove .o .elf and .img files from being tracked. This change was made for easier execution of the program. 

**Expected output:**
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