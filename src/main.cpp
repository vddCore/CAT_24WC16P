#include <Arduino.h>
#include <Wire.h>
#include <string.h>

#include "24WC16.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  Wire.begin();
  Serial.begin(9600);

  uint8_t segment[128] = { 0 };
  if (!EEP_24WC16_ReadSequential(200, segment, (uint16_t)128)) {
    Serial.print("RD ERR: ");
    Serial.println(EEP_24WC16_GetFailureReason(), DEC);

    return;
  }

  for (uint8_t i = 0; i < 128; i++) {
    Serial.print(segment[i], HEX);
    Serial.print(" ");
  }

  Serial.println("");
  Serial.println("All done!");
}

void loop() { }