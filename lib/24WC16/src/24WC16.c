#include <Arduino.h>
#include <utility/twi.h>

#include "24WC16.h"

static uint8_t g_failure_reason = 0;

static uint8_t EEP_24WC16_MakeLinearAddress(uint16_t addr, bool write) {
  uint8_t i2c_addr = 0;
  i2c_addr |= (uint8_t)(5 << 4); // hardwired x1010xxx

  if (!write) {
    i2c_addr |= 1 << 7;
  }

  i2c_addr |= (addr / EEP_24WC16_PAGE_SIZE) % 8;
  return i2c_addr;
}

uint8_t EEP_24WC16_GetFailureReason(void) {
  return g_failure_reason;
}

bool EEP_24WC16_ReadLinear(uint16_t addr, uint8_t* value) {
  if (!TWCR) {
    g_failure_reason = E_TWI_NOT_INITIALIZED;
    return false;
  }

  if (addr >= EEP_24WC16_CHIP_SIZE) {
    g_failure_reason = E_ADDR_OUT_OF_BOUNDS;
    return false;
  }

  uint8_t i2c_addr_sel = EEP_24WC16_MakeLinearAddress(addr, true);
  uint8_t i2c_addr_read = EEP_24WC16_MakeLinearAddress(addr, false);
  uint8_t addr_in_page = (uint8_t)(addr % EEP_24WC16_PAGE_SIZE);

  g_failure_reason = twi_writeTo(
    i2c_addr_sel,
    &addr_in_page, 
    (uint8_t)1, 
    (uint8_t)1, 
    (uint8_t)0
  );

  if (g_failure_reason) {
    return false;
  }

  uint8_t ret = twi_readFrom(
    i2c_addr_read,
    value,
    (uint8_t)1,
    (uint8_t)1
  );

  if (!ret) {
    g_failure_reason = E_TWI_READ_FAILED;
    return false;
  }

  return true;
}

bool EEP_24WC16_WriteLinear(uint16_t addr, uint8_t value) {
  if (!TWCR) {
    g_failure_reason = E_TWI_NOT_INITIALIZED;
    return false;
  }

  if (addr >= EEP_24WC16_CHIP_SIZE) {
    g_failure_reason = E_ADDR_OUT_OF_BOUNDS;
    return false;
  }

  uint8_t i2c_addr_wr = EEP_24WC16_MakeLinearAddress(addr, true);
  uint8_t addr_in_page = (uint8_t)(addr % EEP_24WC16_PAGE_SIZE);

  uint8_t cmd[2] = { addr_in_page, value };

  g_failure_reason = twi_writeTo(
    i2c_addr_wr,
    &cmd, 
    (uint8_t)2, 
    (uint8_t)1, 
    (uint8_t)1
  );

  if (g_failure_reason) {
    return false;
  }

  // Datasheet says a write cycle takes 10ms.
  // Wait 5 more just in case...
  delay(15);

  return true;
}