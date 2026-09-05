#include <Wire.h>
#include <Adafruit_BNO08x.h>

#define SDA_PIN         15
#define SCL_PIN         16
#define BNO_ADDR        0x4B

#define MODEM_RX        17
#define MODEM_TX        18
#define MODEM_BAUD      115200

#define ROTATION_LIMIT  5.0
#define ALERT_INTERVAL  20000UL

//========================
// SECURITY CONFIGURATION
//========================
#define SMS_PASSWORD    "1234"  // Your 4-digit security password

const char* PHONE_NUMBERS[] =
{
    "+919550782008",
    "+919912462008",
    "+919346127789",
    "+918247477874"
};

const int TOTAL_CONTACTS = sizeof(PHONE_NUMBERS) / sizeof(PHONE_NUMBERS[0]);
int currentContact = 0;

const unsigned long CALL_TIMEOUT = 30000UL;
unsigned long callStartTime = 0;
unsigned long lastCLCCCheck = 0;

//========================
// GPS
//========================
float gpsLatitude = 0.0;
float gpsLongitude = 0.0;
bool gpsFix = false;

bool enableGPS();
bool updateGPS();
String getGoogleMapsLink();

Adafruit_BNO08x bno08x(-1);
sh2_SensorValue_t sensorValue;

//========================
// BNO085
//========================
float refW, refX, refY, refZ;
bool referenceSaved = false;
String movementType = "UNKNOWN";
int batteryPercent = 0;
int signalStrength = 0;

float accelX = 0;
float accelY = 0;
float accelZ = 0;

bool liftDetected = false;
bool dropDetected = false;
bool leftDetected = false;
bool rightDetected = false;
bool forwardDetected = false;
bool backwardDetected = false;

#define LIFT_THRESHOLD      2.5
#define DROP_THRESHOLD     -2.5
#define LEFT_THRESHOLD      2.0
#define RIGHT_THRESHOLD    -2.0
#define FORWARD_THRESHOLD   2.0
#define BACKWARD_THRESHOLD -2.0

//========================
// Alert State Machine
//========================
enum AlarmState
{
    DISARMED,
    ARMED,
    CALL_AND_SMS_CONTACT,
    WAITING_FOR_CALL,
    COOLDOWN
};

AlarmState alarmState = ARMED;

unsigned long alertTimer = 0;
unsigned long lastModemCheck = 0;
const unsigned long MODEM_CHECK_INTERVAL = 60000;

//========================
// Function Prototypes
//========================
void enableReports();
bool getRotationAngle(float &angle);
String sendAT(String cmd, unsigned long timeout = 3000);
bool initModem();
void checkIncomingSMS();
bool isWhitelisted(String incomingNumber, int &matchedIndex);
void sendReplySMS(String targetNumber, String message);

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

CallStatus getCallStatus();
void makeCall();
void hangUp();
void sendSMS(String message);
int getBatteryLevel();
int getSignalStrength();

void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    delay(1000);

    Serial.println("\nStarting BNO085...");
    while (!bno08x.begin_I2C(BNO_ADDR, &Wire))
    {
        Serial.println("Waiting for BNO085...");
        delay(1000);
    }
    Serial.println("BNO085 Connected");

    enableReports();

    Serial.println("Starting Modem...");
    Serial1.begin(MODEM_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);

    while (!initModem())
    {
        Serial.println("Waiting for Modem...");
        delay(2000);
    }

    Serial.println("\n===============================");
    Serial.println(" Interactive Anti-Theft Device v5.1");
    Serial.println("===============================");
    enableGPS();
    Serial.println("State   : ARMED");
    Serial.println("===============================");
}

void enableReports()
{
    bno08x.enableReport(SH2_GAME_ROTATION_VECTOR);
    bno08x.enableReport(SH2_LINEAR_ACCELERATION);
}

