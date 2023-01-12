#include <Arduino.h>
#include <Wire.h>

#include "24WC16.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  Wire.begin();
  Serial.begin(9600);

  Serial.println("writing...");
  for (uint16_t i = 0; i < 64; i++) {
    if (!EEP_24WC16_WriteLinear(i, 0x69)) {
      Serial.print("WR_ERR! Code ");
      Serial.print(EEP_24WC16_GetFailureReason(), DEC);

      break;
    }
  }

  Serial.println("reading...");

  uint8_t val = 0;
  for (uint16_t i = 0; i < 64; i++) {
    if (!EEP_24WC16_ReadLinear(i, &val)) {
      Serial.print("RD_ERR! Code ");
      Serial.println(EEP_24WC16_GetFailureReason(), DEC);

      break;
    }

    Serial.print(val, HEX);
    Serial.print(" ");
  }
}

void loop() { }