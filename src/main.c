#include "config/config.h"
#include "pwm/pwm.h"
#include "wifi/wifi_manager.h"
#include "web/web_server.h"

#include <stdbool.h>

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "device/device.h"


static const char *TAG = "MAIN";

static bool web_started = false;


static void heartbeat_task(void *pv_parameter)
{
    unsigned long counter = 0;

    (void)pv_parameter;

    while (1)
    {
        counter++;

        ESP_LOGI(
            "HEARTBEAT",
            "Licznik: %lu, Wi-Fi: %s, WWW: %s, "
            "PWM: [%u%%, %u%%, %u%%, %u%%]",
            counter,
            wifi_manager_is_connected()
                ? "CONNECTED"
                : "DISCONNECTED",
            web_started
                ? "STARTED"
                : "STOPPED",
            pwm_get_percent(0),
            pwm_get_percent(1),
            pwm_get_percent(2),
            pwm_get_percent(3)
        );

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


void app_main(void)
{
    ESP_LOGI(TAG, "Start aplikacji ESP32-C3");

    config_init();

    device_config_t *cfg = config_get();

    if (cfg == NULL)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna pobrac konfiguracji"
        );

        return;
    }

    ESP_LOGI(
        TAG,
        "Device: %s",
        cfg->device_name
    );

    esp_err_t result = pwm_init();

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie udalo sie uruchomic PWM: %s",
            esp_err_to_name(result)
        );

        return;
    }

    /*
     * Przywrócenie wartości zapisanych w NVS.
     */
    for (uint8_t channel = 0;
         channel < PWM_CHANNELS;
         channel++)
    {
        result = pwm_set_percent(
            channel,
            cfg->pwm_value[channel]
        );

        if (result != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Nie mozna przywrocic PWM%d",
                channel + 1
            );

            return;
        }
    }

    result = device_init();

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie udalo sie uruchomic warstwy device: %s",
            esp_err_to_name(result)
        );

        return;
    }

    result = wifi_manager_init();

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie udalo sie uruchomic Wi-Fi: %s",
            esp_err_to_name(result)
        );

        return;
    }

    result = web_server_start();

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie udalo sie uruchomic WWW: %s",
            esp_err_to_name(result)
        );

        return;
    }

    web_started = true;

    BaseType_t task_result = xTaskCreate(
        heartbeat_task,
        "heartbeat_task",
        3072,
        NULL,
        5,
        NULL
    );

    if (task_result != pdPASS)
    {
        ESP_LOGE(
            TAG,
            "Nie udalo sie utworzyc heartbeat_task"
        );

        return;
    }

    ESP_LOGI(TAG, "Aplikacja uruchomiona");
}