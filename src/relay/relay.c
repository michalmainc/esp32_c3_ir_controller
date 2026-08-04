#include "relay.h"

#include "driver/gpio.h"
#include "esp_log.h"


static const char *TAG = "RELAY";

static const gpio_num_t relay_gpio[
    RELAY_CHANNELS
] = {
    GPIO_NUM_6
};

static bool relay_enabled[
    RELAY_CHANNELS
];


esp_err_t relay_init(void)
{
    uint64_t pin_mask = 0;

    for (
        uint8_t channel = 0;
        channel < RELAY_CHANNELS;
        channel++
    )
    {
        pin_mask |= 1ULL << relay_gpio[channel];
        relay_enabled[channel] = false;
    }

    gpio_config_t config = {
        .pin_bit_mask = pin_mask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t result = gpio_config(
        &config
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna skonfigurowac wyjsc relay: %s",
            esp_err_to_name(result)
        );

        return result;
    }

    for (
        uint8_t channel = 0;
        channel < RELAY_CHANNELS;
        channel++
    )
    {
        result = gpio_set_level(
            relay_gpio[channel],
            0
        );

        if (result != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Nie mozna ustawic relay%d w stan OFF: %s",
                channel + 1,
                esp_err_to_name(result)
            );

            return result;
        }
    }

    ESP_LOGI(
        TAG,
        "Uruchomiono %u wyjsc relay, stan poczatkowy OFF",
        RELAY_CHANNELS
    );

    return ESP_OK;
}


esp_err_t relay_set(
    uint8_t channel,
    bool enabled
)
{
    if (channel >= RELAY_CHANNELS)
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = gpio_set_level(
        relay_gpio[channel],
        enabled ? 1 : 0
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna ustawic relay%d: %s",
            channel + 1,
            esp_err_to_name(result)
        );

        return result;
    }

    relay_enabled[channel] = enabled;

    ESP_LOGI(
        TAG,
        "Relay%d: %s",
        channel + 1,
        enabled ? "ON" : "OFF"
    );

    return ESP_OK;
}


bool relay_get(
    uint8_t channel
)
{
    if (channel >= RELAY_CHANNELS)
    {
        return false;
    }

    return relay_enabled[channel];
}