bool getRotationAngle(float &angle)
{
    if (bno08x.wasReset())
    {
        enableReports();
        referenceSaved = false;
    }

    bool newEventReceived = false;
    while (bno08x.getSensorEvent(&sensorValue))
    {
        if(sensorValue.sensorId == SH2_LINEAR_ACCELERATION)
        {
            accelX = sensorValue.un.linearAcceleration.x;
            accelY = sensorValue.un.linearAcceleration.y;
            accelZ = sensorValue.un.linearAcceleration.z;

            liftDetected      = (accelZ > LIFT_THRESHOLD);
            dropDetected      = (accelZ < DROP_THRESHOLD);
            leftDetected      = (accelX > LEFT_THRESHOLD);
            rightDetected     = (accelX < RIGHT_THRESHOLD);
            forwardDetected   = (accelY > FORWARD_THRESHOLD);
            backwardDetected  = (accelY < BACKWARD_THRESHOLD);
            newEventReceived = true;
        }

        if(sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR)
        {
            float w = sensorValue.un.gameRotationVector.real;
            float x = sensorValue.un.gameRotationVector.i;
            float y = sensorValue.un.gameRotationVector.j;
            float z = sensorValue.un.gameRotationVector.k;

            if(!referenceSaved)
            {
                refW = w; refX = x; refY = y; refZ = z;
                referenceSaved = true;
                return false;
            }

            float dot = refW*w + refX*x + refY*y + refZ*z;
            dot = fabs(dot);
            if(dot > 1.0) dot = 1.0;

            angle = 2.0 * acos(dot) * 180.0 / PI;
            newEventReceived = true;
        }
    }
    return newEventReceived;
}

String sendAT(String cmd, unsigned long timeout)
{
    while (Serial1.available()) Serial1.read();
    Serial1.println(cmd);
    String response = "";
    unsigned long start = millis();
    while (millis() - start < timeout)
    {
        while (Serial1.available()) response += (char)Serial1.read();
    }
    return response;
}

bool initModem()
{
    if (sendAT("AT").indexOf("OK") < 0) return false;
    sendAT("ATE0");      
    sendAT("AT+CMGF=1"); 
    sendAT("AT+CNMI=2,2,0,0,0"); 
    return true;
}

bool enableGPS()
{
    sendAT("AT+CGNSSPWR=1",3000);
    delay(1000);
    sendAT("AT+CGNSSMODE=1",1000);
    return true;
}

bool updateGPS()
{
    String response = sendAT("AT+CGNSSINFO", 1000);
    if (response.indexOf(",,,,") >= 0 || response.indexOf("+CGNSSINFO:") < 0)
    {
        gpsFix = false;
        return false;
    }
    response.replace("\r",""); response.replace("\n","");
    int pos = response.indexOf("+CGNSSINFO:");
    response = response.substring(pos + 11);

    String field[20];
    int index = 0;
    while(response.length() && index < 20)
    {
        int comma = response.indexOf(',');
        if(comma < 0) { field[index++] = response; break; }
        field[index++] = response.substring(0,comma);
        response.remove(0,comma+1);
    }
    if(index < 7) return false;

    String lat = field[4]; String ns  = field[5];
    if(lat.length() >= 4)
    {
        float deg = lat.substring(0,2).toFloat();
        float min = lat.substring(2).toFloat();
        gpsLatitude = deg + (min/60.0);
        if(ns=="S") gpsLatitude *= -1;
    }

    String lon = field[6]; String ew  = field[7];
    if(lon.length() >= 5)
    {
        float deg = lon.substring(0,3).toFloat();
        float min = lon.substring(3).toFloat();
        gpsLongitude = deg + (min/60.0);
        if(ew=="W") gpsLongitude *= -1;
    }
    gpsFix = true;
    return true;
}

String getGoogleMapsLink()
{
    if(!gpsFix) return "GPS Searching...";
    return "https://maps.google.com/?q=" + String(gpsLatitude,6) + "," + String(gpsLongitude,6);
}

CallStatus getCallStatus()
{
    String response = sendAT("AT+CLCC", 300);
    if (response.indexOf("+CLCC:") < 0) return CALL_NONE;
    if (response.indexOf(",0,0,0") >= 0) return CALL_ACTIVE;
    if (response.indexOf(",0,2,0") >= 0) return CALL_DIALING;
    if (response.indexOf(",0,3,0") >= 0) return CALL_ALERTING;
    return CALL_DISCONNECTED;
}

void makeCall()
{
    String cmd = "ATD" + String(PHONE_NUMBERS[currentContact]) + ";";
    sendAT(cmd, 500);
    callStartTime = millis();
}

void hangUp()
{
    sendAT("ATH");
}

void sendSMS(String message)
{
    sendReplySMS(PHONE_NUMBERS[currentContact], message);
}

