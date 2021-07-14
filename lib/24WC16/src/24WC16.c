#include <Arduino.h>
#include <utility/twi.h>

#include "24WC16.h"

static uint8_t g_failure_reason = 0;

static uint8_t EEP_24WC16_MakePhysicalAddress(uint16_t linear_addr, bool write) {
  uint8_t i2c_addr = 0;
  i2c_addr |= (uint8_t)(5 << 4); // hardwired x1010xxx

  if (!write) {
    i2c_addr |= 1 << 7;
  }

  i2c_addr |= (linear_addr / EEP_24WC16_PAGE_SIZE) % 8;
  return i2c_addr;
}

uint8_t EEP_24WC16_GetFailureReason(void) {
  return g_failure_reason;
}

// Attempts to read a byte from the
// provided linear (0-2047) address.
bool EEP_24WC16_ReadByte(uint16_t addr, uint8_t* value) {
  if (!TWCR) {
    g_failure_reason = E_TWI_NOT_INITIALIZED;
    return false;
  }

  if (addr >= EEP_24WC16_CHIP_SIZE) {
    g_failure_reason = E_ADDR_OUT_OF_BOUNDS;
    return false;
  }

  uint8_t i2c_addr_sel = EEP_24WC16_MakePhysicalAddress(addr, true);
  uint8_t i2c_addr_read = EEP_24WC16_MakePhysicalAddress(addr, false);
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

  uint8_t read_len = twi_readFrom(
    i2c_addr_read,
    value,
    (uint8_t)1,
    (uint8_t)1
  );

  if (!read_len) {
    g_failure_reason = E_TWI_READ_FAILED;
    return false;
  }

  return true;
}

// Attempts to read an immediate byte
// using chip's internal address counter.
bool EEP_24WC16_ReadImmediate(uint8_t* value) {
  if (!TWCR) {
    g_failure_reason = E_TWI_NOT_INITIALIZED;
    return false;
  }

  uint8_t i2c_addr_read = EEP_24WC16_MakePhysicalAddress(0, false);

  uint8_t read_len = twi_readFrom(
    i2c_addr_read,
    value,
    (uint8_t)1,
    (uint8_t)1
  );

  if (!read_len) {
    g_failure_reason = E_TWI_READ_FAILED;
    return false;
  }

  return true;
}

// Attempts to read a sequence of bytes in one go
// starting at the provided address.
bool EEP_24WC16_ReadSequential(uint16_t addr, uint8_t* buffer, uint16_t len) {
  if (!TWCR) {
    g_failure_reason = E_TWI_NOT_INITIALIZED;
    return false;
  }

  if (addr >= EEP_24WC16_CHIP_SIZE) {
    g_failure_reason = E_ADDR_OUT_OF_BOUNDS;
    return false;
  }

  if (len >= EEP_24WC16_CHIP_SIZE) {
    g_failure_reason = E_BURST_TOO_LONG;
    return false;
  }

  uint8_t i2c_addr_sel = EEP_24WC16_MakePhysicalAddress(addr, true);
  uint8_t i2c_addr_read = EEP_24WC16_MakePhysicalAddress(addr, false);
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

  uint8_t val;
  for (uint16_t i = 0; i < len; i++) {
    uint8_t read_len = twi_readFrom(
      i2c_addr_read,
      &val,
      (uint8_t)1,
      (uint8_t)1
    );

    if (!read_len) {
      g_failure_reason = E_TWI_READ_FAILED;
      return false;
    }

    buffer[i] = val;
  }
  return true;
}

// Attempts to write a single byte to the 
// provided linear (0-2047) byte address.
bool EEP_24WC16_WriteByte(uint16_t addr, uint8_t value) {
  if (!TWCR) {
    g_failure_reason = E_TWI_NOT_INITIALIZED;
    return false;
  }

  if (addr >= EEP_24WC16_CHIP_SIZE) {
    g_failure_reason = E_ADDR_OUT_OF_BOUNDS;
    return false;
  }

  uint8_t i2c_addr_wr = EEP_24WC16_MakePhysicalAddress(addr, true);
  uint8_t addr_in_page = (uint8_t)(addr % EEP_24WC16_PAGE_SIZE);

  uint8_t cmd[2] = { addr_in_page, value };

  g_failure_reason = twi_writeTo(
    i2c_addr_wr,
    &cmd[0], 
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

// Attempts to write a block of data
// using the chip's page write feature
// starting at the provided linear address.
bool EEP_24WC16_WriteBurst(uint16_t addr, uint8_t* buffer, uint8_t len) {
  if (!TWCR) {
    g_failure_reason = E_TWI_NOT_INITIALIZED;
    return false;
  }

  if (addr >= EEP_24WC16_CHIP_SIZE) {
    g_failure_reason = E_ADDR_OUT_OF_BOUNDS;
    return false;
  }

  if (len > 16) {
    g_failure_reason = E_BURST_TOO_LONG;
    return false;
  }

  uint8_t i2c_addr_wr = EEP_24WC16_MakePhysicalAddress(addr, true);
  uint8_t addr_in_page = (uint8_t)(addr % EEP_24WC16_PAGE_SIZE);

  uint8_t cmd[17] = { 0 };
  cmd[0] = addr_in_page;
  memcpy(cmd + 1, buffer, len);

  g_failure_reason = twi_writeTo(
    i2c_addr_wr,
    &cmd[0], 
    len + 1, 
    (uint8_t)1, 
    (uint8_t)1
  );

  if (g_failure_reason) {
    return false;
  }

  delay(15);
  
  return true;
}