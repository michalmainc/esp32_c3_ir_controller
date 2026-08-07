# Architecture

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Architecture
- Directory Structure
- Module Dependencies
- Runtime Flow
- Source Tree
- Related Documents

---

# Overview

The firmware follows a layered architecture.

Each module has a single responsibility.

Communication interfaces (REST, MQTT and Web Dashboard) never access hardware directly.

---

# Architecture

```text
                REST API
                   │
                   │
                   ▼
              device_commands
                   ▲
                   │
MQTT ──────────────┤
                   │
                   ▼
            Web Dashboard
                   │
                   ▼
             device_state
                   │
     ┌─────────────┼─────────────┐
     │             │             │
     ▼             ▼             ▼
    PWM          Relay      Temperature
```

---

# Layer Description

## User Interface

Provides access to the controller.

Modules:

- REST API
- MQTT
- Web Dashboard

---

## Command Layer

Responsible for validating and executing commands.

Current module:

```
device_commands
```

Examples:

```
device_command_set_pwm()
device_command_set_relay()
```

No user interface accesses hardware directly.

---

## State Layer

Contains the current runtime state.

```
device_state
```

Used by:

- REST API
- MQTT
- Web Dashboard

This guarantees that all interfaces return identical information.

---

## Driver Layer

Responsible for hardware access.

Current modules:

- pwm
- relay
- temperature

Future modules:

- scheduler
- astronomy
- ota

---

# Directory Structure

```
src/

config/
device/
mqtt/
onewire/
pwm/
relay/
temperature/
web/
wifi/
```

Each module contains:

```
module.c
module.h
```

---

# Module Dependencies

```
REST API
        │
        ▼
device_commands
        │
        ▼
relay
```

```
MQTT
      │
      ▼
device_commands
      │
      ▼
pwm
```

```
Web Dashboard
        │
        ▼
REST API
```

---

# Runtime Flow

## Boot

```
Power On

↓

Load Configuration

↓

Initialize Drivers

↓

Start Wi-Fi

↓

Start MQTT

↓

Start Web Server

↓

Application Ready
```

---

## PWM Control

```
REST

↓

device_commands

↓

PWM

↓

device_state

↓

REST
MQTT
Web
```

---

## Relay Control

```
REST / MQTT / Web

↓

device_commands

↓

Relay Driver

↓

device_state

↓

MQTT Status

↓

REST Status

↓

Web Dashboard
```

---

## Temperature Update

```
DS18B20

↓

temperature

↓

device_state

↓

REST

↓

MQTT

↓

Web Dashboard
```

---

# Design Rules

Current design rules:

- One module = one responsibility
- Hardware access only inside drivers
- Shared runtime state
- Shared command layer
- Configuration isolated inside config module
- No duplicated JSON generation
- REST and MQTT expose identical information

---

# Source Tree

```
src/

config/
device/
mqtt/
onewire/
pwm/
relay/
temperature/
web/
wifi/

main.c
```

---

# Future Modules

Planned additions:

- Home Assistant
- Astronomy
- Scheduler
- OTA
- NTP

These modules should integrate without changing existing APIs.

---

# Related Documents

- [Configuration](configuration.md)
- [REST API](api_rest.md)
- [MQTT API](api_mqtt.md)
- [JSON Status](json_status.md)
- [Relay](relay.md)
- [PWM](pwm.md)

---

← Back to [Documentation Index](README.md)