#include "sms.h"
#include "modem.h"
#include "gps.h"
#include "alarm.h"

//====================================================
// SMS Queue
//====================================================

struct SMSQueueItem
{
    String number;
    String message;
};

static SMSQueueItem smsQueue[5];

static int queueHead = 0;
static int queueTail = 0;

static bool smsBusy = false;

//====================================================
// Loop
//====================================================
//====================================================
// Setup
//====================================================

void setupSMS()
{
    Serial.println("SMS Module Ready");
}

void loopSMS()
{
    processSMSQueue();
}

//====================================================
// Queue SMS
//====================================================

void sendSMS(String message)
{
    sendSMS(PHONE_NUMBERS[0], message);
}

void sendSMS(String number, String message)
{
    int next =
        (queueTail + 1) % 5;

    if(next == queueHead)
    {
        Serial.println("SMS Queue Full");
        return;
    }

    smsQueue[queueTail].number = number;
    smsQueue[queueTail].message = message;

    queueTail = next;
}

//====================================================
// Queue Processor
//====================================================

void processSMSQueue()
{
    if(smsBusy)
        return;

    if(queueHead == queueTail)
        return;

    smsBusy = true;

    SMSQueueItem item =
        smsQueue[queueHead];

    queueHead =
        (queueHead + 1) % 5;

    Serial.println();
    Serial.println("==============================");
    Serial.println("Sending SMS");
    Serial.println("==============================");

    Serial.print("To : ");
    Serial.println(item.number);

    Serial.println(item.message);

    sendAT("AT+CMGF=1",1000);

    delay(300);

    modem.print("AT+CMGS=\"");
    modem.print(item.number);
    modem.println("\"");

    delay(800);

    modem.print(item.message);

    modem.write(26);

    unsigned long start = millis();

    while(millis()-start<6000)
    {
        processUART();

        if(atResponse.indexOf("OK")>=0)
            break;
    }

    Serial.println("SMS Sent");

    smsBusy = false;
}
//====================================================
// Process Incoming SMS
// Called ONLY from modem.cpp
//====================================================

