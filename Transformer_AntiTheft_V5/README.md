# IoT-Based Transformer Anti-Theft & Monitoring System

An IoT-based embedded system designed to detect unauthorized movement and
orientation changes of a transformer and send alerts to predefined contacts.

## Overview

The system uses an ESP32-S3-based controller with an IMU, cellular modem,
and GPS/GNSS to monitor the physical movement of the transformer.

When suspicious movement is detected, the system can initiate a phone call
and send an SMS alert containing movement information and GPS location.

## Features

- Real-time movement detection
- Rotation detection
- Lift detection
- Drop detection
- Left and right movement detection
- Forward and backward movement detection
- GPS/GNSS location tracking
- SMS alerts
- Automatic phone-call alerts
- Battery level monitoring
- Cellular signal-strength monitoring
- Remote ARM and DISARM control
- Device status monitoring through SMS
- Remote device reboot
- Password-protected commands

## Hardware

- ESP32-S3
- BNO085 IMU
- SIM7670 cellular modem
- GPS/GNSS
- Supporting power and communication components

## Technologies

- C++
- Arduino
- ESP32
- I2C
- UART
- AT Commands
- GPS/GNSS
- Cellular communication
- Sensor data processing

## Movement Detection

The BNO085 provides rotation and linear acceleration data.

The system monitors:

- Rotation
- Lift
- Drop
- Left
- Right
- Forward
- Backward

A reference orientation is saved and subsequent orientation changes are
compared against that reference.

## Alert System

When movement is detected, the alarm system changes its state and starts
the alert process.

The system can:

1. Detect suspicious movement.
2. Identify the movement type.
3. Obtain the current GPS location.
4. Initiate a phone call.
5. Send an SMS alert.
6. Provide location information through a Google Maps link.
7. Monitor the call status.
8. Move into a cooldown state before rearming.

## GPS

The cellular modem is used for GNSS positioning.

When a GPS fix is available, the system obtains latitude and longitude
coordinates and generates a Google Maps location link.

## SMS Commands

Authorized users can interact with the device through SMS commands.

Available commands include:

- STATUS
- LOCATION
- MAP
- SIGNAL
- BATTERY
- ARM
- DISARM
- PING
- TEST
- INFO
- HELP
- REBOOT

Password protection is used for control-related commands.

## System States

The alarm system uses different states to control its operation:

- ARMED
- CALL_CONTACT
- WAITING_FOR_CALL
- COOLDOWN

These states help manage detection, alerting, call monitoring, and
rearming of the system.

## Project Structure

```text
Transformer_AntiTheft_V5/
│
├── Transformer_AntiTheft_V5.ino
├── alarm.cpp
├── alarm.h
├── bno.cpp
├── bno.h
├── config.cpp
├── config.h
├── globals.cpp
├── globals.h
├── gps.cpp
├── gps.h
├── modem.cpp
├── modem.h
├── modem_parser.cpp
├── modem_parser.h
├── sms.cpp
├── sms.h
└── README.md
