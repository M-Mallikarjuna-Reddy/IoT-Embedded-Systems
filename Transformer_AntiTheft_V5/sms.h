#ifndef SMS_H
#define SMS_H

#include <Arduino.h>

#include "config.h"
#include "globals.h"

//====================================================
// Initialization
//====================================================

void setupSMS();

//====================================================
// Main Loop
//====================================================

void loopSMS();

//====================================================
// SMS Queue
//====================================================

void processSMSQueue();

//====================================================
// SMS Processing
//====================================================

void processSMS(String sender, String text);

//====================================================
// Send SMS
//====================================================

void sendSMS(String number, String message);

void sendSMS(String message);

//====================================================
// Reports
//====================================================

String getStatusReport();

String getInfoReport();

String getHelpReport();

#endif