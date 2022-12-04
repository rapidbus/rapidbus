#include "modbus.h"
#include <stdio.h> /* Standard input/output definitions */

int8_t checkModbusCRC(uint8_t *data, uint16_t len, uint8_t orig_crc1, uint8_t orig_crc2) {
  uint16_t crc2 = 0xffff;
  for (uint16_t i = 0; i < len; i++) {
    crc2 ^= (0xff & data[i]);
    for (uint8_t a = 0; a < 8; a++) {
      if (crc2 & 0x0001) {
        crc2 = ((crc2 >> 1) & 0xffff) ^ 0xA001;
      } else {
        crc2 = ((crc2 >> 1) & 0xffff);
      }
    }
  }
  crc2 = crc2 & 0xffff;
  if ((orig_crc1 == (crc2 & 0xff)) && (orig_crc2 == (crc2 >> 8))) {
    // printf("CRC correct!\n");
    return 1;
  } else {
    printf("CRC not correct. Calculated:Packet %x:%x %x:%x\n", crc2 & 0xff, orig_crc1, crc2 >> 8,
           orig_crc2);
    return 0;
  };
}
