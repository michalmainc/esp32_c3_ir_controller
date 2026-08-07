# Build Guide

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Requirements
- Development Environment
- Project Structure
- Build
- Upload
- Serial Monitor
- Erase Flash
- Troubleshooting
- Related Documents

---

# Overview

This document describes how to build, upload and debug the firmware.

The project is based on:

- ESP-IDF
- PlatformIO
- Visual Studio Code

---

# Requirements

## Hardware

- Seeed Studio XIAO ESP32-C3
- USB-C cable

## Software

- Visual Studio Code
- PlatformIO IDE
- Python 3
- Git

---

# Development Environment

Current environment:

| Component | Version |
|-----------|---------|
| Framework | ESP-IDF |
| Build System | PlatformIO |
| Language | C |
| IDE | Visual Studio Code |

---

# Project Structure

```text
project/

├── src/
├── include/
├── lib/
├── docs/
├── examples/
├── test/
│
├── platformio.ini
├── sdkconfig.seeed_xiao_esp32c3
└── partitions.csv
```

---

# Build

Compile firmware:

```bash
pio run
```

PlatformIO creates the firmware inside:

```text
.pio/build/seeed_xiao_esp32c3/
```

---

# Upload

Upload firmware:

```bash
pio run --target upload
```

The upload port is configured in:

```text
platformio.ini
```

Example:

```ini
upload_port=/dev/esp32_xiao
```

---

# Serial Monitor

Open serial monitor:

```bash
pio device monitor
```

Default settings:

```text
115200 baud
```

---

# Erase Flash

Completely erase flash memory:

```bash
pio run --target erase
```

This removes:

- NVS configuration
- Wi-Fi settings
- Device configuration
- Stored parameters

---

# Build Artifacts

Generated files include:

```text
firmware.bin
bootloader.bin
partition-table.bin
firmware.elf
firmware.map
```

---

# Troubleshooting

## Wrong serial port

Check available ports:

```bash
ls -l /dev/esp32*
```

Verify the configured upload port:

```ini
upload_port=/dev/esp32_xiao
```

---

## Upload failed

Possible causes:

- Device disconnected
- Wrong upload port
- Serial monitor still open

---

## Configuration not updated

If configuration changes are not applied:

1. Erase flash
2. Upload firmware
3. Reboot the device

---

## Clean Build

Remove previous build artifacts:

```bash
pio run --target clean
```

Then rebuild:

```bash
pio run
```

---

# Recommended Workflow

1. Pull latest changes

```bash
git pull
```

2. Build

```bash
pio run
```

3. Upload

```bash
pio run --target upload
```

4. Open Serial Monitor

```bash
pio device monitor
```

5. Verify operation

- Web Dashboard
- REST API
- MQTT
- Serial Log

---

# Related Documents

- [Configuration](configuration.md)
- [Hardware](hardware.md)
- [Architecture](architecture.md)
- [REST API](api_rest.md)
- [MQTT API](api_mqtt.md)

---

← Back to [Documentation Index](README.md)