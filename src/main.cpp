#include <Arduino.h>
#include <Wire.h>

#include "24WC16.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  Wire.begin();
  Serial.begin(9600);

  Serial.println("writing single-byte 0-64...");
  for (uint16_t i = 0; i < 64; i++) {
    if (!EEP_24WC16_WriteByte(i, 0x69)) {
      Serial.print("WR_ERR! Code ");
      Serial.print(EEP_24WC16_GetFailureReason(), DEC);

      break;
    }
  }

  uint8_t values[16] = {
    0x12, 0x14, 0x16, 0x18,
    0x20, 0x22, 0x24, 0x26,
    0x28, 0x30, 0x32, 0x34,
    0x36, 0x38, 0xDE, 0xAD
  };

  Serial.println("writing burst 0-16...");
  if (!EEP_24WC16_WriteBurst(0, values, 16)) {
    Serial.print("WR2_ERR! Code ");
    Serial.print(EEP_24WC16_GetFailureReason());
  }

  Serial.println("reading...");
  uint8_t val = 0;
  for (uint16_t i = 0; i < 64; i++) {
    if (!EEP_24WC16_ReadByte(i, &val)) {
      Serial.print("RD_ERR! Code ");
      Serial.println(EEP_24WC16_GetFailureReason(), DEC);

      break;
    }

    Serial.print(val, HEX);
    Serial.print(" ");
  }
}

void loop() { }