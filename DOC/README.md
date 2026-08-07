# Documentation

> ESP32-C3 Controller Documentation

This directory contains the complete technical documentation for the **ESP32-C3 Controller** project.

The documentation is divided into individual topics to make navigation easier and to keep each document focused on a single subject.

---

# Table of Contents

## Getting Started

| Document | Description |
|----------|-------------|
| [Build Guide](build.md) | Build, upload and development environment |
| [Configuration](configuration.md) | Device configuration stored in NVS |
| [Hardware](hardware.md) | Hardware overview and GPIO assignment |

---

## Firmware

| Document | Description |
|----------|-------------|
| [Architecture](architecture.md) | Firmware architecture and module interaction |
| [JSON Status](json_status.md) | Complete status JSON description |

---

## Communication Interfaces

| Document | Description |
|----------|-------------|
| [REST API](api_rest.md) | HTTP REST interface |
| [MQTT API](api_mqtt.md) | MQTT topics and payloads |

---

## Functional Modules

| Document | Description |
|----------|-------------|
| [PWM](pwm.md) | PWM output subsystem |
| [Relay](relay.md) | Relay output subsystem |
| [DS18B20](ds18b20.md) | Temperature measurement subsystem |
| [Web Dashboard](web_dashboard.md) | Embedded Web User Interface |

---

## Future Modules

| Document | Description |
|----------|-------------|
| [Astronomical Scheduler](astronomy.md) | Sunrise and sunset calculations |
| [Home Assistant](home_assistant.md) | Home Assistant integration |
| [OTA Update](ota.md) | Firmware Over-The-Air update |
| [Scheduler](scheduler.md) | Automation scheduler |

---

# Documentation Structure

```text
docs/

├── README.md
│
├── architecture.md
├── hardware.md
├── build.md
├── configuration.md
│
├── api_rest.md
├── api_mqtt.md
├── json_status.md
│
├── pwm.md
├── relay.md
├── ds18b20.md
├── web_dashboard.md
│
├── astronomy.md
├── home_assistant.md
├── ota.md
├── scheduler.md
│
└── images/
```

---

# Documentation Conventions

The following conventions are used throughout the documentation.

## Markdown

All documentation is written in standard GitHub-compatible Markdown.

---

## Code Examples

Source code examples include syntax highlighting.

Examples:

```c
void relay_set(uint8_t channel, bool state);
```

```json
{
    "channel": 1,
    "state": true
}
```

```bash
curl http://192.168.1.100/api/status
```

---

## Relative Links

All links inside the documentation use relative paths.

Example:

```text
[REST API](api_rest.md)
```

This allows the documentation to work correctly on:

- GitHub
- GitLab
- Local Markdown viewers
- Visual Studio Code

---

## Examples

Practical examples are stored separately inside the project.

```text
examples/

├── curl/
└── mqtt/
```

Whenever possible, the documentation refers to these examples instead of duplicating code.

---

# Development Workflow

When a new feature is added to the firmware, the documentation should be updated accordingly.

Typical workflow:

1. Design
2. Implementation
3. Testing
4. Documentation
5. Commit
6. Release

---

# Versioning

Documentation follows the firmware version.

Each firmware release should contain matching documentation.

---

# Navigation

← Back to the project homepage

[Project README](../README.md)