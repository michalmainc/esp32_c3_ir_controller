#include "device.h"

#include "config/config.h"
#include "pwm/pwm.h"

#include "esp_log.h"


static const char *TAG = "DEVICE";


esp_err_t device_init(void)
{
    ESP_LOGI(TAG, "Inicjalizacja warstwy device");

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
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = pwm_set_percent(
        channel,
        percent
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna ustawic PWM%d: %s",
            channel + 1,
            esp_err_to_name(result)
        );

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

    return pwm_get_percent(channel);
}