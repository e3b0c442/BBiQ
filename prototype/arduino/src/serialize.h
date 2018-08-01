/**
 * serialize.h
 * Copyright (c) 2018 Nick Meyer. All rights reserved.
 * 
 * Handles message serialization to be passed to the serial interface.
 * 
 */

#ifndef SERIALIZE_E3B0C442_H
#define SERIALIZE_E3B0C442_H

#include <Arduino.h>

void serializeProbeTempEvent(byte probeID, float value, byte *buf);
void serializeProbeConnectionEvent(byte probeID);
void serializeProbeSetEvent(byte probeID, bool lowHigh, float setpoint, byte *buf);

#endif // SERIALIZE_E3B0C442_H