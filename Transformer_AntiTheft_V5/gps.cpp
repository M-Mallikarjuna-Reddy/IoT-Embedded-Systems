#include "gps.h"
#include "modem.h"

unsigned long lastGPSRead = 0;

//====================================================
// Setup
//====================================================

void setupGPS()
{
    Serial.println();
    Serial.println("==================================");
    Serial.println("        GPS INITIALIZATION");
    Serial.println("==================================");

    gpsFix = false;

    gpsLatitude = 0.0;
    gpsLongitude = 0.0;

    enableGPS();

    Serial.println("GPS READY");
}

//====================================================
// Loop
//====================================================

void loopGPS()
{
    if(!gpsEnabled)
        return;

    if(millis()-lastGPSRead<GPS_UPDATE_INTERVAL)
        return;

    lastGPSRead=millis();

    requestGPSUpdate();
}

//====================================================
// Request GPS
//====================================================

void requestGPSUpdate()
{
    sendAT("AT+CGNSSINFO",3000);
}

//====================================================
// Parse GPS Response
//====================================================

bool parseGPSResponse(String response)
{
    response.replace("\r","");
    response.replace("\n","");

    int pos=response.indexOf("+CGNSSINFO:");

    if(pos<0)
        return false;

    response=response.substring(pos+11);

    response.trim();

    String field[20];

    int index=0;

    while(response.length()>0 && index<20)
    {
        int comma=response.indexOf(',');

        if(comma<0)
        {
            field[index++]=response;
            break;
        }

        field[index++]=response.substring(0,comma);

        response.remove(0,comma+1);
    }

    //----------------------------------
    // Fix
    //----------------------------------

    if(field[0]!="2" && field[0]!="3")
    {
        gpsFix=false;

        Serial.println("GPS : NO FIX");

        return false;
    }

    gpsFix=true;

    //----------------------------------
    // Latitude
    //----------------------------------

    gpsLatitude=field[5].toFloat();

    if(field[6]=="S")
        gpsLatitude*=-1;

    //----------------------------------
    // Longitude
    //----------------------------------

    gpsLongitude=field[7].toFloat();

    if(field[8]=="W")
        gpsLongitude*=-1;
    //----------------------------------
    // Debug Output
    //----------------------------------

    Serial.println();
    Serial.println("==================================");
    Serial.println("           GPS FIX");
    Serial.println("==================================");

    Serial.print("Latitude  : ");
    Serial.println(gpsLatitude, 6);

    Serial.print("Longitude : ");
    Serial.println(gpsLongitude, 6);

    Serial.print("Fix Type  : ");
    Serial.println(field[0]);

    Serial.print("Date      : ");
    Serial.println(field[9]);

    Serial.print("Time      : ");
    Serial.println(field[10]);

    Serial.print("Google Map: ");
    Serial.println(getGoogleMapsLink());

    Serial.println("==================================");

    return true;
}

//====================================================
// Google Maps Link
//====================================================

String getGoogleMapsLink()
{
    if (!gpsFix)
        return "GPS NOT FIXED";

    String url = "https://maps.google.com/?q=";

    url += String(gpsLatitude, 6);
    url += ",";
    url += String(gpsLongitude, 6);

    return url;
}

//====================================================
// Print GPS Status
//====================================================

void printGPSStatus()
{
    Serial.println();
    Serial.println("============= GPS STATUS =============");

    Serial.print("GPS Enabled : ");
    Serial.println(gpsEnabled ? "YES" : "NO");

    Serial.print("GPS Fix     : ");
    Serial.println(gpsFix ? "YES" : "NO");

    if (gpsFix)
    {
        Serial.print("Latitude    : ");
        Serial.println(gpsLatitude, 6);

        Serial.print("Longitude   : ");
        Serial.println(gpsLongitude, 6);

        Serial.print("Map         : ");
        Serial.println(getGoogleMapsLink());
    }

    Serial.println("======================================");
}