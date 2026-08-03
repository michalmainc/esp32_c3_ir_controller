#include "device.h"

#include <string.h>

#include "config/config.h"
#include "pwm/pwm.h"
#include "wifi/wifi_manager.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_mac.h"


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


static void device_update_system_state(void)
{
    device_state.system.uptime =
        (uint32_t)(esp_timer_get_time() / 1000000ULL);

    device_state.system.free_heap =
        esp_get_free_heap_size();
}


static void device_update_wifi_state(void)
{
    device_state.wifi.connected =
        wifi_manager_is_connected();

    if (!device_state.wifi.connected)
    {
        device_state.wifi.rssi = 0;
        device_state.wifi.ip[0] = '\0';
        return;
    }

    if (
        wifi_manager_get_ip(
            device_state.wifi.ip,
            sizeof(device_state.wifi.ip)
        ) != ESP_OK
    )
    {
        device_state.wifi.ip[0] = '\0';
    }

    if (
        wifi_manager_get_rssi(
            &device_state.wifi.rssi
        ) != ESP_OK
    )
    {
        device_state.wifi.rssi = 0;
    }
}


esp_err_t device_init(void)
{
    ESP_LOGI(TAG, "Inicjalizacja warstwy device");

    memset(
        &device_state,
        0,
        sizeof(device_state)
    );

    strcpy(
        device_state.device.name,
        config_get()->device_name
    );

    uint8_t mac[6];

    esp_err_t result = esp_read_mac(
        mac,
        ESP_MAC_WIFI_STA
    );

    if (result == ESP_OK)
    {
        snprintf(
            device_state.device.serial,
            sizeof(device_state.device.serial),
            "ESP32-%02X%02X%02X%02X%02X%02X",
            mac[0],
            mac[1],
            mac[2],
            mac[3],
            mac[4],
            mac[5]
        );
    }
    else
    {
        strcpy(
            device_state.device.serial,
            "UNKNOWN"
        );
    }

    for (
        uint8_t channel = 0;
        channel < PWM_CHANNELS;
        channel++
    )
    {
        device_state.outputs.pwm[channel] =
            pwm_get_percent(channel);
    }

    device_update_system_state();
    device_update_wifi_state();

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
    device_update_system_state();
    device_update_wifi_state();

    return &device_state;
}

void device_set_temperature_count(
    uint8_t count
)
{
    if (count > DEVICE_MAX_TEMPERATURE_SENSORS)
    {
        count = DEVICE_MAX_TEMPERATURE_SENSORS;
    }

    device_state.temperature.count = count;
}

void device_update_temperature_sensor(
    uint8_t index,
    uint64_t address,
    float value,
    bool present
)
{
    if (index >= DEVICE_MAX_TEMPERATURE_SENSORS)
    {
        return;
    }

    device_state.temperature.sensors[index].address = address;
    device_state.temperature.sensors[index].value = value;
    device_state.temperature.sensors[index].present = present;

    if (
        present &&
        device_state.temperature.count < (index + 1)
    )
    {
        device_state.temperature.count = index + 1;
    }
}