#include "config/config.h"
#include "pwm/pwm.h"
#include "wifi/wifi_manager.h"

#include <stdbool.h>

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "MAIN";


static void heartbeat_task(void *pv_parameter)
{
    unsigned long counter = 0;

    (void)pv_parameter;

    while (1)
    {
        counter++;

        ESP_LOGI(
            "HEARTBEAT",
            "Uklad pracuje, licznik: %lu, Wi-Fi: %s",
            counter,
            wifi_manager_is_connected()
                ? "CONNECTED"
                : "DISCONNECTED"
        );

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


static void pwm_test_task(void *pv_parameter)
{
    (void)pv_parameter;

    vTaskDelay(pdMS_TO_TICKS(2000));

    ESP_LOGI(TAG, "Rozpoczynam test kanalow PWM");

    pwm_set_percent(0, 25);
    pwm_set_percent(1, 50);
    pwm_set_percent(2, 75);
    pwm_set_percent(3, 100);

    ESP_LOGI(TAG, "Testowe wartosci PWM ustawione");

    vTaskDelete(NULL);
}


void app_main(void)
{
    ESP_LOGI(TAG, "Start aplikacji ESP32-C3");

    config_init();

    device_config_t *cfg = config_get();

    if (cfg == NULL)
    {
        ESP_LOGE(TAG, "Nie mozna pobrac konfiguracji");
        return;
    }

    ESP_LOGI(TAG, "Device: %s", cfg->device_name);
    ESP_LOGI(TAG, "IP: %s", cfg->ip);

    ESP_LOGI(
        TAG,
        "MQTT: %s:%d",
        cfg->mqtt_host,
        cfg->mqtt_port
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

    ESP_LOGI(TAG, "Inicjalizacja Wi-Fi...");

    esp_err_t wifi_result = wifi_manager_init();

    if (wifi_result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie udalo sie zainicjalizowac Wi-Fi: %s",
            esp_err_to_name(wifi_result)
        );

        return;
    }

    BaseType_t task_result = xTaskCreate(
        heartbeat_task,
        "heartbeat_task",
        2048,
        NULL,
        5,
        NULL
    );

    if (task_result != pdPASS)
    {
        ESP_LOGE(TAG, "Nie udalo sie utworzyc heartbeat_task");
        return;
    }

    task_result = xTaskCreate(
        pwm_test_task,
        "pwm_test_task",
        2048,
        NULL,
        5,
        NULL
    );

    if (task_result != pdPASS)
    {
        ESP_LOGE(TAG, "Nie udalo sie utworzyc pwm_test_task");
        return;
    }

    ESP_LOGI(TAG, "Aplikacja uruchomiona");
}