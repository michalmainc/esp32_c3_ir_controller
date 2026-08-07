# ESP32-C3 Controller

Firmware for the **Seeed Studio XIAO ESP32-C3** developed using **ESP-IDF** and **PlatformIO**.

The project provides a modular IoT controller featuring PWM outputs, relay control, temperature monitoring, REST API, MQTT communication and an embedded Web Dashboard.

---

# Overview

The firmware is designed as a modular platform for automation and IoT applications.

Current functionality includes:

- PWM output control
- Relay control
- Multiple DS18B20 temperature sensors
- Embedded Web Dashboard
- REST API
- MQTT Publish / Subscribe
- Configuration stored in NVS

The project follows a layered architecture where REST, MQTT and the Web Dashboard share a common command layer.

---

# Features

## Implemented

- ESP-IDF Framework
- PlatformIO Build System
- Wi-Fi Station Mode
- Static IP Configuration
- Embedded Web Dashboard
- REST API
- MQTT Client
- MQTT Publish
- MQTT Subscribe
- JSON Device Status
- Four PWM Outputs (400 Hz)
- Relay Output
- Multi-channel Relay Architecture
- Multiple DS18B20 Sensors
- Device Serial Number
- Configuration Storage (NVS)

## Planned

- MQTT Availability
- MQTT Last Will (LWT)
- MQTT Restart Command
- Home Assistant Discovery
- OTA Firmware Update
- Astronomical Scheduler
- Sunrise / Sunset Calculations
- Automation Engine

---

# Hardware

Target platform:

- Seeed Studio XIAO ESP32-C3
- ESP32-C3 @ 160 MHz
- 4 MB Flash
- ESP-IDF Framework

| Function | GPIO |
|----------|-----:|
| PWM Channel 1 | GPIO2 |
| PWM Channel 2 | GPIO3 |
| PWM Channel 3 | GPIO4 |
| PWM Channel 4 | GPIO5 |
| Relay Channel 1 | GPIO6 |
| 1-Wire Bus | GPIO7 |

Detailed hardware description is available here:

- [Hardware Documentation](docs/hardware.md)

---

# Quick Start

Clone repository

```bash
git clone <repository>
```

Build firmware

```bash
pio run
```

Upload firmware

```bash
pio run --target upload
```

Open Serial Monitor

```bash
pio device monitor
```

See also:

- [Build Guide](docs/build.md)

---

# Documentation

Complete project documentation is available in the **docs** directory.

Start here:

- **[Documentation Index](docs/README.md)**

Documentation includes:

- Firmware Architecture
- Hardware Description
- Build Guide
- Configuration
- REST API
- MQTT API
- JSON Status Format
- PWM Module
- Relay Module
- DS18B20 Module
- Web Dashboard
- OTA Update (planned)
- Home Assistant Integration (planned)
- Astronomical Scheduler (planned)

---

# Repository Structure

```text
.
├── docs/
├── examples/
├── include/
├── lib/
├── src/
├── test/
├── platformio.ini
├── README.md
├── CHANGELOG.md
└── LICENSE
```

---

# Development Principles

The project follows several design principles:

- Layered architecture
- Modular components
- Shared device state
- Single command layer for REST, MQTT and Web
- English source code and documentation
- Incremental development
- Semantic versioning

---

# Project Status

| Sprint | Description | Status |
|---------|-------------|:------:|
| Sprint 1 | Core Firmware | ✅ |
| Sprint 2 | Web Dashboard | ✅ |
| Sprint 3 | MQTT & Relay | ✅ |
| Sprint 4 | Documentation | 🚧 |
| Sprint 5 | Home Assistant | ⏳ |
| Sprint 6 | Astronomy | ⏳ |

---

# Roadmap

## Version 0.5.x

- MQTT Communication
- REST API
- Relay Control
- Documentation

## Version 0.6.x

- Home Assistant Discovery
- MQTT Availability
- MQTT Last Will (LWT)
- MQTT Restart Command

## Version 0.7.x

- Astronomical Scheduler
- Sunrise / Sunset Calculations
- Automation Engine

## Version 0.8.x

- OTA Firmware Update

---

# License

See the **LICENSE** file for licensing information.