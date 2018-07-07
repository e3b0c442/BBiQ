#include <Arduino.h>
#include "crc.h"

byte crcCompute(byte* data, int len) {
    byte crc = 0;
    for(int i = 0; i < len; i++) {
        crc = pgm_read_byte(&CRC_TABLE[data[i] ^ crc]);
    }
    return crc;
}

bool crcVerify(byte crc, byte* data, int len) {
    byte dataCRC = crcCompute(data, len);
    return dataCRC == crc;
}