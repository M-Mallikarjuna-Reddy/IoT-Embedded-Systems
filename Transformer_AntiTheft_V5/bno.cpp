#include "bno.h"

//====================================================
// Reference Quaternion
//====================================================

float refW = 0.0f;
float refX = 0.0f;
float refY = 0.0f;
float refZ = 0.0f;

bool referenceSaved = false;

//====================================================
// Movement Flags
//====================================================

bool liftDetected = false;
bool dropDetected = false;

bool leftDetected = false;
bool rightDetected = false;

bool forwardDetected = false;
bool backwardDetected = false;

//====================================================
// Setup
//====================================================

void setupBNO()
{
    Serial.println();
    Serial.println("========================================");
    Serial.println("       INITIALIZING BNO085");
    Serial.println("========================================");

    while (!bno08x.begin_I2C(BNO_ADDRESS, &Wire))
    {
        Serial.println("Waiting for BNO085...");
        delay(1000);
    }

    Serial.println("BNO085 Connected");

    enableReports();

    referenceSaved = false;

    Serial.println("BNO085 Ready");
    Serial.println("========================================");
}

//====================================================
// Enable Sensor Reports
//====================================================

void enableReports()
{
    Serial.println("Enabling Reports...");

    if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR))
    {
        Serial.println("Rotation Report Failed");
        while (1);
    }

    if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION))
    {
        Serial.println("Linear Acceleration Report Failed");
        while (1);
    }

    Serial.println("Reports Enabled");
}

//====================================================
// Main Loop
//====================================================

void loopBNO()
{
    static unsigned long lastPrint = 0;

    float angle;

    if (!getRotationAngle(angle))
        return;

    if (millis() - lastPrint < 1000)
        return;

    lastPrint = millis();

    Serial.println();
    Serial.println("========================================");
    Serial.println("           BNO085 DATA");
    Serial.println("========================================");

    Serial.print("Rotation : ");
    Serial.print(angle, 2);
    Serial.println(" deg");

    Serial.print("Accel X : ");
    Serial.println(accelX, 2);

    Serial.print("Accel Y : ");
    Serial.println(accelY, 2);

    Serial.print("Accel Z : ");
    Serial.println(accelZ, 2);

    if (liftDetected)
        Serial.println("Movement : LIFT");

    if (dropDetected)
        Serial.println("Movement : DROP");

    if (leftDetected)
        Serial.println("Movement : LEFT");

    if (rightDetected)
        Serial.println("Movement : RIGHT");

    if (forwardDetected)
        Serial.println("Movement : FORWARD");

    if (backwardDetected)
        Serial.println("Movement : BACKWARD");

    Serial.println("========================================");
}

//====================================================
// Rotation Angle
//====================================================

bool getRotationAngle(float &angle)
{
    if (bno08x.wasReset())
    {
        Serial.println("BNO085 Reset");

        enableReports();

        referenceSaved = false;
    }

    bool rotationUpdated = false;

    liftDetected = false;
    dropDetected = false;

    leftDetected = false;
    rightDetected = false;

    forwardDetected = false;
    backwardDetected = false;

    while (bno08x.getSensorEvent(&sensorValue))
    {
        //----------------------------------------
        // Linear Acceleration
        //----------------------------------------

        if (sensorValue.sensorId == SH2_LINEAR_ACCELERATION)
        {
            accelX = sensorValue.un.linearAcceleration.x;
            accelY = sensorValue.un.linearAcceleration.y;
            accelZ = sensorValue.un.linearAcceleration.z;

            liftDetected = accelZ > LIFT_THRESHOLD;
            dropDetected = accelZ < DROP_THRESHOLD;

            leftDetected = accelX > LEFT_THRESHOLD;
            rightDetected = accelX < RIGHT_THRESHOLD;

            forwardDetected = accelY > FORWARD_THRESHOLD;
            backwardDetected = accelY < BACKWARD_THRESHOLD;
        }

        //----------------------------------------
        // Rotation Vector
        //----------------------------------------

        if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR)
        {
            float w = sensorValue.un.gameRotationVector.real;
            float x = sensorValue.un.gameRotationVector.i;
            float y = sensorValue.un.gameRotationVector.j;
            float z = sensorValue.un.gameRotationVector.k;

            if (!referenceSaved)
            {
                refW = w;
                refX = x;
                refY = y;
                refZ = z;

                referenceSaved = true;

                Serial.println("Reference Quaternion Saved");

                return false;
            }

            float dot =
                refW * w +
                refX * x +
                refY * y +
                refZ * z;

            if (isnan(dot))
                return false;

            dot = constrain(dot, 0.0f, 1.0f);

            angle =
                2.0f *
                acos(dot) *
                180.0f /
                PI;

            rotationUpdated = true;
        }
    }

    return rotationUpdated;
}