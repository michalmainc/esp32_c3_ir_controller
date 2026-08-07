# Configuration

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Configuration Storage
- Configuration Parameters
- Default Values
- NVS
- Factory Reset
- Configuration Flow
- Related Documents

---

# Overview

All user configuration is stored in the ESP32 NVS (Non-Volatile Storage).

Configuration is loaded during startup and remains available until the device is powered off.

---

# Configuration Storage

Configuration is managed by:

```
src/config/
    config.c
    config.h
```

The configuration is represented by a single structure:

```c
device_config_t
```

---

# Configuration Parameters

## Device

| Parameter | Description |
|-----------|-------------|
| Device Name | MQTT topic root |
| Serial Number | Unique device identifier |

---

## Network

| Parameter | Description |
|-----------|-------------|
| IP Address | Static IP address |
| Netmask | Network mask |
| Gateway | Default gateway |

---

## MQTT

| Parameter | Description |
|-----------|-------------|
| Broker Address | MQTT server |
| Broker Port | MQTT port |

---

## PWM

Each PWM channel stores:

- Name

PWM values are runtime variables and are not part of the configuration.

---

## Relay

Current firmware:

- Relay Channel 1

Future versions will allow:

- Relay names
- Default startup state

---

## DS18B20

Each sensor stores:

- User name (planned)

Sensor ROM IDs are detected automatically.

---

## Location

Used by the astronomical scheduler.

| Parameter | Description |
|-----------|-------------|
| Latitude | GPS latitude |
| Longitude | GPS longitude |

---

# Default Values

Current defaults are defined in:

```
config.c
```

Typical values include:

- Device Name
- Static IP
- MQTT Broker
- GPS Coordinates

If no valid configuration exists in NVS, default values are loaded automatically.

---

# NVS

Configuration is automatically:

- loaded during boot
- modified in RAM
- saved to NVS

No manual file handling is required.

---

# Factory Reset

Factory reset can be performed by erasing the flash memory.

PlatformIO:

```bash
pio run --target erase
```

After reboot the firmware recreates the default configuration.

---

# Configuration Flow

```text
Boot
 │
 ▼
Load defaults
 │
 ▼
Read NVS
 │
 ▼
Configuration available
 │
 ▼
Application start
```

---

# Future Configuration

Planned configuration items:

- Home Assistant
- OTA
- Astronomical Scheduler
- NTP
- Time Zone
- Relay Names
- PWM Limits

---

# Related Documents

- [Build Guide](build.md)
- [Architecture](architecture.md)
- [REST API](api_rest.md)
- [MQTT API](api_mqtt.md)

---

← Back to [Documentation Index](README.md)