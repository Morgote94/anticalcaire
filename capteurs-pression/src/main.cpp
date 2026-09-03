#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// Broches I2C utilisees pour dialoguer avec l'ADS1115 (a verifier selon la carte)
#define I2C_SDA 33
#define I2C_SCL 35

// Valeur de la resistance de shunt (en ohms) placee sur chaque boucle de courant.
// C'est elle qui transforme le courant 4-20mA du capteur en une tension mesurable par l'ADS1115.
#define SHUNT_OHMS 150.0

// Plage de courant standard des capteurs 4-20mA (4mA = pression min, 20mA = pression max)
#define I_MIN_MA 4.0
#define I_MAX_MA 20.0

// Plage de pression correspondante (en bar), a adapter selon le capteur commande
#define P_MIN_BAR 0.0
#define P_MAX_BAR 10.0

// Objet pilotant le convertisseur analogique-numerique I2C ADS1115
Adafruit_ADS1115 ads;

// Convertit la tension mesuree aux bornes de la resistance de shunt en une pression (bar).
// Etapes : tension -> courant (loi d'Ohm) -> on borne au cas où le courant sortirait
// de la plage 4-20mA (bruit, capteur deconnecte) -> on ramene le courant a une pression
// par interpolation lineaire entre (I_MIN_MA, P_MIN_BAR) et (I_MAX_MA, P_MAX_BAR).
float voltageToPressure(float voltage) {
  float current_mA = (voltage / SHUNT_OHMS) * 1000.0;

  if (current_mA < I_MIN_MA) current_mA = I_MIN_MA;
  if (current_mA > I_MAX_MA) current_mA = I_MAX_MA;

  float ratio = (current_mA - I_MIN_MA) / (I_MAX_MA - I_MIN_MA);
  return P_MIN_BAR + ratio * (P_MAX_BAR - P_MIN_BAR);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialise le bus I2C sur les broches choisies avant de parler a l'ADS1115
  Wire.begin(I2C_SDA, I2C_SCL);

  // Si l'ADS1115 ne repond pas, on bloque le programme avec un message d'erreur
  // plutot que de continuer a lire des valeurs incoherentes.
  if (!ads.begin()) {
    Serial.println("Erreur : ADS1115 non detecte. Verifie le cablage I2C (SDA/SCL/VDD/GND).");
    while (1) {
      delay(1000);
    }
  }

  // Gain x1 : plage de mesure +/-4.096V, adaptee a une tension de shunt qui reste
  // en dessous de quelques volts (20mA * 150 ohms = 3V max)
  ads.setGain(GAIN_ONE);
  Serial.println("Lecture des capteurs de pression - demarrage");
}

void loop() {
  // Lecture brute (16 bits) des deux entrees de l'ADS1115 :
  // A0 = capteur AVANT le filtre, A1 = capteur APRES le filtre
  int16_t raw_avant = ads.readADC_SingleEnded(0);
  int16_t raw_apres = ads.readADC_SingleEnded(1);

  // Conversion de la valeur brute en tension reelle (volts)
  float v_avant = ads.computeVolts(raw_avant);
  float v_apres = ads.computeVolts(raw_apres);

  // Conversion de chaque tension en pression (bar)
  float p_avant = voltageToPressure(v_avant);
  float p_apres = voltageToPressure(v_apres);

  // Delta de pression entre avant et apres le filtre : plus il est grand,
  // plus le filtre est encrasse (perte de charge elevee)
  float delta_p = p_avant - p_apres;

  Serial.print("P avant: ");
  Serial.print(p_avant, 3);
  Serial.print(" bar | P apres: ");
  Serial.print(p_apres, 3);
  Serial.print(" bar | Delta P (encrassement): ");
  Serial.print(delta_p, 3);
  Serial.println(" bar");

  // Une mesure par seconde
  delay(1000);
}
