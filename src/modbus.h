#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>

void getModbusCRC(uint8_t *data, uint16_t len, uint8_t *ret_crc1, uint8_t *ret_crc2);
int8_t checkModbusCRC(uint8_t *data, uint16_t len, uint8_t orig_crc1, uint8_t orig_crc2);

#endif
