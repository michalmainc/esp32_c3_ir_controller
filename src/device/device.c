#include "device.h"

#include <string.h>

#include "config/config.h"
#include "pwm/pwm.h"

#include "esp_log.h"


static const char *TAG = "DEVICE";

static device_state_t device_state;


static esp_err_t device_apply_pwm(
    uint8_t channel,
    uint8_t percent
)
{
    esp_err_t result = pwm_set_percent(
        channel,
        percent
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna ustawic sprzetowo PWM%d: %s",
            channel + 1,
            esp_err_to_name(result)
        );

        return result;
    }

    return ESP_OK;
}


esp_err_t device_init(void)
{
    ESP_LOGI(TAG, "Inicjalizacja warstwy device");

    memset(
        &device_state,
        0,
        sizeof(device_state)
    );

    for (uint8_t channel = 0;
         channel < PWM_CHANNELS;
         channel++)
    {
        device_state.outputs.pwm[channel] =
            pwm_get_percent(channel);
    }

    return ESP_OK;
}


esp_err_t device_set_pwm(
    uint8_t channel,
    uint8_t percent
)
{
    if (
        channel >= PWM_CHANNELS ||
        percent > 100
    )
    {
        ESP_LOGE(
            TAG,
            "Nieprawidlowe parametry PWM: kanal=%u, wartosc=%u",
            channel,
            percent
        );

        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = device_apply_pwm(
        channel,
        percent
    );

    if (result != ESP_OK)
    {
        return result;
    }

    result = config_set_pwm_value(
        channel,
        percent
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna zapisac PWM%d w konfiguracji: %s",
            channel + 1,
            esp_err_to_name(result)
        );

        return result;
    }

    device_state.outputs.pwm[channel] = percent;

    ESP_LOGI(
        TAG,
        "PWM%d ustawiono na %u%%",
        channel + 1,
        percent
    );

    return ESP_OK;
}


uint8_t device_get_pwm(
    uint8_t channel
)
{
    if (channel >= PWM_CHANNELS)
    {
        return 0;
    }

    return device_state.outputs.pwm[channel];
}


const device_state_t *device_get_state(void)
{
    return &device_state;
}