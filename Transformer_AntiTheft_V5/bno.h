#ifndef BNO_H
#define BNO_H

#include "config.h"
#include "globals.h"

//====================================================
// Initialization
//====================================================

void setupBNO();
void enableReports();

//====================================================
// Main Loop
//====================================================

void loopBNO();

//====================================================
// Rotation
//====================================================

bool getRotationAngle(float &angle);

//====================================================
// Motion Detection Flags
//====================================================

extern bool liftDetected;
extern bool dropDetected;

extern bool leftDetected;
extern bool rightDetected;

extern bool forwardDetected;
extern bool backwardDetected;

//====================================================
// Reference Quaternion
//====================================================

extern bool referenceSaved;

extern float refW;
extern float refX;
extern float refY;
extern float refZ;

#endif