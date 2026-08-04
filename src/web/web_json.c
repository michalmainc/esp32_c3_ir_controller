#include "web_json.h"

#include <stdio.h>

#include "cJSON.h"

#include <stdlib.h>
#include <string.h>

#include "onewire/onewire_utils.h"


static cJSON *build_device_json(
    const device_state_t *state
)
{
    cJSON *device = cJSON_CreateObject();

    if (device == NULL)
    {
        return NULL;
    }

    if (
        cJSON_AddStringToObject(
            device,
            "name",
            state->device.name
        ) == NULL ||
        cJSON_AddStringToObject(
            device,
            "serial",
            state->device.serial
        ) == NULL
    )
    {
        cJSON_Delete(device);
        return NULL;
    }

    return device;
}


static cJSON *build_wifi_json(
    const device_state_t *state
)
{
    cJSON *wifi = cJSON_CreateObject();

    if (wifi == NULL)
    {
        return NULL;
    }

    if (
        cJSON_AddBoolToObject(
            wifi,
            "connected",
            state->wifi.connected
        ) == NULL ||
        cJSON_AddStringToObject(
            wifi,
            "ip",
            state->wifi.ip
        ) == NULL ||
        cJSON_AddNumberToObject(
            wifi,
            "rssi",
            state->wifi.rssi
        ) == NULL
    )
    {
        cJSON_Delete(wifi);
        return NULL;
    }

    return wifi;
}


static cJSON *build_mqtt_json(
    const device_state_t *state
)
{
    cJSON *mqtt = cJSON_CreateObject();

    if (mqtt == NULL)
    {
        return NULL;
    }

    if (
        cJSON_AddBoolToObject(
            mqtt,
            "connected",
            state->mqtt.connected
        ) == NULL
    )
    {
        cJSON_Delete(mqtt);
        return NULL;
    }

    return mqtt;
}


static cJSON *build_outputs_json(
    const device_state_t *state
)
{
    cJSON *outputs = cJSON_CreateObject();

    if (outputs == NULL)
    {
        return NULL;
    }

    cJSON *pwm = cJSON_CreateArray();

    if (pwm == NULL)
    {
        cJSON_Delete(outputs);
        return NULL;
    }

    for (
        uint8_t channel = 0;
        channel < PWM_CHANNELS;
        channel++
    )
    {
        cJSON *value = cJSON_CreateNumber(
            state->outputs.pwm[channel]
        );

        if (value == NULL)
        {
            cJSON_Delete(pwm);
            cJSON_Delete(outputs);
            return NULL;
        }

        cJSON_AddItemToArray(
            pwm,
            value
        );
    }

    cJSON_AddItemToObject(
        outputs,
        "pwm",
        pwm
    );

    cJSON *relay = cJSON_CreateArray();

    if (relay == NULL)
    {
        cJSON_Delete(outputs);
        return NULL;
    }

    for (
        uint8_t channel = 0;
        channel < RELAY_CHANNELS;
        channel++
    )
    {
        cJSON *state_json = cJSON_CreateBool(
            state->outputs.relay[channel]
        );

        if (state_json == NULL)
        {
            cJSON_Delete(relay);
            cJSON_Delete(outputs);
            return NULL;
        }

        cJSON_AddItemToArray(
            relay,
            state_json
        );
    }

    cJSON_AddItemToObject(
        outputs,
        "relay",
        relay
    );

    return outputs;
}


