#include "device_commands.h"

#include "device.h"

#include "relay/relay.h"

#include "esp_log.h"


static const char *TAG = "DEVICE_CMD";


esp_err_t device_command_set_pwm(
    uint8_t channel,
    uint8_t percent
)
{
    esp_err_t result = device_set_pwm(
        channel,
        percent
    );

    if (result != ESP_OK)
    {
        return result;
    }

    ESP_LOGI(
        TAG,
        "PWM%d -> %d%%",
        channel + 1,
        percent
    );

    return ESP_OK;
}


esp_err_t device_command_set_relay(
    uint8_t channel,
    bool enabled
)
{
    if (channel >= RELAY_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = relay_set(
        channel,
        enabled
    );

    if (result != ESP_OK)
    {
        return result;
    }

    device_set_relay(
        channel,
        enabled
    );

    ESP_LOGI(
        TAG,
        "Relay%d -> %s",
        channel + 1,
        enabled ? "ON" : "OFF"
    );

    return ESP_OK;
}
