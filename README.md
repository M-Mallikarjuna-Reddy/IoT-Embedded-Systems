# IoT & Embedded Systems

A collection of IoT and embedded-systems work focused on sensor integration, ESP32-based devices, cellular communication, GPS/GNSS, and real-world monitoring and automation.

## Project

### Transformer Anti-Theft & Monitoring System

An ESP32-S3 based embedded system designed to detect unauthorized movement and orientation changes and send alerts through cellular communication.

#### Main capabilities

- Movement and orientation detection using BNO085 IMU
- Lift, drop, left, right, forward and backward movement detection
- GPS/GNSS location acquisition
- SMS alerts
- Automated phone-call alerts
- Remote ARM/DISARM and device-status commands through SMS
- Signal and battery-status reporting
- Google Maps location link generation

#### Technologies / Hardware

- ESP32-S3
- BNO085 IMU
- SIM7670 cellular modem
- GNSS/GPS
- Arduino / C++

> **Security note:** Real phone numbers, passwords, API keys, and other private credentials should never be committed to a public repository. The included configuration file contains placeholders only.

## Folder Structure

```text
Transformer_AntiTheft_V5/
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
└── sms.h
```
