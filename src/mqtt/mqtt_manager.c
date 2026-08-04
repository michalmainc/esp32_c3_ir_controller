#include "mqtt_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config/config.h"
#include "device/device.h"
#include "web/web_json.h"

#include "cJSON.h"

#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define MQTT_STATUS_INTERVAL_MS 10000
#define MQTT_TASK_STACK_SIZE    4096
#define MQTT_TASK_PRIORITY      5

#define MQTT_TOPIC_SIZE         96
#define MQTT_COMMAND_SIZE       128


static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;

static char broker_uri[96];
static char status_topic[MQTT_TOPIC_SIZE];
static char pwm_command_topic[MQTT_TOPIC_SIZE];


static bool mqtt_topic_equals(
    const esp_mqtt_event_handle_t event,
    const char *expected_topic
)
{
    if (
        event == NULL ||
        expected_topic == NULL ||
        event->topic == NULL
    )
    {
        return false;
    }

    size_t expected_length =
        strlen(expected_topic);

    return (
        event->topic_len == (int)expected_length &&
        memcmp(
            event->topic,
            expected_topic,
            expected_length
        ) == 0
    );
}


static esp_err_t mqtt_handle_pwm_command(
    const char *payload
)
{
    if (payload == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *root = cJSON_Parse(payload);

    if (root == NULL)
    {
        ESP_LOGW(
            TAG,
            "Nieprawidlowy JSON komendy PWM"
        );

        return ESP_ERR_INVALID_ARG;
    }

    const cJSON *channel_json =
        cJSON_GetObjectItemCaseSensitive(
            root,
            "channel"
        );

    const cJSON *value_json =
        cJSON_GetObjectItemCaseSensitive(
            root,
            "value"
        );

    if (
        !cJSON_IsNumber(channel_json) ||
        !cJSON_IsNumber(value_json)
    )
    {
        ESP_LOGW(
            TAG,
            "Komenda PWM wymaga pol channel i value"
        );

        cJSON_Delete(root);

        return ESP_ERR_INVALID_ARG;
    }

    int channel_number =
        channel_json->valueint;

    int percent =
        value_json->valueint;

    if (
        channel_number < 1 ||
        channel_number > PWM_CHANNELS ||
        percent < 0 ||
        percent > 100
    )
    {
        ESP_LOGW(
            TAG,
            "Nieprawidlowe parametry PWM: kanal=%d, wartosc=%d",
            channel_number,
            percent
        );

        cJSON_Delete(root);

        return ESP_ERR_INVALID_ARG;
    }

    /*
     * MQTT używa numeracji 1–4,
     * natomiast firmware wewnętrznie 0–3.
     */
    uint8_t internal_channel =
        (uint8_t)(channel_number - 1);

    esp_err_t result = device_set_pwm(
        internal_channel,
        (uint8_t)percent
    );

    cJSON_Delete(root);

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna ustawic PWM%d przez MQTT: %s",
            channel_number,
            esp_err_to_name(result)
        );

        return result;
    }

    ESP_LOGI(
        TAG,
        "PWM%d ustawiono przez MQTT na %d%%",
        channel_number,
        percent
    );

    return ESP_OK;
}


static esp_err_t mqtt_publish_status(void)
{
    if (
        mqtt_client == NULL ||
        !mqtt_connected
    )
    {
        return ESP_ERR_INVALID_STATE;
    }

    const device_state_t *state =
        device_get_state();

    if (state == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    char *payload =
        web_json_build_status(state);

    if (payload == NULL)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna utworzyc JSON statusu"
        );

        return ESP_ERR_NO_MEM;
    }

    int message_id = esp_mqtt_client_publish(
        mqtt_client,
        status_topic,
        payload,
        0,
        0,
        0
    );

    free(payload);

    if (message_id < 0)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna opublikowac statusu MQTT"
        );

        return ESP_FAIL;
    }

    ESP_LOGI(
        TAG,
        "Opublikowano status: %s, msg_id=%d",
        status_topic,
        message_id
    );

    return ESP_OK;
}


static void mqtt_status_task(void *parameter)
{
    (void)parameter;

    while (1)
    {
        if (mqtt_connected)
        {
            esp_err_t result =
                mqtt_publish_status();

            if (
                result != ESP_OK &&
                result != ESP_ERR_INVALID_STATE
            )
            {
                ESP_LOGW(
                    TAG,
                    "Blad publikacji statusu: %s",
                    esp_err_to_name(result)
                );
            }
        }

        vTaskDelay(
            pdMS_TO_TICKS(
                MQTT_STATUS_INTERVAL_MS
            )
        );
    }
}


