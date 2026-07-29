#include "config.h"

#include <string.h>

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"


#define NVS_NAMESPACE        "config"
#define CONFIG_SAVE_DELAY_MS 5000


static const char *TAG = "CONFIG";

static device_config_t device_config;
static TimerHandle_t save_timer = NULL;


static void load_defaults(void)
{
    memset(
        &device_config,
        0,
        sizeof(device_config)
    );

    strcpy(
        device_config.device_name,
        "esp32_c3"
    );

    device_config.latitude = 52.8597009f;
    device_config.longitude = 16.0817148f;

    strcpy(
        device_config.mqtt_host,
        "192.168.1.182"
    );

    device_config.mqtt_port = 1883;

    strcpy(device_config.pwm_name[0], "PWM1");
    strcpy(device_config.pwm_name[1], "PWM2");
    strcpy(device_config.pwm_name[2], "PWM3");
    strcpy(device_config.pwm_name[3], "PWM4");

    for (uint8_t i = 0; i < PWM_CHANNELS; i++)
    {
        device_config.pwm_value[i] = 0;
    }

    strcpy(
        device_config.temperature_name,
        "DS18B20"
    );
}


void config_save(void)
{
    nvs_handle_t handle;

    esp_err_t err = nvs_open(
        NVS_NAMESPACE,
        NVS_READWRITE,
        &handle
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna otworzyc NVS: %s",
            esp_err_to_name(err)
        );

        return;
    }

    err = nvs_set_blob(
        handle,
        "device",
        &device_config,
        sizeof(device_config)
    );

    if (err == ESP_OK)
    {
        err = nvs_commit(handle);
    }

    nvs_close(handle);

    if (err == ESP_OK)
    {
        ESP_LOGI(
            TAG,
            "Konfiguracja zapisana do NVS"
        );
    }
    else
    {
        ESP_LOGE(
            TAG,
            "Blad zapisu konfiguracji: %s",
            esp_err_to_name(err)
        );
    }
}


static void save_timer_callback(TimerHandle_t timer)
{
    (void)timer;

    config_save();
}


void config_init(void)
{
    esp_err_t err = nvs_flash_init();

    if (
        err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND
    )
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);

    load_defaults();

    nvs_handle_t handle;

    err = nvs_open(
        NVS_NAMESPACE,
        NVS_READWRITE,
        &handle
    );

    if (err == ESP_OK)
    {
        size_t size = sizeof(device_config);

        err = nvs_get_blob(
            handle,
            "device",
            &device_config,
            &size
        );

        if (err != ESP_OK)
        {
            load_defaults();

            nvs_set_blob(
                handle,
                "device",
                &device_config,
                sizeof(device_config)
            );

            nvs_commit(handle);

            ESP_LOGI(
                TAG,
                "Zapisano konfiguracje domyslna"
            );
        }
        else
        {
            ESP_LOGI(
                TAG,
                "Wczytano konfiguracje z NVS"
            );
        }

        nvs_close(handle);
    }
    else
    {
        ESP_LOGE(
            TAG,
            "Nie mozna otworzyc NVS: %s",
            esp_err_to_name(err)
        );
    }

    save_timer = xTimerCreate(
        "config_save",
        pdMS_TO_TICKS(CONFIG_SAVE_DELAY_MS),
        pdFALSE,
        NULL,
        save_timer_callback
    );

    if (save_timer == NULL)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna utworzyc timera zapisu NVS"
        );
    }
}


device_config_t *config_get(void)
{
    return &device_config;
}


esp_err_t config_set_pwm_value(
    uint8_t channel,
    uint8_t value
)
{
    if (
        channel >= PWM_CHANNELS ||
        value > 100
    )
    {
        return ESP_ERR_INVALID_ARG;
    }

    device_config.pwm_value[channel] = value;

    if (save_timer == NULL)
    {
        ESP_LOGE(
            TAG,
            "Timer zapisu NVS nie jest dostepny"
        );

        return ESP_ERR_INVALID_STATE;
    }

    if (
        xTimerReset(save_timer, 0) != pdPASS
    )
    {
        ESP_LOGE(
            TAG,
            "Nie mozna uruchomic timera zapisu"
        );

        return ESP_FAIL;
    }

    ESP_LOGI(
        TAG,
        "PWM%d=%u%%, zapis NVS za 5 sekund",
        channel + 1,
        value
    );

    return ESP_OK;
}