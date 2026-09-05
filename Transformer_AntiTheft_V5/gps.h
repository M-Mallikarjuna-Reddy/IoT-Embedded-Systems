#ifndef GPS_H
#define GPS_H

#include "config.h"
#include "globals.h"

//====================================================
// Initialization
//====================================================

void setupGPS();

//====================================================
// Main Loop
//====================================================

void loopGPS();

//====================================================
// GPS Functions
//====================================================

void requestGPSUpdate();

bool parseGPSResponse(String response);

String getGoogleMapsLink();

void printGPSStatus();

#endif