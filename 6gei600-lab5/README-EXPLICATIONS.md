# Laboratoire 5 - Contrôleur de feu de circulation sur Raspberry Pi

## Table des matières

1. [Description du projet](#description)
2. [Explications des changements](#explications)
3. [Sortie attendue](#sortie)
4. [Crédits](#credits)
5. [Date de remise](#dateRemise)

## Description du projet <a name="description"></a>

Ce projet vise à concevoir et mettre en œuvre un système de contrôle pour un feu de circulation à l'aide d'un microcontrôleur Raspberry Pi, en utilisant le langage C et la gestion des interruptions.

## Explications des changements techniques <a name="explications"></a>

Voici les détails des modifications apportées aux fichiers sources pour répondre aux exigences du laboratoire :

### 1. `gpio.c`

- **Initialisation des broches (Pins) :** Ajout de l'initialisation des broches pour les DELs verte et jaune (en plus de la rouge existante) pour contrôler les trois feux.
- **Configuration :** Configuration explicite de ces broches en mode `OUTPUT` (sortie) et initialisation de leur état à `LOW` (éteint) pour éviter tout comportement indésirable au démarrage.

### 2. `interrupts.c`

C'est ici que réside la logique principale du contrôle de flux.

- **Inclusion de bibliothèques :** Ajout de `<stdbool.h>` pour l'utilisation de types booléens, facilitant la gestion de l'état (allumé/éteint) des broches.
- **Définition des délais (Ticks) :** Configuration des constantes de temps pour respecter la séquence demandée :
  - Vert : ~10 secondes
  - Jaune : ~3 secondes
  - Rouge : ~15 secondes
- **Machine à états :** Implémentation d'une structure `switch-case` dans la fonction de gestion pour contrôler le cycle :
  1.  Allumage de la DEL verte (autres éteintes).
  2.  Allumage de la DEL jaune (autres éteintes).
  3.  Allumage de la DEL rouge (autres éteintes).
- **Gestionnaire d'interruption (`irq_handler`) :** Modification de la logique pour décrémenter le compteur de temps et déclencher le changement d'état (et la reprogrammation du timer) une fois le délai écoulé.
- **Initialisation (`interrupts_init`) :** Mise à jour pour définir l'état initial du feu au lancement du programme.

### 3. `timer.c`

- **Précision du temporisateur :** Modification de la variable `tick_interval_us` de `10000u` à `1000u` (1 milliseconde).
  - _Raison :_ Cela permet d'avoir des intervalles de temps proportionnels à une unité standard (ms), simplifiant les calculs de conversion pour les délais de 10s, 3s et 15s.

### 4. Divers

- **`.gitignore` :** Ajout d'un fichier `.gitignore` pour exclure les fichiers binaires et temporaires (`*.o`, `*.elf`, `*.img`) du suivi de version, assurant un dépôt propre.

## Sortie attendue dans la console <a name="sortie"></a>

Lors de l'exécution sur le Raspberry Pi, la console affichera la séquence d'initialisation suivie du cycle des feux :

text
[GPIO] Initializing pin 17
[GPIO] Set pin 17 as OUTPUT
[GPIO] Pin 17 -> LOW
[GPIO] Initializing pin 27
[GPIO] Set pin 27 as OUTPUT
[GPIO] Pin 27 -> LOW
[GPIO] Initializing pin 22
[GPIO] Set pin 22 as OUTPUT
[GPIO] Pin 22 -> LOW
[GPIO] Pin 22 -> HIGH (Début du cycle)
[GPIO] Pin 27 -> LOW
[GPIO] Pin 17 -> LOW
[GPIO] Pin 22 -> LOW
[GPIO] Pin 27 -> HIGH
[GPIO] Pin 17 -> LOW
[GPIO] Pin 22 -> LOW
[GPIO] Pin 27 -> LOW
[GPIO] Pin 17 -> HIGH
...

## Crédits <a name="credits"></a>

Groupe B

Mobina Shamsadini - SHAM13520600
Samara Boudreault - BOUS08610400
David Chalons - CHAD17070000

## Dates de remise <a name="dateRemise"></a>

- Date limite de remise - 10 Décembre
