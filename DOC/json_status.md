# JSON Status

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Status Structure
- Device
- Wi-Fi
- MQTT
- Outputs
- Inputs
- Temperature
- System
- Example
- Related Documents

---

# Overview

The status JSON represents the complete runtime state of the controller.

It is used by:

- REST API
- MQTT
- Web Dashboard

All three interfaces return identical runtime information.

---

# Status Structure

```text
device
wifi
mqtt
outputs
inputs
temperature
system
```

---

# Device

Contains device identification.

| Field | Type | Description |
|--------|------|-------------|
| name | String | Device name |
| serial | String | Unique serial number |

Example

```json
"device":
{
    "name":"esp32_c3",
    "serial":"ESP32-1CDBD4EA9DB0"
}
```

---

# Wi-Fi

Contains current Wi-Fi status.

| Field | Type | Description |
|--------|------|-------------|
| connected | Boolean | Connection status |
| ip | String | Current IP address |
| rssi | Integer | Signal strength (dBm) |

Example

```json
"wifi":
{
    "connected":true,
    "ip":"172.16.5.182",
    "rssi":-22
}
```

---

# MQTT

Contains MQTT client information.

| Field | Type | Description |
|--------|------|-------------|
| connected | Boolean | MQTT connection status |

Example

```json
"mqtt":
{
    "connected":true
}
```

---

# Outputs

Contains all output states.

## PWM

PWM duty cycle.

Range:

```
0...100 %
```

Example

```json
"pwm":
[
    50,
    75,
    50,
    25
]
```

---

## Relay

Relay output states.

Example

```json
"relay":
[
    false
]
```

The relay array size depends on:

```c
RELAY_CHANNELS
```

Future firmware versions may contain multiple relay outputs.

---

# Inputs

Reserved for future digital and analog inputs.

Current implementation:

```json
"inputs":
{
}
```

---

# Temperature

Contains detected DS18B20 sensors.

Fields

| Field | Type | Description |
|--------|------|-------------|
| count | Integer | Number of detected sensors |
| sensors | Array | Sensor list |

Each sensor contains:

| Field | Type | Description |
|--------|------|-------------|
| address | String | ROM ID without CRC byte |
| present | Boolean | Sensor detected |
| value | Float | Temperature (°C) |

Example

```json
"temperature":
{
    "count":2,
    "sensors":
    [
        {
            "address":"288F210D090000",
            "present":true,
            "value":25.875
        },
        {
            "address":"28FFB4A5311703",
            "present":true,
            "value":25.625
        }
    ]
}
```

---

# System

Runtime information.

| Field | Type | Description |
|--------|------|-------------|
| uptime | Integer | Seconds since boot |
| free_heap | Integer | Free heap memory |

Example

```json
"system":
{
    "uptime":132,
    "free_heap":189580
}
```

---

# Complete Example

```json
{
    "device":
    {
        "name":"esp32_c3",
        "serial":"ESP32-1CDBD4EA9DB0"
    },

    "wifi":
    {
        "connected":true,
        "ip":"172.16.5.182",
        "rssi":-22
    },

    "mqtt":
    {
        "connected":true
    },

    "outputs":
    {
        "pwm":[50,75,50,25],
        "relay":[false]
    },

    "inputs":
    {
    },

    "temperature":
    {
        "count":2,
        "sensors":
        [
            {
                "address":"288F210D090000",
                "present":true,
                "value":25.875
            },
            {
                "address":"28FFB4A5311703",
                "present":true,
                "value":25.625
            }
        ]
    },

    "system":
    {
        "uptime":132,
        "free_heap":189580
    }
}
```

---

# Notes

- Used by REST API
- Published via MQTT
- Displayed by Web Dashboard
- Generated from a shared runtime state
- Output structure is identical across all interfaces

---

# Related Documents

- [REST API](api_rest.md)
- [MQTT API](api_mqtt.md)
- [Architecture](architecture.md)
- [Configuration](configuration.md)

---

← Back to [Documentation Index](README.md)