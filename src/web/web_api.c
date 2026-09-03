#include "web_api.h"
#include "web_json.h"

#include <stdio.h>
#include <stdlib.h>

#include "device/device.h"
#include "config/config.h"

#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "device/device_commands.h"
#include "relay/relay.h"
#include "cJSON.h"

#include "mqtt/mqtt_manager.h"


static const char *TAG = "WEB_API";


static void restart_task(void *parameter)
{
    (void)parameter;

    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGW(TAG, "Restart ukladu");

    esp_restart();
}

static esp_err_t pwm_handler(httpd_req_t *request)
{
    char query[96];
    char channel_text[8];
    char value_text[8];

    esp_err_t result = httpd_req_get_url_query_str(
        request,
        query,
        sizeof(query)
    );

    if (result != ESP_OK)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Brak parametrow"
        );

        return ESP_FAIL;
    }

    result = httpd_query_key_value(
        query,
        "channel",
        channel_text,
        sizeof(channel_text)
    );

    if (result != ESP_OK)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Brak parametru channel"
        );

        return ESP_FAIL;
    }

    result = httpd_query_key_value(
        query,
        "value",
        value_text,
        sizeof(value_text)
    );

    if (result != ESP_OK)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Brak parametru value"
        );

        return ESP_FAIL;
    }

    char *channel_end = NULL;
    char *value_end = NULL;

    long channel = strtol(
        channel_text,
        &channel_end,
        10
    );

    long percent = strtol(
        value_text,
        &value_end,
        10
    );

    if (
        channel_end == channel_text ||
        *channel_end != '\0' ||
        channel < 0
    )
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Kanal musi miec wartosc 0-3"
        );

        return ESP_FAIL;
    }

    if (
        value_end == value_text ||
        *value_end != '\0' ||
        percent < 0 ||
        percent > 100
    )
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "PWM musi miec wartosc 0-100"
        );

        return ESP_FAIL;
    }

    result = device_command_set_pwm(
        (uint8_t)channel,
        (uint8_t)percent
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna ustawic PWM%ld: %s",
            channel + 1,
            esp_err_to_name(result)
        );

        httpd_resp_send_err(
            request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Blad ustawiania PWM"
        );

        return result;
    }

    ESP_LOGI(
        TAG,
        "PWM%ld ustawiono przez WWW na %ld%%",
        channel + 1,
        percent
    );

    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"channel\":%ld,\"value\":%ld}",
        channel,
        percent
    );

    httpd_resp_set_type(
        request,
        "application/json"
    );

    return httpd_resp_sendstr(
        request,
        response
    );
}

static esp_err_t relay_handler(httpd_req_t *request)
{
    char buffer[96];

    int length = httpd_req_recv(
        request,
        buffer,
        sizeof(buffer) - 1
    );

    if (length <= 0)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Brak danych"
        );

        return ESP_FAIL;
    }

    buffer[length] = '\0';

    cJSON *root = cJSON_Parse(buffer);

    if (root == NULL)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Nieprawidlowy JSON"
        );

        return ESP_FAIL;
    }

    const cJSON *channel =
        cJSON_GetObjectItem(
            root,
            "channel"
        );

    const cJSON *state =
        cJSON_GetObjectItem(
            root,
            "state"
        );

    if (
        !cJSON_IsNumber(channel) ||
        !cJSON_IsBool(state)
    )
    {
        cJSON_Delete(root);

        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Brak channel/state"
        );

        return ESP_FAIL;
    }

    esp_err_t result =
        device_command_set_relay(
            channel->valueint - 1,
            cJSON_IsTrue(state)
        );

    cJSON_Delete(root);

    if (result != ESP_OK)
    {
        httpd_resp_send_err(
            request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Blad relay"
        );

        return result;
    }
    
    esp_err_t mqtt_result =
        mqtt_manager_publish_status();

    if (
        mqtt_result != ESP_OK &&
        mqtt_result != ESP_ERR_INVALID_STATE
    )
    {
        ESP_LOGW(
            TAG,
            "Nie mozna natychmiast opublikowac statusu MQTT: %s",
            esp_err_to_name(mqtt_result)
        );
    }    

    httpd_resp_set_type(
        request,
        "application/json"
    );

    return httpd_resp_sendstr(
        request,
        "{\"status\":\"ok\"}"
    );
}

static esp_err_t relay_name_handler(
    httpd_req_t *request
)
{
    char buffer[128];

    int length = httpd_req_recv(
        request,
        buffer,
        sizeof(buffer) - 1
    );

    if (length <= 0)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Brak danych"
        );

        return ESP_FAIL;
    }

    buffer[length] = '\0';

    cJSON *root = cJSON_Parse(buffer);

    if (root == NULL)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Nieprawidlowy JSON"
        );

        return ESP_FAIL;
    }

    const cJSON *channel =
        cJSON_GetObjectItem(
            root,
            "channel"
        );

    const cJSON *name =
        cJSON_GetObjectItem(
            root,
            "name"
        );

    if (
        !cJSON_IsNumber(channel) ||
        !cJSON_IsString(name) ||
        channel->valueint < 1 ||
        channel->valueint > RELAY_CHANNELS
    )
    {
        cJSON_Delete(root);

        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Nieprawidlowy channel/name"
        );

        return ESP_FAIL;
    }

    esp_err_t result =
        config_set_relay_name(
            channel->valueint - 1,
            name->valuestring
        );

    cJSON_Delete(root);

    if (result != ESP_OK)
    {
        httpd_resp_send_err(
            request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Blad zapisu nazwy"
        );

        return result;
    }

    httpd_resp_set_type(
        request,
        "application/json"
    );

    return httpd_resp_sendstr(
        request,
        "{\"status\":\"ok\"}"
    );
}