void sendReplySMS(String targetNumber, String message)
{
    sendAT("AT+CMGF=1");
    while (Serial1.available()) Serial1.read();
    Serial1.print("AT+CMGS=\"");
    Serial1.print(targetNumber);
    Serial1.println("\"");
    delay(500);
    Serial1.print(message);
    delay(200);
    Serial1.write(26);
    delay(2000);
}

bool isWhitelisted(String incomingNumber, int &matchedIndex)
{
    for(int i = 0; i < TOTAL_CONTACTS; i++)
    {
        String cleanContact = String(PHONE_NUMBERS[i]);
        cleanContact.replace("+","");
        String cleanIncoming = incomingNumber;
        cleanIncoming.replace("+","");
        cleanIncoming.replace("\"","");

        if(cleanIncoming.indexOf(cleanContact) >= 0 || cleanContact.indexOf(cleanIncoming) >= 0)
        {
            matchedIndex = i;
            return true;
        }
    }
    return false;
}

void checkIncomingSMS()
{
    if (Serial1.available())
    {
        String incoming = Serial1.readString();
        int cmtPos = incoming.indexOf("+CMT:");
        if (cmtPos >= 0)
        {
            int firstQuote = incoming.indexOf("\"", cmtPos);
            int secondQuote = incoming.indexOf("\"", firstQuote + 1);
            String senderNumber = incoming.substring(firstQuote + 1, secondQuote);

            int matchedIndex = -1;
            if (!isWhitelisted(senderNumber, matchedIndex))
            {
                Serial.println("Rejected: Sender not in Contact List.");
                return;
            }

            int nxl = incoming.indexOf("\n", secondQuote);
            String smsBody = incoming.substring(nxl + 1);
            smsBody.trim();
            smsBody.toUpperCase();

            // 1. HELP (No Password Required)
            if (smsBody == "HELP")
            {
                String helpMsg = "Commands:\nHELP\nPING\nSTATUS [pwd]\nGPS [pwd]\nMAP [pwd]\nARM [pwd]\nDISARM [pwd]\nRESET [pwd]\nREBOOT [pwd]";
                sendReplySMS(senderNumber, helpMsg);
                return;
            }

            // 2. PING (No Password Required)
            if (smsBody == "PING")
            {
                sendReplySMS(senderNumber, "PONG");
                return;
            }

            // Password Check Engine for remaining functions
            if (smsBody.indexOf(SMS_PASSWORD) < 0)
            {
                Serial.println("Rejected: Password missing or wrong.");
                return;
            }

            // 3. STATUS
            if (smsBody.indexOf("STATUS") >= 0)
            {
                updateGPS();
                String statusMsg = "Status:\nMode: ";
                statusMsg += (alarmState == ARMED) ? "ARMED" : (alarmState == DISARMED) ? "DISARMED" : "ALERTING";
                statusMsg += "\nSignal: " + String(getSignalStrength()) + "/31";
                statusMsg += "\nBattery: " + String(getBatteryLevel()) + "%";
                statusMsg += "\nGPS: " + String(gpsFix ? "FIXED" : "NO FIX");
                sendReplySMS(senderNumber, statusMsg);
            }
            // 4. GPS
            else if (smsBody.indexOf("GPS") >= 0)
            {
                updateGPS();
                String gpsMsg = "GPS Coordinates:\n";
                if(gpsFix) {
                    gpsMsg += "Lat: " + String(gpsLatitude, 6) + "\nLon: " + String(gpsLongitude, 6);
                } else {
                    gpsMsg += "Searching satellites...";
                }
                sendReplySMS(senderNumber, gpsMsg);
            }
            // 5. MAP
            else if (smsBody.indexOf("MAP") >= 0)
            {
                updateGPS();
                sendReplySMS(senderNumber, "Google Map Link:\n" + getGoogleMapsLink());
            }
            // 6. ARM
            else if (smsBody.indexOf("ARM") >= 0)
            {
                alarmState = ARMED;
                referenceSaved = false;
                sendReplySMS(senderNumber, "System setup: ARMED successfully.");
            }
            // 7. DISARM
            else if (smsBody.indexOf("DISARM") >= 0)
            {
                alarmState = DISARMED;
                hangUp();
                sendReplySMS(senderNumber, "System setup: DISARMED successfully.");
            }
            // 8. RESET
            else if (smsBody.indexOf("RESET") >= 0)
            {
                hangUp();
                referenceSaved = false;
                alarmState = ARMED;
                sendReplySMS(senderNumber, "Alarm state reset. System RE-ARMED.");
            }
            // 9. REBOOT
            else if (smsBody.indexOf("REBOOT") >= 0)
            {
                sendReplySMS(senderNumber, "Rebooting ESP32 device...");
                delay(1000);
                ESP.restart(); 
            }
        }
    }
}