static void mqtt_event_handler(
    void *handler_args,
    esp_event_base_t base,
    int32_t event_id,
    void *event_data
)
{
    (void)handler_args;
    (void)base;

    esp_mqtt_event_handle_t event =
        (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:
        {
            mqtt_connected = true;
            device_set_mqtt_connected(true);

            ESP_LOGI(
                TAG,
                "Polaczono z brokerem MQTT"
            );

            int message_id =
                esp_mqtt_client_subscribe(
                    mqtt_client,
                    pwm_command_topic,
                    1
                );

            if (message_id < 0)
            {
                ESP_LOGE(
                    TAG,
                    "Nie mozna zasubskrybowac %s",
                    pwm_command_topic
                );
            }
            else
            {
                ESP_LOGI(
                    TAG,
                    "Subskrypcja: %s, msg_id=%d",
                    pwm_command_topic,
                    message_id
                );
            }

            mqtt_publish_status();
            break;
        }

        case MQTT_EVENT_DATA:
        {
            if (
                !mqtt_topic_equals(
                    event,
                    pwm_command_topic
                )
            )
            {
                break;
            }

            /*
             * Komenda jest niewielka. Nie obsługujemy tutaj
             * wiadomości podzielonej na wiele fragmentów.
             */
            if (
                event->data == NULL ||
                event->data_len <= 0 ||
                event->data_len != event->total_data_len ||
                event->data_len >= MQTT_COMMAND_SIZE
            )
            {
                ESP_LOGW(
                    TAG,
                    "Nieprawidlowa dlugosc komendy MQTT"
                );

                break;
            }

            char payload[MQTT_COMMAND_SIZE];

            memcpy(
                payload,
                event->data,
                event->data_len
            );

            payload[event->data_len] = '\0';

            esp_err_t result =
                mqtt_handle_pwm_command(payload);

            if (result == ESP_OK)
            {
                /*
                 * Po poprawnej komendzie publikujemy stan
                 * natychmiast, bez czekania 10 sekund.
                 */
                mqtt_publish_status();
            }

            break;
        }

        case MQTT_EVENT_DISCONNECTED:
            mqtt_connected = false;
            device_set_mqtt_connected(false);

            ESP_LOGW(
                TAG,
                "Rozlaczono z brokerem MQTT"
            );
            break;

        case MQTT_EVENT_ERROR:
            mqtt_connected = false;
            device_set_mqtt_connected(false);

            ESP_LOGE(
                TAG,
                "Blad klienta MQTT"
            );
            break;

        default:
            break;
    }
}


esp_err_t mqtt_manager_init(void)
{
    device_config_t *config = config_get();

    if (config == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (
        config->mqtt_host[0] == '\0' ||
        config->mqtt_port == 0 ||
        config->device_name[0] == '\0'
    )
    {
        ESP_LOGE(
            TAG,
            "Nieprawidlowa konfiguracja MQTT"
        );

        return ESP_ERR_INVALID_ARG;
    }

    int written = snprintf(
        broker_uri,
        sizeof(broker_uri),
        "mqtt://%s:%u",
        config->mqtt_host,
        config->mqtt_port
    );

    if (
        written < 0 ||
        written >= (int)sizeof(broker_uri)
    )
    {
        return ESP_ERR_INVALID_SIZE;
    }

    written = snprintf(
        status_topic,
        sizeof(status_topic),
        "%s/status",
        config->device_name
    );

    if (
        written < 0 ||
        written >= (int)sizeof(status_topic)
    )
    {
        return ESP_ERR_INVALID_SIZE;
    }

    written = snprintf(
        pwm_command_topic,
        sizeof(pwm_command_topic),
        "%s/command/pwm",
        config->device_name
    );

    if (
        written < 0 ||
        written >= (int)sizeof(pwm_command_topic)
    )
    {
        return ESP_ERR_INVALID_SIZE;
    }

    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = broker_uri
    };

    mqtt_client = esp_mqtt_client_init(
        &mqtt_config
    );

    if (mqtt_client == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t result =
        esp_mqtt_client_register_event(
            mqtt_client,
            ESP_EVENT_ANY_ID,
            mqtt_event_handler,
            NULL
        );

    if (result != ESP_OK)
    {
        esp_mqtt_client_destroy(mqtt_client);
        mqtt_client = NULL;

        return result;
    }

    result = esp_mqtt_client_start(
        mqtt_client
    );

    if (result != ESP_OK)
    {
        esp_mqtt_client_destroy(mqtt_client);
        mqtt_client = NULL;

        return result;
    }

    BaseType_t task_result = xTaskCreate(
        mqtt_status_task,
        "mqtt_status_task",
        MQTT_TASK_STACK_SIZE,
        NULL,
        MQTT_TASK_PRIORITY,
        NULL
    );

    if (task_result != pdPASS)
    {
        esp_mqtt_client_stop(mqtt_client);
        esp_mqtt_client_destroy(mqtt_client);

        mqtt_client = NULL;

        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(
        TAG,
        "Klient MQTT uruchomiony: %s",
        broker_uri
    );

    ESP_LOGI(
        TAG,
        "Temat statusu: %s",
        status_topic
    );

    ESP_LOGI(
        TAG,
        "Temat komend PWM: %s",
        pwm_command_topic
    );

    return ESP_OK;
}


bool mqtt_manager_is_connected(void)
{
    return mqtt_connected;
}