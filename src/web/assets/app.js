"use strict";

const PWM_CHANNEL_COUNT = 4;
const STATUS_REFRESH_INTERVAL_MS = 5000;
const PWM_SEND_DELAY_MS = 120;

const pwmTimers = Array(PWM_CHANNEL_COUNT).fill(null);
const pwmEditing = Array(PWM_CHANNEL_COUNT).fill(false);

let statusInterval = null;

let relayState = false;


function getElement(id) {
    return document.getElementById(id);
}


function setMessage(text, type = "") {
    const message = getElement("message");

    message.textContent = text;
    message.className = "message";

    if (type === "success") {
        message.classList.add("message-success");
    } else if (type === "error") {
        message.classList.add("message-error");
    }
}


function formatUptime(totalSeconds) {
    const seconds = Number(totalSeconds);

    if (!Number.isFinite(seconds) || seconds < 0) {
        return "--";
    }

    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);

    const parts = [];

    if (days > 0) {
        parts.push(`${days} d`);
    }

    if (hours > 0 || days > 0) {
        parts.push(`${hours} h`);
    }

    parts.push(`${minutes} min`);

    return parts.join(" ");
}


function formatHeap(bytes) {
    const value = Number(bytes);

    if (!Number.isFinite(value) || value < 0) {
        return "--";
    }

    return `${Math.round(value / 1024)} kB`;
}


function formatRssi(rssi) {
    const value = Number(rssi);

    if (!Number.isFinite(value) || value === 0) {
        return "--";
    }

    return `${value} dBm`;
}


function updateDeviceStatus(online) {
    const badge = getElement("deviceStatus");
    const text = getElement("deviceStatusText");

    badge.classList.toggle(
        "status-online",
        online
    );

    badge.classList.toggle(
        "status-offline",
        !online
    );

    text.textContent = online
        ? "ONLINE"
        : "OFFLINE";
}


function updateDevice(device) {
    if (!device) {
        return;
    }

    getElement("deviceName").textContent =
        device.name || "ESP32-C3 IR Controller";

    getElement("deviceSerial").textContent =
        device.serial || "--";
}


function updateWifi(wifi) {
    const connected = Boolean(wifi?.connected);
    const status = getElement("wifiStatus");

    status.textContent = connected
        ? "Połączono"
        : "Rozłączono";

    status.classList.toggle(
        "status-value-online",
        connected
    );

    status.classList.toggle(
        "status-value-offline",
        !connected
    );

    getElement("wifiIp").textContent =
        wifi?.ip || "--";

    getElement("wifiRssi").textContent =
        formatRssi(wifi?.rssi);

    updateDeviceStatus(connected);
}


function updateMqtt(mqtt) {
    const connected = Boolean(mqtt?.connected);
    const status = getElement("mqttStatus");

    status.textContent = connected
        ? "Połączono"
        : "Rozłączono";

    status.classList.toggle(
        "status-value-online",
        connected
    );

    status.classList.toggle(
        "status-value-offline",
        !connected
    );
}


function updateSystem(system) {
    getElement("systemUptime").textContent =
        formatUptime(system?.uptime);

    getElement("systemHeap").textContent =
        formatHeap(system?.free_heap);
}


function updatePwm(outputs) {
    const pwm = outputs?.pwm;

    if (
        !Array.isArray(pwm) ||
        pwm.length < PWM_CHANNEL_COUNT
    ) {
        return;
    }

    for (
        let channel = 0;
        channel < PWM_CHANNEL_COUNT;
        channel++
    ) {
        if (pwmEditing[channel]) {
            continue;
        }

        const value = Number(pwm[channel]);
        const slider = getElement(
            `pwmSlider${channel}`
        );
        const output = getElement(
            `pwmValue${channel}`
        );

        if (
            !Number.isFinite(value) ||
            slider === null ||
            output === null
        ) {
            continue;
        }

        slider.value = String(value);
        output.textContent = `${value}%`;
    }
}


function getTemperatureValueClass(value) {
    if (value < 0) {
        return "temperature-value-cold";
    }

    if (value <= 35) {
        return "temperature-value-normal";
    }

    if (value <= 50) {
        return "temperature-value-warning";
    }

    return "temperature-value-danger";
}


function createTemperatureCard(sensor) {
    const card = document.createElement("article");
    card.className = "temperature-sensor";

    if (!sensor.present) {
        card.classList.add(
            "temperature-sensor-offline"
        );
    }

    const name = document.createElement("div");
    name.className = "temperature-name";
    name.textContent = sensor.name || "DS18B20";

    const address = document.createElement("div");
    address.className = "temperature-address";
    address.textContent =
        sensor.address || "Nieznany adres";

    const footer = document.createElement("div");
    footer.className = "temperature-footer";

    const state = document.createElement("span");
    state.className = "temperature-state";

    const value = document.createElement("span");
    value.className = "temperature-value";

    if (sensor.present) {
        const numericValue = Number(sensor.value);

        state.textContent = "Online";

        if (Number.isFinite(numericValue)) {
            value.textContent =
                `${numericValue.toFixed(2)} °C`;

            value.classList.add(
                getTemperatureValueClass(
                    numericValue
                )
            );
        } else {
            value.textContent = "--";
        }
    } else {
        state.textContent = "Brak odczytu";

        state.classList.add(
            "temperature-state-offline"
        );

        value.textContent = "Offline";

        value.classList.add(
            "temperature-value-danger"
        );
    }

    footer.appendChild(state);
    footer.appendChild(value);

    card.appendChild(name);
    card.appendChild(address);
    card.appendChild(footer);

    return card;
}