void processSMS(String sender, String text)
{
    Serial.println();
    Serial.println("======================================");
    Serial.println("        PROCESSING SMS");
    Serial.println("======================================");

    Serial.print("Sender : ");
    Serial.println(sender);

    Serial.print("Text   : ");
    Serial.println(text);

    //--------------------------------------
    // Owner Verification
    //--------------------------------------

    if(sender != OWNER_NUMBER)
    {
        Serial.println("Unauthorized Number");

        sendSMS(sender,
                "ACCESS DENIED");

        return;
    }

    //--------------------------------------
    // Password Verification
    //--------------------------------------

    if(!text.startsWith(SMS_PASSWORD))
    {
        Serial.println("Wrong Password");

        sendSMS(sender,
                "WRONG PASSWORD");

        return;
    }

    //--------------------------------------
    // Remove Password
    //--------------------------------------

    text.remove(0, strlen(SMS_PASSWORD));

    text.trim();

    text.toUpperCase();

    Serial.print("Command : ");
    Serial.println(text);

    //--------------------------------------
    // STATUS
    //--------------------------------------

    if(text=="STATUS")
    {
        sendSMS(sender,getStatusReport());
        return;
    }

    //--------------------------------------
    // LOCATION
    //--------------------------------------

    if(text=="LOCATION")
    {
        if(gpsFix)
            sendSMS(sender,getGoogleMapsLink());
        else
            sendSMS(sender,"GPS NOT FIXED");

        return;
    }

    //--------------------------------------
    // MAP
    //--------------------------------------

    if(text=="MAP")
    {
        if(gpsFix)
            sendSMS(sender,getGoogleMapsLink());
        else
            sendSMS(sender,"GPS NOT FIXED");

        return;
    }

    //--------------------------------------
    // ARM
    //--------------------------------------

    if(text=="ARM")
    {
        systemArmed=true;

        sendSMS(sender,
                "SYSTEM ARMED");

        return;
    }

    //--------------------------------------
    // DISARM
    //--------------------------------------

    if(text=="DISARM")
    {
        systemArmed=false;

        sendSMS(sender,
                "SYSTEM DISARMED");

        return;
    }

    //--------------------------------------
    // SIGNAL
    //--------------------------------------

    if(text=="SIGNAL")
    {
        int signal=getSignalStrength();

        sendSMS(
            sender,
            "SIGNAL : "+String(signal)
        );

        return;
    }

    //--------------------------------------
    // BATTERY
    //--------------------------------------

    if(text=="BATTERY")
    {
        int battery=getBatteryLevel();

        sendSMS(
            sender,
            "BATTERY : "+
            String(battery)+"%"
        );

        return;
    }

    //--------------------------------------
    // PING
    //--------------------------------------

    if(text=="PING")
    {
        sendSMS(sender,"ONLINE");
        return;
    }

    //--------------------------------------
    // TEST
    //--------------------------------------

    if(text=="TEST")
    {
        sendSMS(sender,
                "DEVICE OK");
        return;
    }

    //--------------------------------------
    // REBOOT
    //--------------------------------------

    if(text=="REBOOT")
    {
        sendSMS(
            sender,
            "RESTARTING DEVICE..."
        );

        delay(1000);

        ESP.restart();

        return;
    }

    //--------------------------------------
    // HELP
    //--------------------------------------

    if(text=="HELP")
    {
        sendSMS(sender,
                getHelpReport());

        return;
    }

    //--------------------------------------
    // UNKNOWN COMMAND
    //--------------------------------------

    sendSMS(sender,
            "UNKNOWN COMMAND");
}
//====================================================
// Status Report
//====================================================

String getStatusReport()
{
    String msg = "";

    msg += "===========\n";
    msg += "TRANSFORMER STATUS\n";
    msg += "===========\n\n";

    msg += "ID : TR-001\n";

    msg += "System : ";
    msg += systemArmed ? "ARMED\n" : "DISARMED\n";

    msg += "Movement : ";
    msg += movementType;
    msg += "\n";

    msg += "Signal : ";
    msg += String(getSignalStrength());
    msg += "\n";

    msg += "Battery : ";
    msg += String(getBatteryLevel());
    msg += "%\n";

    msg += "GPS : ";

    if(gpsFix)
        msg += "FIXED\n";
    else
        msg += "NOT FIXED\n";

    if(gpsFix)
    {
        msg += "Latitude : ";
        msg += String(gpsLatitude,6);
        msg += "\n";

        msg += "Longitude : ";
        msg += String(gpsLongitude,6);
        msg += "\n";

        msg += getGoogleMapsLink();
    }

    return msg;
}

//====================================================
// Information Report
//====================================================

String getInfoReport()
{
    String msg;

    msg += "Transformer Anti Theft\n";
    msg += "Firmware : V6\n";
    msg += "MCU : ESP32-S3\n";
    msg += "Modem : SIM7670\n";
    msg += "GPS : GNSS\n";
    msg += "IMU : BNO085\n";

    return msg;
}

//====================================================
// Help Report
//====================================================

String getHelpReport()
{
    String msg;

    msg += "AVAILABLE COMMANDS\n\n";

    msg += "STATUS\n";
    msg += "LOCATION\n";
    msg += "MAP\n";
    msg += "SIGNAL\n";
    msg += "BATTERY\n";
    msg += "ARM\n";
    msg += "DISARM\n";
    msg += "PING\n";
    msg += "TEST\n";
    msg += "INFO\n";
    msg += "HELP\n";
    msg += "REBOOT\n";

    msg += "\n";

    msg += "Format:\n";
    msg += String(SMS_PASSWORD);
    msg += " STATUS";

    return msg;
}