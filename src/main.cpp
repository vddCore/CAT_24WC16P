#include <Arduino.h>
#include <Wire.h>

#define EEP_24WC16_SIZE 2048
#define EEP_24WC16_PAGE 256

uint8_t EEP_24WC16_MakePhysicalAddress(uint16_t addr, bool write) {
  if (addr >= EEP_24WC16_SIZE)
    return 0;

  uint8_t i2c_addr = 0;
  i2c_addr |= (uint8_t)(5 << 4); // hardwired 1010

  if (!write)
    i2c_addr |= 1 << 7;

  i2c_addr |= (addr / EEP_24WC16_PAGE) % 8;
  return i2c_addr;
}

int16_t EEP_24WC16_ReadAddress(uint16_t addr) {
  if (addr >= EEP_24WC16_SIZE)
    return -1;

  uint8_t i2c_addr_sel = EEP_24WC16_MakePhysicalAddress(addr, true);
  uint8_t i2c_addr_read = EEP_24WC16_MakePhysicalAddress(addr, false);

  Wire.beginTransmission(i2c_addr_sel);
  Wire.write((uint8_t)(addr % EEP_24WC16_PAGE));
  Wire.requestFrom(i2c_addr_read, 1, 0);
  uint8_t err = Wire.endTransmission();
  
  if (err) {
    Serial.print("Selective read error: ");
    Serial.println(err, DEC);

    return -1;
  }

  return Wire.read();
}

bool EEP_24WC16_WriteAddress(uint16_t addr, uint8_t value) {
  if (addr >= EEP_24WC16_SIZE)
    return false;

  uint8_t i2c_addr_wr = EEP_24WC16_MakePhysicalAddress(addr, true);

  Wire.beginTransmission(i2c_addr_wr);
  Wire.write((uint8_t)(addr % EEP_24WC16_PAGE));
  Wire.write(value);
  uint8_t err = Wire.endTransmission();

  if (err) { 
    Serial.print("Write error: ");
    Serial.println(err, DEC);

    return false;
  }

  return true;
}

void I2C_ScanBus() {
  for (uint8_t i = 0; i < 0xFF; i++) {
    Wire.beginTransmission(i);
    uint8_t err = Wire.endTransmission();

    if (!err) {
      Serial.println(i, HEX);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  Serial.begin(9600);
  Wire.begin();

  // Serial.println("writing...");
  // for (uint16_t i = 0; i < 255; i++) {
  //   uint8_t nibble = 0xD1;
  //   if (i % 2 != 0) {
  //     nibble = 0xCC;
  //   }
    
  //   if (!EEP_24WC16_WriteAddress(i, nibble))
  //     return;

  //   delay(25);
  // }

  Serial.println("reading...");
  for (uint16_t i = 0; i < 255; i++) {
    Serial.print(EEP_24WC16_ReadAddress(i), HEX);
    Serial.print(" ");
  }
}

void loop() { }