# Contexte du projet

Dépôt de projets embarqués (ESP32, Arduino, Nucleo) développé en binôme :
- Le fils s'occupe de la conception logicielle (souvent en discussion avec Claude.ai
  pour la réflexion/conception avant d'arriver ici).
- Le père s'occupe de la conception et du câblage du hardware.

Quand tu (Claude Code) es sollicité ici, c'est en général pour **écrire les fichiers
et gérer git** (commit/push) à partir de spécifications déjà réfléchies en amont.
Si une instruction manque de contexte technique (valeur de composant, choix de carte...),
demande plutôt que de deviner.

## Conventions du dépôt

- Un sous-dossier par projet à la racine (ex: `capteurs-pression/`).
- Chaque projet est un projet PlatformIO : `platformio.ini` à la racine du sous-dossier,
  code dans `src/`.
- Un `README.md` par sous-dossier de projet expliquant le câblage, le matériel utilisé
  et les points à vérifier/calibrer avant test.
- Commentaires de code et README en français.

## Projet en cours : capteurs-pression/ (détection d'encrassement de filtre)

Objectif final (pas encore terminé) : détecter l'encrassement de filtres à eau par
mesure de pression différentielle, puis piloter une électrovanne pour un système
anticalcaire à base de CO2. **Étape actuelle : lecture des capteurs uniquement**,
le pilotage de l'électrovanne n'est pas encore implémenté.

Installation : 2 filtres montés en série, donc 3 points de mesure de pression
(A0 = avant filtre 1, A1 = entre filtre 1 et filtre 2, A2 = après filtre 2), pour
calculer un delta de pression (donc un encrassement) par filtre.

Matériel choisi :
- 3x capteurs de pression industriels 4-20mA, G1/4, alimentation boucle 12-36V.
- 1x module ADS1115 (ADC I2C 16 bits, 4 canaux) — module brut, pas de shunt intégré.
- 1x ESP32-S2 Mini (mono-cœur, pas de Bluetooth, USB natif). Certains clones ont des
  soucis USB signalés en avis client — toujours tester la carte à vide avant câblage.

Choix de câblage retenus :
- Les 3 capteurs tiennent sur les 4 canaux du même ADS1115 (1 canal libre restant).
- Résistance de shunt 150 ohms par capteur (0,6V à 4mA, 3,0V à 20mA) — compatible
  ADS1115 alimenté en 3,3V depuis l'ESP32 (pas besoin de level-shifter I2C).
- Masse commune obligatoire entre alimentation 24V des capteurs, ESP32 et ADS1115.
- ADS1115 : VDD sur 3V3 (pas 5V), SDA/SCL définis en haut de `main.cpp` (33/35 par
  défaut — à vérifier sur le silkscreen de la carte reçue, ça varie selon les clones).
- Gain PGA ADS1115 : GAIN_ONE (+/-4.096V), adapté à la plage 0-3,3V du shunt.

À faire / décisions en attente :
- Confirmer la plage de mesure réelle des capteurs commandés (ex: 0-1Mpa = 0-10 bar)
  pour ajuster `P_MIN_BAR` / `P_MAX_BAR` dans `main.cpp`.
- Filtrage/lissage des mesures (moyenne glissante).
- Seuil de détection d'encrassement (delta P au-delà duquel le filtre est considéré
  colmaté).
- Pilotage de l'électrovanne CO2 (non commencé).