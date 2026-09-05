#ifndef MODEM_H
#define MODEM_H

#include <Arduino.h>

//==================================================
// UART
//==================================================

#define MODEM_SERIAL Serial1

//==================================================
// Event Types
//==================================================

enum ModemEventType
{
    EVENT_NONE,
    EVENT_AT_RESPONSE,
    EVENT_SMS,
    EVENT_CALL,
    EVENT_CALLER_ID,
    EVENT_NETWORK,
    EVENT_GPS,
    EVENT_SIGNAL,
    EVENT_BATTERY,
    EVENT_RING,
    EVENT_OK,
    EVENT_ERROR
};

//==================================================
// Call Status
//==================================================

enum CallStatus
{
    CALL_NONE,
    CALL_DIALING,
    CALL_ALERTING,
    CALL_ACTIVE,
    CALL_HELD,
    CALL_INCOMING,
    CALL_WAITING,
    CALL_DISCONNECTED
};

//==================================================
// Modem State
//==================================================

enum ModemState
{
    MODEM_IDLE,
    MODEM_BUSY,
    MODEM_WAITING_RESPONSE,
    MODEM_SMS,
    MODEM_CALL
};

//==================================================
// Event Structure
//==================================================

struct ModemEvent
{
    ModemEventType type;
    String line;
    String number;
    String text;
};

//==================================================
// Globals
//==================================================

extern ModemState modemState;
extern String modemBuffer;
extern String atResponse;

extern bool responseReady;
extern bool gpsEnabled;

//==================================================
// Initialization
//==================================================

void setupModem();

bool initModem();

void loopModem();

void recoverModem();

void modemWatchdog();

void printModemStatus();

//==================================================
// UART Parser
//==================================================

void processUART();

void parseLine(String line);

void handleSMS(String line);

void handleCall(String line);

void handleGPS(String line);

void handleNetwork(String line);

//==================================================
// AT Commands
//==================================================

bool sendAT(
    String cmd,
    uint32_t timeout = 3000
);

void sendATNoWait(String cmd);

bool waitForResponse(
    uint32_t timeout
);

//==================================================
// Call
//==================================================

CallStatus getCallStatus();

void makeCall();

void answerCall();

void rejectCall();

void hangUp();

void checkIncomingCall();

//==================================================
// SMS
//==================================================

void sendSMS(
    String number,
    String message
);

void sendSMS(
    String message
);

//==================================================
// Utilities
//==================================================

bool checkModem();

int getSignalStrength();

int getBatteryLevel();

//==================================================
// GPS
//==================================================

void enableGPS();

void disableGPS();

void updateGPS();

#endif