int getSignalStrength()
{
    String response = sendAT("AT+CSQ",1000);
    int index = response.indexOf("+CSQ:");
    if(index < 0) return -1;
    return response.substring(index + 6, response.indexOf(",", index)).toInt();
}

int getBatteryLevel()
{
    String response = sendAT("AT+CBC",1000);
    int first = response.lastIndexOf(",");
    if(first < 0) return -1;
    return response.substring(first + 1).toInt();
}

bool checkModem()
{
    return (sendAT("AT", 1000).indexOf("OK") >= 0);
}

void recoverModem()
{
    Serial1.end();
    delay(1000);
    Serial1.begin(MODEM_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);
    while(!initModem()) delay(2000);
    enableGPS();
    gpsFix = false;
}

void loop()
{
    static unsigned long lastGPSUpdate = 0;
    if(millis() - lastGPSUpdate > 5000)
    {
        lastGPSUpdate = millis();
        updateGPS();
    }

    checkIncomingSMS();

    float angle = 0.0;
    static float lastValidAngle = 0.0;
    if (getRotationAngle(angle)) 
    {
        lastValidAngle = angle;
    }

    switch (alarmState)
    {
        case DISARMED:
            break;

        case ARMED:
            if (lastValidAngle >= ROTATION_LIMIT || liftDetected || dropDetected || 
                leftDetected || rightDetected || forwardDetected || backwardDetected)
            {
                if(lastValidAngle >= ROTATION_LIMIT)   movementType = "ROTATION";
                else if(liftDetected)              movementType = "LIFT";
                else if(dropDetected)              movementType = "DROP";
                else if(leftDetected)              movementType = "LEFT";
                else if(rightDetected)             movementType = "RIGHT";
                else if(forwardDetected)           movementType = "FORWARD";
                else if(backwardDetected)          movementType = "BACKWARD";

                currentContact = 0; 
                alarmState = CALL_AND_SMS_CONTACT;
            }
            break;

        case CALL_AND_SMS_CONTACT:
        { // Braces added here to safely isolate scope and fix compile error
            liftDetected = false; dropDetected = false;
            leftDetected = false; rightDetected = false;
            forwardDetected = false; backwardDetected = false;

            makeCall(); 
            delay(500); 
            
            String msg;
            msg = "TRANSFORMER THEFT ALERT\nID:TR-001\n";
            msg += "Move: " + movementType + "\n";
            msg += "Angle: " + String(lastValidAngle, 1) + " deg\n";
            msg += getGoogleMapsLink();
            
            sendSMS(msg); 
            alarmState = WAITING_FOR_CALL;
            break;
        }

        case WAITING_FOR_CALL:
            if (millis() - lastCLCCCheck >= 1000)
            {
                lastCLCCCheck = millis();
                CallStatus status = getCallStatus();

                if (status == CALL_ACTIVE)
                {
                    alertTimer = millis();
                    alarmState = COOLDOWN; 
                }
                else if (status == CALL_NONE || status == CALL_DISCONNECTED)
                {
                    currentContact++;
                    if (currentContact >= TOTAL_CONTACTS) currentContact = 0; 
                    alarmState = CALL_AND_SMS_CONTACT;
                }
            }

            if (millis() - callStartTime >= CALL_TIMEOUT)
            {
                hangUp();
                currentContact++;
                if (currentContact >= TOTAL_CONTACTS) currentContact = 0;
                alarmState = CALL_AND_SMS_CONTACT;
            }
            break;

        case COOLDOWN:
            if (millis() - alertTimer >= ALERT_INTERVAL)
            {
                if (lastValidAngle >= ROTATION_LIMIT)
                {
                    currentContact = 0;
                    alarmState = CALL_AND_SMS_CONTACT;
                }
                else
                {
                    alarmState = ARMED;
                }
            }
            break;
    }

    if (millis() - lastModemCheck >= MODEM_CHECK_INTERVAL)
    {
        lastModemCheck = millis();
        if (!checkModem()) recoverModem();
    }
    delay(20);
}