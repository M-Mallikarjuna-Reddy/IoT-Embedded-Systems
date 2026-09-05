#include "alarm.h"
#include "bno.h"
#include "gps.h"
#include "sms.h"
#include "modem.h"

//====================================================
// Globals
//====================================================

AlarmState alarmState = ARMED;

int currentContact = 0;

static float currentAngle = 0.0;

static bool alarmTriggered = false;

static unsigned long movementTime = 0;

//====================================================
// Main Alarm Loop
//====================================================

void loopAlarm()
{
    if (!systemArmed)
        return;

    if (!getRotationAngle(currentAngle))
        return;

    detectMovement();

    switch (alarmState)
    {
        //------------------------------------------------
        // Armed
        //------------------------------------------------

        case ARMED:
        {
            if (!alarmTriggered)
                return;

            Serial.println();
            Serial.println("====================================");
            Serial.println("      TRANSFORMER THEFT");
            Serial.println("====================================");

            Serial.print("Movement : ");
            Serial.println(movementType);

            Serial.print("Angle : ");
            Serial.println(currentAngle);

            movementTime = millis();

            currentContact = 0;

            alarmState = CALL_CONTACT;

            break;
        }

        //------------------------------------------------
        // Call Contact
        //------------------------------------------------

        case CALL_CONTACT:
        {
            if(currentContact >= TOTAL_CONTACTS)
            {
                Serial.println("No Contacts Left");

                alertTimer = millis();

                alarmState = COOLDOWN;

                break;
            }

            Serial.println();
            Serial.println("Calling Contact");

            Serial.println(PHONE_NUMBERS[currentContact]);

            makeCall();

            sendAlertSMS(currentAngle);

            callStartTime = millis();

            alarmState = WAITING_FOR_CALL;

            break;
        }

        //------------------------------------------------
        // Waiting
        //------------------------------------------------

        case WAITING_FOR_CALL:
        {
            processCallState();
            break;
        }

        //------------------------------------------------
        // Cooldown
        //------------------------------------------------

        case COOLDOWN:
        {
            if(millis()-alertTimer >= ALERT_INTERVAL)
            {
                Serial.println("Alarm Rearmed");

                alarmTriggered = false;

                alarmState = ARMED;
            }

            break;
        }
    }
}
//====================================================
// Detect Movement
//====================================================

void detectMovement()
{
    alarmTriggered = false;

    //----------------------------------
    // Rotation
    //----------------------------------

    if(currentAngle >= ROTATION_LIMIT)
    {
        movementType = "ROTATION";

        alarmTriggered = true;

        return;
    }

    //----------------------------------
    // Lift
    //----------------------------------

    if(liftDetected)
    {
        movementType = "LIFT";

        alarmTriggered = true;

        return;
    }

    //----------------------------------
    // Drop
    //----------------------------------

    if(dropDetected)
    {
        movementType = "DROP";

        alarmTriggered = true;

        return;
    }

    //----------------------------------
    // Left
    //----------------------------------

    if(leftDetected)
    {
        movementType = "LEFT";

        alarmTriggered = true;

        return;
    }

    //----------------------------------
    // Right
    //----------------------------------

    if(rightDetected)
    {
        movementType = "RIGHT";

        alarmTriggered = true;

        return;
    }

    //----------------------------------
    // Forward
    //----------------------------------

    if(forwardDetected)
    {
        movementType = "FORWARD";

        alarmTriggered = true;

        return;
    }

    //----------------------------------
    // Backward
    //----------------------------------

    if(backwardDetected)
    {
        movementType = "BACKWARD";

        alarmTriggered = true;

        return;
    }
}

//====================================================
// Send Alert SMS
//====================================================

void sendAlertSMS(float angle)
{
    String msg = "";

    msg += "***************\n";
    msg += "THEFT ALERT\n";
    msg += "***************\n\n";

    msg += "ID : TR-001\n";

    msg += "Movement : ";
    msg += movementType;
    msg += "\n";

    msg += "Angle : ";
    msg += String(angle,1);
    msg += " deg\n";

    msg += "Signal : ";
    msg += String(getSignalStrength());
    msg += "\n";

    msg += "Battery : ";
    msg += String(getBatteryLevel());
    msg += "%\n";

    msg += "GPS : ";

    if(gpsFix)
    {
        msg += "FIXED\n";
        msg += getGoogleMapsLink();
    }
    else
    {
        msg += "NOT FIXED";
    }

    Serial.println();
    Serial.println("==============================");
    Serial.println("Sending Alert SMS");
    Serial.println("==============================");

    Serial.println(msg);

    sendSMS(
        PHONE_NUMBERS[currentContact],
        msg
    );
}