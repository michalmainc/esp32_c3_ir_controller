# PWM Module

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Features
- Hardware
- Configuration
- REST API
- MQTT
- Web Dashboard
- Source Files
- Runtime Flow
- Future Improvements
- Related Documents

---

# Overview

The PWM module controls four independent PWM outputs.

Current implementation:

- 4 channels
- 400 Hz frequency
- Duty cycle: 0...100 %
- Runtime control
- REST API
- MQTT
- Web Dashboard

---

# Features

- Four independent channels
- Fixed frequency
- Runtime duty cycle control
- Shared device state
- Immediate MQTT status update
- Immediate Web Dashboard update

---

# Hardware

| Channel | GPIO |
|---------|-----:|
| PWM1 | GPIO2 |
| PWM2 | GPIO3 |
| PWM3 | GPIO4 |
| PWM4 | GPIO5 |

---

# Configuration

Current parameters:

| Parameter | Value |
|-----------|------:|
| Channels | 4 |
| Frequency | 400 Hz |
| Duty Cycle | 0...100 % |

PWM channel names are stored in the configuration.

---

# REST API

Endpoint

```http
POST /api/pwm
```

Example

```bash
curl \
-X POST \
"http://192.168.1.100/api/pwm?channel=2&value=75"
```

---

# MQTT

Topic

```
<device_name>/command/pwm
```

Example

```json
{
    "channel":2,
    "value":75
}
```

Changing PWM automatically publishes a new status message.

---

# Web Dashboard

Each channel is represented by:

- channel name
- slider
- percentage value

Changing a slider:

1. sends REST request
2. updates PWM output
3. updates runtime state
4. publishes MQTT status

---

# Source Files

```
src/

pwm/
    pwm.c
    pwm.h

device/
    device_commands.c
```

---

# Runtime Flow

REST

↓

device_command_set_pwm()

↓

PWM Driver

↓

device_state

↓

MQTT Status

↓

Web Dashboard

---

MQTT

↓

device_command_set_pwm()

↓

PWM Driver

↓

device_state

↓

REST

↓

Web Dashboard

---

# Future Improvements

Possible future additions:

- configurable frequency
- fade in / fade out
- minimum duty limit
- maximum duty limit
- astronomical control
- scheduler integration

---

# Related Documents

- [REST API](api_rest.md)
- [MQTT API](api_mqtt.md)
- [Configuration](configuration.md)
- [Architecture](architecture.md)

---

← Back to [Documentation Index](README.md)