static cJSON *build_temperature_json(
    const device_state_t *state
)
{
    cJSON *temperature = cJSON_CreateObject();

    if (temperature == NULL)
    {
        return NULL;
    }

    if (
        cJSON_AddNumberToObject(
            temperature,
            "count",
            state->temperature.count
        ) == NULL
    )
    {
        cJSON_Delete(temperature);
        return NULL;
    }

    cJSON *sensors = cJSON_CreateArray();

    if (sensors == NULL)
    {
        cJSON_Delete(temperature);
        return NULL;
    }

    for (
        uint8_t index = 0;
        index < state->temperature.count &&
        index < DEVICE_MAX_TEMPERATURE_SENSORS;
        index++
    )
    {
        const device_temperature_sensor_t *sensor =
            &state->temperature.sensors[index];

        cJSON *sensor_json = cJSON_CreateObject();

        if (sensor_json == NULL)
        {
            cJSON_Delete(sensors);
            cJSON_Delete(temperature);
            return NULL;
        }

        char address[ONEWIRE_ADDRESS_STRING_SIZE];

        esp_err_t address_result =
            onewire_address_to_string(
                sensor->address,
                address,
                sizeof(address)
            );

        if (address_result != ESP_OK)
        {
            cJSON_Delete(sensor_json);
            cJSON_Delete(sensors);
            cJSON_Delete(temperature);

            return NULL;
        }

        if (
            cJSON_AddStringToObject(
                sensor_json,
                "address",
                address
            ) == NULL ||
            cJSON_AddBoolToObject(
                sensor_json,
                "present",
                sensor->present
            ) == NULL ||
            cJSON_AddNumberToObject(
                sensor_json,
                "value",
                sensor->value
            ) == NULL
        )
        {
            cJSON_Delete(sensor_json);
            cJSON_Delete(sensors);
            cJSON_Delete(temperature);
            return NULL;
        }

        cJSON_AddItemToArray(
            sensors,
            sensor_json
        );
    }

    cJSON_AddItemToObject(
        temperature,
        "sensors",
        sensors
    );

    return temperature;
}


static cJSON *build_system_json(
    const device_state_t *state
)
{
    cJSON *system = cJSON_CreateObject();

    if (system == NULL)
    {
        return NULL;
    }

    if (
        cJSON_AddNumberToObject(
            system,
            "uptime",
            state->system.uptime
        ) == NULL ||
        cJSON_AddNumberToObject(
            system,
            "free_heap",
            state->system.free_heap
        ) == NULL
    )
    {
        cJSON_Delete(system);
        return NULL;
    }

    return system;
}


char *web_json_build_status(
    const device_state_t *state
)
{
    if (state == NULL)
    {
        return NULL;
    }

    cJSON *root = cJSON_CreateObject();

    if (root == NULL)
    {
        return NULL;
    }

    cJSON *device = build_device_json(state);
    cJSON *wifi = build_wifi_json(state);
    cJSON *mqtt = build_mqtt_json(state);
    cJSON *outputs = build_outputs_json(state);
    cJSON *inputs = cJSON_CreateObject();
    cJSON *temperature = build_temperature_json(state);
    cJSON *system = build_system_json(state);

    if (
        device == NULL ||
        wifi == NULL ||
        mqtt == NULL ||
        outputs == NULL ||
        inputs == NULL ||
        temperature == NULL ||
        system == NULL
    )
    {
        cJSON_Delete(device);
        cJSON_Delete(wifi);
        cJSON_Delete(mqtt);
        cJSON_Delete(outputs);
        cJSON_Delete(inputs);
        cJSON_Delete(temperature);
        cJSON_Delete(system);
        cJSON_Delete(root);

        return NULL;
    }

    cJSON_AddItemToObject(root, "device", device);
    cJSON_AddItemToObject(root, "wifi", wifi);
    cJSON_AddItemToObject(root, "mqtt", mqtt);
    cJSON_AddItemToObject(root, "outputs", outputs);
    cJSON_AddItemToObject(root, "inputs", inputs);
    cJSON_AddItemToObject(root, "temperature", temperature);
    cJSON_AddItemToObject(root, "system", system);

    char *json_text =
        cJSON_PrintUnformatted(root);

    cJSON_Delete(root);

    if (json_text == NULL)
    {
        return NULL;
    }

    size_t json_length = strlen(json_text);

    char *json_with_newline = malloc(
        json_length + 2
    );

    if (json_with_newline == NULL)
    {
        cJSON_free(json_text);

        return NULL;
    }

    memcpy(
        json_with_newline,
        json_text,
        json_length
    );

    json_with_newline[json_length] = '\n';
    json_with_newline[json_length + 1] = '\0';

    cJSON_free(json_text);

    return json_with_newline;
}