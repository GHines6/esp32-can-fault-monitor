# ESP32 CAN-Based Distributed Sensor and Fault-Monitoring System

A two-node embedded system that uses the ESP32 TWAI controller and SN65HVD230 transceivers to transmit MPU-6050 inertial sensor data over a CAN bus.

The project demonstrates embedded C/C++, CAN communication, I2C sensor integration, fault detection, recovery monitoring, hardware validation, and technical documentation.

## Project Goals

This project was built to demonstrate skills relevant to embedded systems, avionics, aerospace, industrial controls, hardware test, and defense-related engineering roles.

Key goals include:

- Implementing a two-node CAN network
- Reading accelerometer and gyroscope data from an MPU-6050
- Packing signed 16-bit sensor values into CAN payloads
- Detecting communication and sensor faults
- Recognizing automatic recovery without requiring a manual reset
- Validating the physical CAN bus with resistance measurements
- Preparing the system for Python logging and plotting

## System Architecture

### Sensor Node

The sensor node:

- Reads MPU-6050 acceleration and gyroscope registers over I2C
- Validates I2C acknowledgments and received byte count
- Transmits acceleration data on CAN ID `0x100`
- Transmits heartbeat and sensor-health status on CAN ID `0x101`
- Transmits gyroscope data on CAN ID `0x102`
- Stops transmitting invalid sensor data during an MPU fault
- Continues transmitting heartbeat messages while the ESP32 remains alive
- Automatically resumes sensor-data transmission after the MPU recovers

### Monitor Node

The monitor node:

- Receives and validates CAN frames
- Checks CAN identifiers and payload lengths
- Reconstructs signed 16-bit sensor values
- Converts acceleration to `g`
- Converts angular velocity to degrees per second
- Tracks acceleration-frame sequence numbers
- Detects data-frame timeouts
- Detects heartbeat timeouts
- Reports sensor-health status received over CAN
- Automatically reports recovery when communication resumes

## Hardware

- 2 × ESP-WROOM-32D development boards
- 2 × Waveshare SN65HVD230 3.3 V CAN transceiver boards
- 1 × MPU-6050 accelerometer and gyroscope module
- Twisted-pair wiring for CANH and CANL
- Shared ground between nodes
- Breadboards and jumper wires
- Digital multimeter

## Pin Assignments

### CAN / TWAI

| Signal | ESP32 Pin |
|---|---:|
| CAN TX | GPIO 25 |
| CAN RX | GPIO 26 |

### I2C

| Signal | ESP32 Pin |
|---|---:|
| SDA | GPIO 21 |
| SCL | GPIO 22 |

## Physical CAN Connections

Each ESP32 connects to one SN65HVD230 transceiver:

| ESP32 | Transceiver |
|---|---|
| 3.3 V | 3.3 V |
| GND | GND |
| GPIO 25 | CAN TX |
| GPIO 26 | CAN RX |

The transceiver boards connect together as follows:

| Node A | Node B |
|---|---|
| CANH | CANH |
| CANL | CANL |
| GND | GND |

Each Waveshare transceiver board includes a 120-ohm termination resistor. With both modules connected and unpowered, the measured bus resistance between CANH and CANL was approximately 60 ohms.

## CAN Configuration

- CAN mode: Classical CAN
- ESP32 peripheral: TWAI
- Bit rate: `125 kbps`
- Standard 11-bit identifiers
- Maximum payload size: 8 bytes

## CAN Message Definitions

### Acceleration Frame — ID `0x100`

| Byte | Field |
|---:|---|
| 0 | Acceleration sequence counter |
| 1 | Accel X high byte |
| 2 | Accel X low byte |
| 3 | Accel Y high byte |
| 4 | Accel Y low byte |
| 5 | Accel Z high byte |
| 6 | Accel Z low byte |

Payload length: 7 bytes

Acceleration conversion at the default MPU-6050 ±2 g range:

```text
acceleration_g = raw_acceleration / 16384.0
