# Hardware

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Target Platform
- Hardware Resources
- GPIO Assignment
- PWM Outputs
- Relay Output
- 1-Wire Interface
- Device Identification
- Power Supply
- Expansion
- Related Documents

---

# Overview

The firmware is designed for the **Seeed Studio XIAO ESP32-C3** development board.

The board provides all hardware resources required by the current firmware while leaving additional GPIOs available for future expansion.

---

# Target Platform

| Parameter | Value |
|-----------|-------|
| Board | Seeed Studio XIAO ESP32-C3 |
| MCU | ESP32-C3 |
| CPU | RISC-V 32-bit |
| Clock | 160 MHz |
| Flash | 4 MB |
| Framework | ESP-IDF |

Reference:

- XIAO ESP32-C3 Schematic
- ESP32-C3 Datasheet

---

# Hardware Resources

Current firmware uses:

- 4 PWM outputs
- 1 Relay output
- 1-Wire bus
- Wi-Fi
- Embedded Web Server
- MQTT Client

Future firmware versions will additionally support:

- Astronomical scheduler
- Home Assistant Discovery
- OTA firmware update

---

# GPIO Assignment

| GPIO | Function | Status |
|------|----------|:------:|
| GPIO2 | PWM Channel 1 | ✅ |
| GPIO3 | PWM Channel 2 | ✅ |
| GPIO4 | PWM Channel 3 | ✅ |
| GPIO5 | PWM Channel 4 | ✅ |
| GPIO6 | Relay Channel 1 | ✅ |
| GPIO7 | 1-Wire DS18B20 Bus | ✅ |

---

# PWM Outputs

Current implementation:

- 4 independent PWM channels
- Frequency: 400 Hz
- Duty cycle: 0…100 %
- Configurable channel names

Future versions may support:

- fading
- schedules
- astronomical control

---

# Relay Output

Current implementation:

- Relay Channel 1
- REST control
- MQTT control
- Web Dashboard control

The firmware architecture supports multiple relay outputs.

Adding another relay only requires:

- increasing `RELAY_CHANNELS`
- assigning an additional GPIO

---

# 1-Wire Interface

GPIO7 is used as a shared 1-Wire bus.

Supported devices:

- DS18B20

Features:

- automatic device discovery
- multiple sensors
- unique ROM identification
- temperature reporting
- REST
- MQTT
- Web Dashboard

---

# Device Identification

Each controller provides:

- Device Name
- Device Serial Number

Both values are included in:

- REST API
- MQTT Status

---

# Power Supply

The controller is powered directly from the USB-C connector of the XIAO ESP32-C3.

The firmware does not require any additional power management circuitry.

---

# Expansion

The project architecture is designed for future expansion.

Examples:

- additional relay outputs
- digital inputs
- analog inputs
- I²C sensors
- SPI peripherals

Most new hardware can be integrated as independent firmware modules.

---

# Related Documents

- [Architecture](architecture.md)
- [Configuration](configuration.md)
- [PWM](pwm.md)
- [Relay](relay.md)
- [DS18B20](ds18b20.md)

---

← Back to [Documentation Index](README.md)