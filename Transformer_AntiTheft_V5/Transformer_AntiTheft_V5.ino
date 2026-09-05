#include "config.h"
#include "globals.h"

#include "bno.h"
#include "modem.h"
#include "gps.h"
#include "sms.h"
#include "alarm.h"

//====================================================
// Setup
//====================================================

void setup()
{
    Serial.begin(115200);

    delay(2000);

    Serial.println();
    Serial.println("======================================");
    Serial.println(" Transformer Anti-Theft System ");
    Serial.println("======================================");

    //------------------------------------
    // I2C
    //------------------------------------

    Wire.begin(SDA_PIN, SCL_PIN);

    //------------------------------------
    // Modules
    //------------------------------------

    setupBNO();

    setupModem();

    setupGPS();

    setupSMS();

    setupAlarm();

    Serial.println();
    Serial.println("======================================");
    Serial.println(" SYSTEM READY ");
    Serial.println("======================================");
}

//====================================================
// Loop
//====================================================

void loop()
{
    //------------------------------------
    // Core Modules
    //------------------------------------

    loopModem();

    loopGPS();

    loopSMS();

    loopBNO();

    loopAlarm();

    //------------------------------------
    // Watchdog
    //------------------------------------

    modemWatchdog();
}