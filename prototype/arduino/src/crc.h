#ifndef CRC_E3B0C442_H
#define CRC_E3B0C442_H

#include <Arduino.h>

byte crcCompute(byte *data, int len);
bool crcVerify(byte crc, byte *data, int len);

#endif // CRC_E3B0C442_H