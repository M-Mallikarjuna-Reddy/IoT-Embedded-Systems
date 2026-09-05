#include "globals.h"

//====================================================
// Hardware
//====================================================

Adafruit_BNO08x bno08x(-1);

sh2_SensorValue_t sensorValue;

NimBLECharacteristic *pCharacteristic = nullptr;

//====================================================
// GPS
//====================================================

float gpsLatitude = 0.0f;
float gpsLongitude = 0.0f;

bool gpsFix = false;

//====================================================
// Accelerometer
//====================================================

float accelX = 0.0f;
float accelY = 0.0f;
float accelZ = 0.0f;

//====================================================
// Motion
//====================================================

String movementType = "";

//====================================================
// System
//====================================================

bool systemArmed = true;

//====================================================
// Timers
//====================================================

unsigned long lastGPSUpdate = 0;

unsigned long lastModemCheck = 0;

unsigned long alertTimer = 0;

unsigned long callStartTime = 0;

unsigned long lastCLCCCheck = 0;