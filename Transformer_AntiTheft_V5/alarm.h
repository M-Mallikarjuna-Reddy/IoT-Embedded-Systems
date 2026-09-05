#ifndef ALARM_H
#define ALARM_H

#include "config.h"
#include "globals.h"

//====================================================
// Alarm States
//====================================================

enum AlarmState
{
    ARMED,

    CALL_CONTACT,

    WAITING_FOR_CALL,

    COOLDOWN
};

//====================================================
// Globals
//====================================================

extern AlarmState alarmState;

extern int currentContact;

//====================================================
// Initialization
//====================================================

void setupAlarm();

//====================================================
// Main Loop
//====================================================

void loopAlarm();

//====================================================
// Movement Detection
//====================================================

void detectMovement();

//====================================================
// Alert Functions
//====================================================

void sendAlertSMS(float angle);

void processCallState();

//====================================================
// Manual Control
//====================================================

void triggerAlarm(float angle);

void resetAlarm();

bool isAlarmActive();

#endif