function updateTemperatures(temperature) {
    const container = getElement(
        "temperatureSensors"
    );

    const sensors = temperature?.sensors;

    container.replaceChildren();

    if (
        !Array.isArray(sensors) ||
        sensors.length === 0
    ) {
        const emptyState =
            document.createElement("div");

        emptyState.className = "empty-state";
        emptyState.textContent =
            "Nie wykryto czujników DS18B20";

        container.appendChild(emptyState);

        return;
    }

    for (const sensor of sensors) {
        container.appendChild(
            createTemperatureCard(sensor)
        );
    }
}


async function loadStatus() {
    try {
        const response = await fetch(
            "/api/status",
            {
                cache: "no-store"
            }
        );

        if (!response.ok) {
            throw new Error(
                `HTTP ${response.status}`
            );
        }

        const data = await response.json();

        updateDevice(data.device);
        updateWifi(data.wifi);
        updateMqtt(data.mqtt);
        updatePwm(data.outputs);
        updateRelay(data.outputs);
        updateTemperatures(data.temperature);
        updateSystem(data.system);

        setMessage("");
    } catch (error) {
        updateDeviceStatus(false);

        setMessage(
            "Nie można pobrać aktualnego stanu urządzenia",
            "error"
        );

        console.error(error);
    }
}


async function setPwm(channel, value) {
    pwmEditing[channel] = true;

    try {
        const response = await fetch(
            `/api/pwm?channel=${channel}&value=${value}`,
            {
                method: "POST"
            }
        );

        if (!response.ok) {
            throw new Error(
                `HTTP ${response.status}`
            );
        }

        setMessage(
            `PWM${channel + 1} ustawiono na ${value}%`,
            "success"
        );
    } catch (error) {
        setMessage(
            `Błąd ustawiania PWM${channel + 1}`,
            "error"
        );

        console.error(error);
    } finally {
        pwmEditing[channel] = false;
    }
}


function initializePwmControls() {
    for (
        let channel = 0;
        channel < PWM_CHANNEL_COUNT;
        channel++
    ) {
        const slider = getElement(
            `pwmSlider${channel}`
        );

        const output = getElement(
            `pwmValue${channel}`
        );

        slider.addEventListener(
            "input",
            () => {
                const value = slider.value;

                pwmEditing[channel] = true;
                output.textContent = `${value}%`;

                clearTimeout(
                    pwmTimers[channel]
                );

                pwmTimers[channel] = setTimeout(
                    () => setPwm(
                        channel,
                        value
                    ),
                    PWM_SEND_DELAY_MS
                );
            }
        );

        slider.addEventListener(
            "change",
            () => {
                clearTimeout(
                    pwmTimers[channel]
                );

                setPwm(
                    channel,
                    slider.value
                );
            }
        );
    }
}


async function restartDevice() {
    const button = getElement(
        "restartButton"
    );

    button.disabled = true;

    setMessage(
        "Restartowanie urządzenia...",
        "success"
    );

    if (statusInterval !== null) {
        clearInterval(statusInterval);
        statusInterval = null;
    }

    try {
        await fetch(
            "/api/restart",
            {
                method: "POST"
            }
        );
    } catch (error) {
        console.debug(
            "Połączenie przerwane podczas restartu",
            error
        );
    }

    setTimeout(
        () => window.location.reload(),
        7000
    );
}


function initializePage() {
    initializePwmControls();

    getElement(
        "restartButton"
    ).addEventListener(
        "click",
        restartDevice
    );

    getElement(
        "relayButton0"
    ).addEventListener(
        "click",
        () => {
            setRelay(
                0,
                !relayState
            );
        }
    );

    loadStatus();

    statusInterval = setInterval(
        loadStatus,
        STATUS_REFRESH_INTERVAL_MS
    );
}


window.addEventListener(
    "DOMContentLoaded",
    initializePage
);

function updateRelay(outputs) {
    const relay = outputs?.relay;

    if (
        !Array.isArray(relay) ||
        relay.length < 1
    ) {
        return;
    }

    relayState = Boolean(relay[0]);

    const button = getElement("relayButton0");

    button.textContent =
        relayState ? "ON" : "OFF";

    button.classList.toggle(
        "relay-on",
        relayState
    );

    button.classList.toggle(
        "relay-off",
        !relayState
    );
}

async function setRelay(channel, state) {
    const button = getElement(
        `relayButton${channel}`
    );

    button.disabled = true;

    try {
        const response = await fetch(
            "/api/relay",
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    channel: channel + 1,
                    state: state
                })
            }
        );

        if (!response.ok) {
            throw new Error(
                `HTTP ${response.status}`
            );
        }

        relayState = state;

        updateRelay({
            relay: [relayState]
        });

        setMessage(
            `Relay${channel + 1} ustawiono na ${state ? "ON" : "OFF"}`,
            "success"
        );
    } catch (error) {
        setMessage(
            `Błąd ustawiania Relay${channel + 1}`,
            "error"
        );

        console.error(error);
    } finally {
        button.disabled = false;
    }
}