static esp_err_t temperature_name_handler(
    httpd_req_t *request
)
{
    char buffer[160];

    int length = httpd_req_recv(
        request,
        buffer,
        sizeof(buffer) - 1
    );

    if (length <= 0)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Brak danych"
        );

        return ESP_FAIL;
    }

    buffer[length] = '\0';

    cJSON *root = cJSON_Parse(buffer);

    if (root == NULL)
    {
        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Nieprawidlowy JSON"
        );

        return ESP_FAIL;
    }

    const cJSON *address =
        cJSON_GetObjectItem(
            root,
            "address"
        );

    const cJSON *name =
        cJSON_GetObjectItem(
            root,
            "name"
        );

    if (
        !cJSON_IsString(address) ||
        !cJSON_IsString(name)
    )
    {
        cJSON_Delete(root);

        httpd_resp_send_err(
            request,
            HTTPD_400_BAD_REQUEST,
            "Brak address/name"
        );

        return ESP_FAIL;
    }

    esp_err_t result =
        config_set_temperature_name(
            address->valuestring,
            name->valuestring
        );

    cJSON_Delete(root);

    if (result != ESP_OK)
    {
        httpd_resp_send_err(
            request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Blad zapisu nazwy"
        );

        return result;
    }

    httpd_resp_set_type(
        request,
        "application/json"
    );

    return httpd_resp_sendstr(
        request,
        "{\"status\":\"ok\"}"
    );
}

static esp_err_t restart_handler(httpd_req_t *request)
{
    ESP_LOGW(
        TAG,
        "Odebrano polecenie restartu przez WWW"
    );

    httpd_resp_set_type(
        request,
        "application/json"
    );

    esp_err_t result = httpd_resp_sendstr(
        request,
        "{\"status\":\"restarting\"}"
    );

    if (result != ESP_OK)
    {
        return result;
    }

    BaseType_t task_result = xTaskCreate(
        restart_task,
        "restart_task",
        2048,
        NULL,
        10,
        NULL
    );

    if (task_result != pdPASS)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna utworzyc zadania restartu"
        );

        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t status_handler(httpd_req_t *request)
{
    const device_state_t *state =
        device_get_state();

    if (state == NULL)
    {
        httpd_resp_send_err(
            request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Brak stanu urzadzenia"
        );

        return ESP_FAIL;
    }

    char *response =
        web_json_build_status(state);

    if (response == NULL)
    {
        httpd_resp_send_err(
            request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Nie mozna utworzyc JSON"
        );

        return ESP_ERR_NO_MEM;
    }

    httpd_resp_set_type(
        request,
        "application/json"
    );

    esp_err_t result =
        httpd_resp_sendstr(
            request,
            response
        );

    free(response);

    return result;
}

esp_err_t web_api_register(httpd_handle_t server)
{
    if (server == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    const httpd_uri_t status_uri = {
        .uri = "/api/status",
        .method = HTTP_GET,
        .handler = status_handler,
        .user_ctx = NULL
    };

    esp_err_t result = httpd_register_uri_handler(
        server,
        &status_uri
    );

    if (result != ESP_OK)
    {
        return result;
    }

    const httpd_uri_t pwm_uri = {
        .uri = "/api/pwm",
        .method = HTTP_POST,
        .handler = pwm_handler,
        .user_ctx = NULL
    };

    result = httpd_register_uri_handler(
        server,
        &pwm_uri
    );

    if (result != ESP_OK)
    {
        return result;
    }

    const httpd_uri_t relay_uri = {
        .uri = "/api/relay",
        .method = HTTP_POST,
        .handler = relay_handler,
        .user_ctx = NULL
    };

    result = httpd_register_uri_handler(
        server,
        &relay_uri
    );

    if (result != ESP_OK)
    {
        return result;
    }

    const httpd_uri_t relay_name_uri = {
        .uri = "/api/relay/name",
        .method = HTTP_POST,
        .handler = relay_name_handler,
        .user_ctx = NULL
    };

    result = httpd_register_uri_handler(
        server,
        &relay_name_uri
    );

    if (result != ESP_OK)
    {
        return result;
    }

    const httpd_uri_t temperature_name_uri = {
        .uri = "/api/temperature/name",
        .method = HTTP_POST,
        .handler = temperature_name_handler,
        .user_ctx = NULL
    };

    result = httpd_register_uri_handler(
        server,
        &temperature_name_uri
    );

    if (result != ESP_OK)
    {
        return result;
    }

    const httpd_uri_t restart_uri = {
        .uri = "/api/restart",
        .method = HTTP_POST,
        .handler = restart_handler,
        .user_ctx = NULL
    };

    return httpd_register_uri_handler(
        server,
        &restart_uri
    );
}

