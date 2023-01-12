#ifndef EEP_24WC16_H
#define EEP_24WC16_H

#define E_TWI_NOT_INITIALIZED 5
#define E_ADDR_OUT_OF_BOUNDS 6
#define E_TWI_READ_FAILED 7
#define E_BURST_TOO_LONG 8

#define EEP_24WC16_CHIP_SIZE 2048
#define EEP_24WC16_PAGE_SIZE 256

#ifdef __cplusplus
extern "C" {
#endif

void EEP_24WC16_Initialize(void);
uint8_t EEP_24WC16_GetFailureReason(void);

bool EEP_24WC16_ReadByte(uint16_t addr, uint8_t* value);

bool EEP_24WC16_WriteByte(uint16_t addr, uint8_t value);
bool EEP_24WC16_WriteBurst(uint16_t addr, uint8_t* values, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif