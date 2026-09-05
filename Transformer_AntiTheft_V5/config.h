#ifndef CONFIG_H
#define CONFIG_H

// -------------------------------
// Hardware Pins
// -------------------------------
#define SDA_PIN         15
#define SCL_PIN         16

#define MODEM_RX        17
#define MODEM_TX        18
#define MODEM_BAUD      115200

#define BNO_ADDRESS     0x4B

// -------------------------------
// Theft Detection
// -------------------------------
#define ROTATION_LIMIT      5.0

#define LIFT_THRESHOLD       2.5
#define DROP_THRESHOLD      -2.5

#define LEFT_THRESHOLD       2.0
#define RIGHT_THRESHOLD     -2.0

#define FORWARD_THRESHOLD    2.0
#define BACKWARD_THRESHOLD  -2.0

// -------------------------------
// Timing
// -------------------------------
#define GPS_UPDATE_INTERVAL   5000UL
#define MODEM_CHECK_INTERVAL 60000UL
#define ALERT_INTERVAL       20000UL
#define CALL_TIMEOUT         10000UL

// -------------------------------
// Owner / Security
// -------------------------------
// Replace these placeholders with your own values on the device.
// Do NOT commit real phone numbers or passwords to a public repository.
const char OWNER_NUMBER[] = "YOUR_PHONE_NUMBER";
const char SMS_PASSWORD[] = "YOUR_SMS_PASSWORD";

// -------------------------------
// Emergency Contacts
// -------------------------------
extern const char* PHONE_NUMBERS[];
extern const int TOTAL_CONTACTS;

#endif
