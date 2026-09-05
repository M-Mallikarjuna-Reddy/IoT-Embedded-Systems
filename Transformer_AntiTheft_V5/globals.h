#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_BNO08x.h>
#include <NimBLEDevice.h>

#include "config.h"

//====================================================
// Hardware
//====================================================

extern Adafruit_BNO08x bno08x;
extern sh2_SensorValue_t sensorValue;

extern NimBLECharacteristic *pCharacteristic;

//====================================================
// GPS
//====================================================

extern float gpsLatitude;
extern float gpsLongitude;

extern bool gpsFix;

//====================================================
// Accelerometer
//====================================================

extern float accelX;
extern float accelY;
extern float accelZ;

//====================================================
// Motion
//====================================================

extern String movementType;

//====================================================
// System
//====================================================

extern bool systemArmed;

//====================================================
// Timers
//====================================================

extern unsigned long lastGPSUpdate;

extern unsigned long lastModemCheck;

extern unsigned long alertTimer;

extern unsigned long callStartTime;

extern unsigned long lastCLCCCheck;

#endif