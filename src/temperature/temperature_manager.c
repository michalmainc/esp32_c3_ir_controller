#include "temperature_manager.h"

#include "device/device.h"

#include "ds18b20.h"
#include "onewire_bus.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define TEMPERATURE_ONEWIRE_GPIO       7
#define TEMPERATURE_READ_INTERVAL_MS   5000
#define TEMPERATURE_TASK_STACK_SIZE    4096
#define TEMPERATURE_TASK_PRIORITY      5


static const char *TAG = "TEMPERATURE";

static onewire_bus_handle_t onewire_bus = NULL;

static ds18b20_device_handle_t sensors[
    TEMPERATURE_MAX_SENSORS
];

static onewire_device_address_t sensor_addresses[
    TEMPERATURE_MAX_SENSORS
];

static uint8_t sensor_count = 0;


static void temperature_task(void *parameter)
{
    (void)parameter;

    while (1)
    {
        if (
            onewire_bus == NULL ||
            sensor_count == 0
        )
        {
            vTaskDelay(
                pdMS_TO_TICKS(
                    TEMPERATURE_READ_INTERVAL_MS
                )
            );

            continue;
        }

        esp_err_t result =
            ds18b20_trigger_temperature_conversion_for_all(
                onewire_bus
            );

        if (result != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Blad rozpoczecia konwersji temperatury: %s",
                esp_err_to_name(result)
            );

            vTaskDelay(
                pdMS_TO_TICKS(
                    TEMPERATURE_READ_INTERVAL_MS
                )
            );

            continue;
        }

        for (
            uint8_t index = 0;
            index < sensor_count;
            index++
        )
        {
            float temperature = 0.0f;

            result = ds18b20_get_temperature(
                sensors[index],
                &temperature
            );

            if (result != ESP_OK)
            {
                ESP_LOGW(
                    TAG,
                    "Blad odczytu czujnika %016llX: %s",
                    (unsigned long long)sensor_addresses[index],
                    esp_err_to_name(result)
                );

                device_update_temperature_sensor(
                    index,
                    (uint64_t)sensor_addresses[index],
                    0.0f,
                    false
                );

                continue;
            }

            device_update_temperature_sensor(
                index,
                (uint64_t)sensor_addresses[index],
                temperature,
                true
            );

            ESP_LOGI(
                TAG,
                "DS18B20 %016llX: %.2f C",
                (unsigned long long)sensor_addresses[index],
                temperature
            );
        }

        vTaskDelay(
            pdMS_TO_TICKS(
                TEMPERATURE_READ_INTERVAL_MS
            )
        );
    }
}


esp_err_t temperature_manager_init(void)
{
    ESP_LOGI(
        TAG,
        "Inicjalizacja magistrali 1-Wire na GPIO%d",
        TEMPERATURE_ONEWIRE_GPIO
    );

    sensor_count = 0;

    for (
        uint8_t index = 0;
        index < TEMPERATURE_MAX_SENSORS;
        index++
    )
    {
        sensors[index] = NULL;
        sensor_addresses[index] = 0;
    }

    onewire_bus_config_t bus_config = {
        .bus_gpio_num = TEMPERATURE_ONEWIRE_GPIO,
        .flags = {
            .en_pull_up = true
        }
    };

    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10
    };

    esp_err_t result = onewire_new_bus_rmt(
        &bus_config,
        &rmt_config,
        &onewire_bus
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna utworzyc magistrali 1-Wire: %s",
            esp_err_to_name(result)
        );

        return result;
    }

    onewire_device_iter_handle_t iterator = NULL;

    result = onewire_new_device_iter(
        onewire_bus,
        &iterator
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna utworzyc iteratora 1-Wire: %s",
            esp_err_to_name(result)
        );

        return result;
    }

    ESP_LOGI(
        TAG,
        "Rozpoczynam wyszukiwanie czujnikow"
    );

    while (sensor_count < TEMPERATURE_MAX_SENSORS)
    {
        onewire_device_t discovered_device;

        result = onewire_device_iter_get_next(
            iterator,
            &discovered_device
        );

        if (result == ESP_ERR_NOT_FOUND)
        {
            break;
        }

        if (result != ESP_OK)
        {
            ESP_LOGW(
                TAG,
                "Blad przeszukiwania magistrali: %s",
                esp_err_to_name(result)
            );

            break;
        }

        ds18b20_config_t sensor_config;
        ds18b20_device_handle_t sensor = NULL;

        result = ds18b20_new_device_from_enumeration(
            &discovered_device,
            &sensor_config,
            &sensor
        );

        if (result != ESP_OK)
        {
            ESP_LOGW(
                TAG,
                "Pominieto nieznane urzadzenie 1-Wire: %s",
                esp_err_to_name(result)
            );

            continue;
        }

        onewire_device_address_t address = 0;

        result = ds18b20_get_device_address(
            sensor,
            &address
        );

        if (result != ESP_OK)
        {
            ESP_LOGW(
                TAG,
                "Nie mozna odczytac adresu czujnika: %s",
                esp_err_to_name(result)
            );

            ds18b20_del_device(sensor);

            continue;
        }

        sensors[sensor_count] = sensor;
        sensor_addresses[sensor_count] = address;

        device_update_temperature_sensor(
            sensor_count,
            (uint64_t)address,
            0.0f,
            true
        );

        ESP_LOGI(
            TAG,
            "Znaleziono DS18B20[%u], adres: %016llX",
            sensor_count,
            (unsigned long long)address
        );

        sensor_count++;
    }

    esp_err_t iterator_result =
        onewire_del_device_iter(iterator);

    if (iterator_result != ESP_OK)
    {
        ESP_LOGW(
            TAG,
            "Nie mozna usunac iteratora: %s",
            esp_err_to_name(iterator_result)
        );
    }

    ESP_LOGI(
        TAG,
        "Wyszukiwanie zakonczone, znaleziono %u czujnikow",
        sensor_count
    );

    device_set_temperature_count(sensor_count);

    if (sensor_count == 0)
    {
        ESP_LOGW(
            TAG,
            "Nie znaleziono czujnikow DS18B20"
        );

        return ESP_OK;
    }

    BaseType_t task_result = xTaskCreate(
        temperature_task,
        "temperature_task",
        TEMPERATURE_TASK_STACK_SIZE,
        NULL,
        TEMPERATURE_TASK_PRIORITY,
        NULL
    );

    if (task_result != pdPASS)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna utworzyc zadania temperatury"
        );

        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(
        TAG,
        "Uruchomiono cykliczny odczyt temperatury"
    );

    return ESP_OK;
}


uint8_t temperature_manager_get_sensor_count(void)
{
    return sensor_count;
}