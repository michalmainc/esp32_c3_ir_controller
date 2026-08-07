# MQTT API

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Broker Configuration
- Topic Structure
- Published Topics
- Subscribed Topics
- Message Payloads
- QoS
- Retain
- Examples
- Future Topics
- Related Documents

---

# Overview

The controller includes an MQTT client used for:

- Device status publishing
- Remote PWM control
- Remote relay control

The MQTT client starts automatically after the Wi-Fi connection has been established.

---

# Broker Configuration

MQTT parameters are stored in the device configuration.

Current parameters:

| Parameter | Description |
|-----------|-------------|
| Broker Address | MQTT server IP or hostname |
| Broker Port | MQTT server port |

Example:

```
Broker : 192.168.1.182
Port   : 1883
```

---

# Topic Structure

All topics use the configured device name.

```
<device_name>/...
```

Example:

```
esp32_c3/status
esp32_c3/command/pwm
esp32_c3/command/relay
```

If the device name is changed to:

```
garage_controller
```

Topics automatically become:

```
garage_controller/status
garage_controller/command/pwm
garage_controller/command/relay
```

---

# Published Topics

## Status

Topic

```
<device_name>/status
```

Direction

```
ESP32 → Broker
```

Payload

```json
{
    "device": {},
    "wifi": {},
    "mqtt": {},
    "outputs": {},
    "inputs": {},
    "temperature": {},
    "system": {}
}
```

Published:

- every 10 seconds
- after PWM change
- after relay change
- immediately after MQTT connection

---

# Subscribed Topics

## PWM Control

Topic

```
<device_name>/command/pwm
```

Direction

```
Broker → ESP32
```

Payload

```json
{
    "channel":2,
    "value":75
}
```

Parameters

| Name | Type | Range |
|------|------|------:|
| channel | Integer | 1...4 |
| value | Integer | 0...100 |

Example

```bash
mosquitto_pub \
-h localhost \
-t "esp32_c3/command/pwm" \
-m '{"channel":2,"value":75}'
```

---

## Relay Control

Topic

```
<device_name>/command/relay
```

Direction

```
Broker → ESP32
```

Payload

```json
{
    "channel":1,
    "state":true
}
```

Parameters

| Name | Type | Description |
|------|------|-------------|
| channel | Integer | Relay number |
| state | Boolean | true / false |

Example

```bash
mosquitto_pub \
-h localhost \
-t "esp32_c3/command/relay" \
-m '{"channel":1,"state":true}'
```

Switch relay OFF

```bash
mosquitto_pub \
-h localhost \
-t "esp32_c3/command/relay" \
-m '{"channel":1,"state":false}'
```

---

# Message Payloads

## PWM

```json
{
    "channel":1,
    "value":50
}
```

---

## Relay

```json
{
    "channel":1,
    "state":true
}
```

---

## Status

See:

- [JSON Status](json_status.md)

---

# QoS

Current implementation:

| Topic | QoS |
|------|----:|
| status | 1 |
| command/pwm | 1 |
| command/relay | 1 |

---

# Retain

Current implementation:

| Topic | Retain |
|------|:------:|
| status | No |
| command/pwm | No |
| command/relay | No |

---

# Typical Workflow

Device startup

```
Power On

↓

Wi-Fi Connected

↓

MQTT Connected

↓

Publish Status
```

---

Remote PWM Control

```
Broker

↓

command/pwm

↓

ESP32

↓

PWM Driver

↓

Publish Status
```

---

Remote Relay Control

```
Broker

↓

command/relay

↓

ESP32

↓

Relay Driver

↓

Publish Status
```

---

# Future Topics

Planned topics:

```
<device_name>/availability
```

```
<device_name>/command/restart
```

```
homeassistant/...
```

---

# Notes

Current implementation:

- JSON payloads
- Shared device state
- Automatic status publication
- Dynamic topic prefix based on device name
- REST and MQTT expose identical runtime information

---

# Related Documents

- [REST API](api_rest.md)
- [JSON Status](json_status.md)
- [Configuration](configuration.md)
- [Architecture](architecture.md)

---

← Back to [Documentation Index](README.md)