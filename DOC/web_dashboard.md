# Web Dashboard

> ESP32-C3 Controller Documentation

← Back to [Documentation Index](README.md)

---

# Table of Contents

- Overview
- Features
- Access
- Dashboard Layout
- Device Information
- PWM Control
- Relay Control
- Temperature
- Restart
- Data Refresh
- Runtime Flow
- Source Files
- Future Improvements
- Related Documents

---

# Overview

The firmware contains an embedded Web Dashboard.

The dashboard provides real-time monitoring and control of the controller using any modern web browser.

No external software is required.

---

# Features

Current implementation:

- Responsive layout
- Device information
- Wi-Fi status
- MQTT status
- PWM control
- Relay control
- DS18B20 temperature display
- Device restart

---

# Access

Open a web browser and enter the device IP address.

Example:

```
http://172.16.5.182
```

The dashboard is served directly by the ESP32 HTTP server.

---

# Dashboard Layout

Current layout:

```
+--------------------------------------+
| Device Information                   |
+--------------------------------------+

+--------------------------------------+
| PWM Outputs                          |
+--------------------------------------+

+--------------------------------------+
| Relay Outputs                        |
+--------------------------------------+

+--------------------------------------+
| Temperature Sensors                  |
+--------------------------------------+

+--------------------------------------+
| System                               |
+--------------------------------------+
```

---

# Device Information

Displayed information:

- Device Name
- Serial Number
- IP Address
- Wi-Fi Status
- MQTT Status
- RSSI
- Uptime
- Free Heap

The information is refreshed automatically.

---

# PWM Control

The dashboard displays one slider for each PWM channel.

Each slider contains:

- Channel name
- Duty cycle
- Percentage value

Changing a slider:

```
Slider

↓

POST /api/pwm

↓

device_command_set_pwm()

↓

PWM Driver

↓

device_state

↓

MQTT Status
```

---

# Relay Control

The dashboard displays one button for each relay output.

Current implementation:

```
Relay 1

[ ON ]
```

Clicking the button:

```
Button

↓

POST /api/relay

↓

device_command_set_relay()

↓

Relay Driver

↓

device_state

↓

MQTT Status
```

The button state is updated immediately after a successful command.

---

# Temperature

For each detected DS18B20 sensor the dashboard displays:

- Sensor Address
- Temperature
- Online Status

Temperature values are refreshed automatically.

---

# Restart

The dashboard provides a restart button.

Action:

```
Button

↓

POST /api/restart

↓

ESP Restart
```

---

# Data Refresh

The dashboard periodically requests:

```
GET /api/status
```

All displayed information is generated from the shared runtime state.

---

# Runtime Flow

```
Browser

↓

HTTP

↓

REST API

↓

device_state

↓

JSON

↓

Browser
```

Commands:

```
Browser

↓

REST API

↓

device_commands

↓

Driver

↓

device_state

↓

MQTT Status

↓

Updated Dashboard
```

---

# Source Files

```
src/

web/

    web_server.c
    web_api.c
    web_json.c
    web_pages.c

assets/

    index.html
    style.css
    app.js
```

---

# Future Improvements

Planned features:

- Dark / Light theme
- User authentication
- Live updates using WebSocket
- Historical charts
- Configuration editor
- Home Assistant quick links

---

# Related Documents

- [REST API](api_rest.md)
- [JSON Status](json_status.md)
- [Architecture](architecture.md)
- [Relay](relay.md)
- [PWM](pwm.md)
- [DS18B20](ds18b20.md)

---

← Back to [Documentation Index](README.md)