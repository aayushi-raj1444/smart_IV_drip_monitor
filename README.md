# Smart IV Drip Monitoring System

An IoT-based healthcare monitoring system that continuously monitors the fluid level in an IV bottle and alerts medical staff before the bottle becomes empty. The system is built around the ESP32 microcontroller and uses an ultrasonic sensor to measure the remaining IV fluid level in real time.

The objective of this project is to reduce the need for frequent manual inspection of IV bottles while improving patient safety through timely alerts.

---

## Problem Statement

In many hospitals, IV bottles are monitored manually. During busy hours, delayed replacement of an empty IV bottle can interrupt medication delivery and increase the workload of healthcare staff.

WHO says unsafe healthcare causes over 3 million deaths globally each year, and medication related errors account for a significant proportion of preventable harm. Continuous monitoring of IV therapy can help reduce treatment interruptions, improve patient safety, and lessen the workload on healthcare professionals.

This project automates IV bottle monitoring by continuously measuring the fluid level and notifying the user whenever attention is required.

---

## Features

- Continuous IV fluid level monitoring
- Real-time level display on OLED
- Audible alert when the IV level falls below a threshold
- Wi-Fi-based notification using ESP32
- Blockage detection 
- Low-cost and compact embedded system

---

## Hardware Used

| Component 

| ESP32 Development Board 
| HC-SR04 Ultrasonic Sensor 
| OLED Display (SPI) 
| IR Sensor
| Active Buzzer
| Breadboard 
| Jumper Wires 
| USB Power Supply 

---

## Software Used

- Arduino IDE
- ESP32 Board Package
- Embedded C/C++
- Git & GitHub

---

## Project Structure

```
Smart-IV-Drip-Monitor/
│
├── src/
│   └── main.ino
│
├── Circuit_Diagram/
│   ├── Smart_IV_Circuit.pdf
│   └── Circuit_Documentation.pdf
│
├── Block_Diagram/
│   └── Block_Diagram.png
│
├── README.md
└── LICENSE
```

---

## Working Principle

1. The ultrasonic sensor measures the distance between the sensor and the IV fluid to determine the remaining fluid level.
2. The ESP32 processes the sensor data and calculates the current fluid level.
3. The IR sensor monitors the IV drip line to detect possible flow blockages.
4. The OLED display shows the IV status in real time.
5. If the IV level becomes low or a blockage is detected, the ESP32 activates the buzzer and sends a Wi-Fi notification.
6. The system continuously repeats this process for real-time monitoring.

---

## Repository Contents

This repository includes:

- Circuit diagram
- Block diagram
- Circuit documentation
- Source code
- Project documentation

---

## Future Improvements

The following features can be added in future versions:

- Cloud dashboard for remote monitoring
- Mobile application for notifications
- Battery backup
- Data logging
- Monitoring of multiple IV bottles
- Predictive alert system based on flow rate

---

## Applications

- Hospitals
- Clinics
- Intensive Care Units (ICU)
- Home healthcare monitoring

---

## Current Status

- Circuit design completed
- Block diagram completed
- Hardware documentation completed
- Software development in progress
- Prototype under development

---

## References

1. World Health Organization (WHO). *Patient Safety Fact Sheet*. https://www.who.int/news-room/fact-sheets/detail/patient-safety

---
