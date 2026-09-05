#include "modem_parser.h"
#include "modem.h"
#include "sms.h"

String modemBuffer="";

void readModem()
{
    while(Serial1.available())
    {
        char c=Serial1.read();

        modemBuffer+=c;

        if(c!='\n')
            continue;

        modemBuffer.trim();

        if(modemBuffer.length()==0)
        {
            modemBuffer="";
            continue;
        }

        Serial.println();
        Serial.println("============= MODEM =============");
        Serial.println(modemBuffer);
        Serial.println("=================================");

        //---------------- SMS ----------------

        if(modemBuffer.startsWith("+CMT:"))
        {
            Serial.println("SMS EVENT");
        }

        //---------------- CALL ----------------

        else if(modemBuffer=="RING")
        {
            Serial.println("Incoming Call");
        }

        //---------------- Caller ----------------

        else if(modemBuffer.startsWith("+CLIP:"))
        {
            Serial.println("Caller ID");
        }

        //---------------- Network ----------------

        else if(modemBuffer.startsWith("+CREG"))
        {
            Serial.println("Network");
        }

        //---------------- Signal ----------------

        else if(modemBuffer.startsWith("+CSQ"))
        {
            Serial.println("Signal");
        }

        //---------------- Battery ----------------

        else if(modemBuffer.startsWith("+CBC"))
        {
            Serial.println("Battery");
        }

        modemBuffer="";
    }
}