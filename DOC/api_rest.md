# REST API

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Base URL
- Authentication
- Endpoints
- GET /api/status
- POST /api/pwm
- POST /api/relay
- POST /api/restart
- HTTP Status Codes
- Examples
- Related Documents

---

# Overview

The REST API provides HTTP access to the controller.

Data format:

- Request: HTTP
- Response: JSON

Authentication is currently not required.

---

# Base URL

Example:

```
http://192.168.1.100
```

All endpoints are relative to the device IP address.

---

# Authentication

Current firmware:

```
No authentication
```

Future versions may support:

- Username / Password
- API Token

---

# Endpoints

| Method | URI | Description |
|---------|-----|-------------|
| GET | `/api/status` | Returns complete device status |
| POST | `/api/pwm` | Sets PWM output |
| POST | `/api/relay` | Sets relay output |
| POST | `/api/restart` | Restarts the controller |

---

# GET /api/status

Returns the complete runtime status.

## Request

```http
GET /api/status
```

## Response

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

Detailed field description:

See:

- [JSON Status](json_status.md)

---

# POST /api/pwm

Sets PWM duty cycle.

## Request

```http
POST /api/pwm
```

Parameters:

| Name | Type | Range |
|------|------|------:|
| channel | Integer | 0...3 |
| value | Integer | 0...100 |

Example:

```bash
curl \
-X POST \
"http://192.168.1.100/api/pwm?channel=1&value=50"
```

Response:

```json
{
    "status":"ok"
}
```

---

# POST /api/relay

Controls relay outputs.

## Request

```http
POST /api/relay
```

Body:

```json
{
    "channel":1,
    "state":true
}
```

Parameters:

| Name | Type | Description |
|------|------|-------------|
| channel | Integer | Relay number (1...) |
| state | Boolean | true / false |

Response:

```json
{
    "status":"ok"
}
```

Example:

```bash
curl \
-X POST \
-H "Content-Type: application/json" \
-d '{"channel":1,"state":true}' \
"http://192.168.1.100/api/relay"
```

---

# POST /api/restart

Restarts the controller.

## Request

```http
POST /api/restart
```

Example:

```bash
curl \
-X POST \
"http://192.168.1.100/api/restart"
```

Response:

```json
{
    "status":"ok"
}
```

---

# HTTP Status Codes

| Code | Description |
|------|-------------|
| 200 | Success |
| 400 | Invalid request |
| 404 | Endpoint not found |
| 500 | Internal error |

---

# Typical Workflow

Read current status

```text
GET /api/status
```

↓

Change PWM

```text
POST /api/pwm
```

↓

Read status

```text
GET /api/status
```

---

Change relay

```text
POST /api/relay
```

↓

Read status

```text
GET /api/status
```

---

Restart controller

```text
POST /api/restart
```

---

# Notes

Current REST API characteristics:

- HTTP only
- JSON responses
- Stateless
- No authentication
- Shared runtime state
- Shared command layer

---

# Related Documents

- [MQTT API](api_mqtt.md)
- [JSON Status](json_status.md)
- [Architecture](architecture.md)
- [Relay](relay.md)
- [PWM](pwm.md)

---

← Back to [Documentation Index](README.md)