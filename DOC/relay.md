# Relay Module

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Features
- Hardware
- Configuration
- Architecture
- REST API
- MQTT
- Web Dashboard
- Runtime Flow
- Source Files
- Future Expansion
- Related Documents

---

# Overview

The Relay module controls digital output channels.

The current firmware supports one relay output, however the architecture is designed to support multiple relay channels without changes to the communication interfaces.

---

# Features

Current implementation:

- Relay ON/OFF control
- REST API
- MQTT
- Web Dashboard
- Shared runtime state
- Immediate MQTT status update
- Immediate Web Dashboard update

---

# Hardware

Current GPIO assignment:

| Relay | GPIO |
|-------|-----:|
| Relay 1 | GPIO6 |

---

# Configuration

Current configuration:

```c
#define RELAY_CHANNELS    1
```

To add another relay:

```c
#define RELAY_CHANNELS    2
```

Assign the GPIO in:

```
relay.c
```

Example:

```c
static const gpio_num_t relay_gpio[RELAY_CHANNELS] =
{
    GPIO_NUM_6,
    GPIO_NUM_10
};
```

The remaining firmware automatically adapts to the new relay count.

---

# Architecture

Relay commands are processed through the common command layer.

```
REST

↓

device_command_set_relay()

↓

Relay Driver

↓

device_state

↓

MQTT Status

↓

Web Dashboard
```

The Web Dashboard never accesses the relay driver directly.

MQTT never accesses the relay driver directly.

REST never accesses the relay driver directly.

---

# REST API

Endpoint

```http
POST /api/relay
```

Request

```json
{
    "channel":1,
    "state":true
}
```

Example

```bash
curl \
-X POST \
-H "Content-Type: application/json" \
-d '{"channel":1,"state":true}' \
"http://192.168.1.100/api/relay"
```

---

# MQTT

Topic

```
<device_name>/command/relay
```

Payload

```json
{
    "channel":1,
    "state":true
}
```

Example

```bash
mosquitto_pub \
-h localhost \
-t "esp32_c3/command/relay" \
-m '{"channel":1,"state":true}'
```

Turn relay OFF

```bash
mosquitto_pub \
-h localhost \
-t "esp32_c3/command/relay" \
-m '{"channel":1,"state":false}'
```

---

# Web Dashboard

The dashboard displays one button for each relay channel.

Current implementation:

```
Relay 1

[ ON ]
```

Button actions:

- Toggle relay
- Update runtime state
- Publish MQTT status
- Refresh dashboard

---

# Runtime Flow

## REST

```
POST /api/relay

↓

device_command_set_relay()

↓

relay_set()

↓

device_state

↓

MQTT Status
```

---

## MQTT

```
command/relay

↓

device_command_set_relay()

↓

relay_set()

↓

device_state

↓

REST

↓

Web Dashboard
```

---

## Web Dashboard

```
Button

↓

POST /api/relay

↓

device_command_set_relay()

↓

relay_set()

↓

device_state

↓

MQTT Status
```

---

# Source Files

```
src/

relay/
    relay.c
    relay.h

device/
    device_commands.c
    device_commands.h
```

---

# Future Expansion

The relay module is designed to support:

- multiple relay outputs
- relay names
- startup state
- scheduler integration
- astronomical control
- Home Assistant entities

No API changes should be required when additional relay channels are added.

---

# Related Documents

- [REST API](api_rest.md)
- [MQTT API](api_mqtt.md)
- [Architecture](architecture.md)
- [JSON Status](json_status.md)

---

← Back to [Documentation Index](README.md)