# Lecture des capteurs de pression (encrassement filtre)

## Organisation des fichiers dans le depot GitHub

Cree ce dossier a la racine du depot, avec cette structure :

capteurs-pression/
├── platformio.ini
├── src/
│   └── main.cpp
└── README.md

(Le fichier main.cpp fourni va dans src/, platformio.ini reste a la racine du dossier capteurs-pression/.)

## Materiel

Installation avec 2 filtres montes en serie, donc 3 points de mesure de pression
(avant filtre 1 / entre les deux filtres / apres filtre 2) pour pouvoir calculer
l'encrassement de chaque filtre independamment.

- 3x capteurs de pression 4-20mA, G1/4, alimentation boucle 12-36V
- 1x module ADS1115 (ADC I2C 16 bits, 4 canaux - il en reste 1 de libre)
- 1x ESP32-S2 Mini
- 3x resistances de precision 150 ohms (1%)
- 1x alimentation externe 24V DC (pour la boucle de courant des capteurs)

## Cablage

Pour chaque capteur (le principe est identique pour les 3) :

1. Fil + du capteur -> 24V+ de l'alimentation externe.
2. Fil - du capteur -> point commun entre :
   - l'entree analogique de l'ADS1115 (A0 = avant filtre 1, A1 = entre filtre 1
     et filtre 2, A2 = apres filtre 2)
   - une extremite de la resistance de 150 ohms
3. L'autre extremite de la resistance de 150 ohms -> masse commune (GND).

Important : la masse (GND) de l'alimentation 24V, du module ADS1115 et de l'ESP32 doivent
toutes etre reliees ensemble. Sans masse commune, les mesures seront incoherentes ou nulles.

Cote ADS1115 :
- VDD -> 3V3 de l'ESP32 (pas 5V, pour rester compatible I2C avec le S2 Mini)
- GND -> GND commun
- SDA -> broche definie par I2C_SDA dans le code (33 par defaut, a verifier sur la carte)
- SCL -> broche definie par I2C_SCL dans le code (35 par defaut, a verifier sur la carte)

## A verifier / regler avant le premier test

Dans main.cpp, en haut du fichier :

- I2C_SDA / I2C_SCL : verifier que ca correspond aux broches marquees sur le silkscreen
  de votre carte S2 Mini (les clones varient parfois).
- P_MIN_BAR / P_MAX_BAR : a regler selon la plage de mesure choisie a la commande du
  capteur (ex: si vous avez pris "0-1Mpa", ca correspond a 0-10 bar).

## Test

1. Ouvrir le projet dans VS Code + extension PlatformIO.
2. Brancher l'ESP32-S2 Mini en USB (tester d'abord SANS les capteurs branches : la carte
   doit etre reconnue et un programme vide doit s'uploader correctement - certains clones
   de cette carte ont des soucis d'USB signales par d'autres utilisateurs).
3. Une fois le cablage fait, uploader main.cpp (bouton "Upload" de PlatformIO).
4. Ouvrir le moniteur serie (115200 bauds) : les 3 pressions et les deltas de chaque
   filtre s'affichent toutes les secondes.

## Etapes suivantes (pas encore dans ce code)

- Filtrage/lissage des mesures (moyenne glissante) pour eviter les faux positifs.
- Seuil de detection d'encrassement par filtre (delta P au-dela duquel on considere
  le filtre colmate).
- Pilotage de l'electrovanne CO2 (ajoute dans une prochaine iteration).
