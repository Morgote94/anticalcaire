#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#define I2C_SDA 33
#define I2C_SCL 35
#define SHUNT_OHMS 150.0
#define I_MIN_MA 4.0
#define I_MAX_MA 20.0
#define P_MIN_BAR 0.0
#define P_MAX_BAR 10.0

Adafruit_ADS1115 ads;

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
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!ads.begin()) {
    Serial.println("Erreur : ADS1115 non detecte. Verifie le cablage I2C (SDA/SCL/VDD/GND).");
    while (1) {
      delay(1000);
    }
  }

  ads.setGain(GAIN_ONE);
  Serial.println("Lecture des capteurs de pression - demarrage");
}

void loop() {
  int16_t raw_avant = ads.readADC_SingleEnded(0);
  int16_t raw_apres = ads.readADC_SingleEnded(1);

  float v_avant = ads.computeVolts(raw_avant);
  float v_apres = ads.computeVolts(raw_apres);

  float p_avant = voltageToPressure(v_avant);
  float p_apres = voltageToPressure(v_apres);
  float delta_p = p_avant - p_apres;

  Serial.print("P avant: ");
  Serial.print(p_avant, 3);
  Serial.print(" bar | P apres: ");
  Serial.print(p_apres, 3);
  Serial.print(" bar | Delta P (encrassement): ");
  Serial.print(delta_p, 3);
  Serial.println(" bar");

  delay(1000);
}
