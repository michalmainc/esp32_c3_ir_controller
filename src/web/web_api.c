#include "web_api.h"

#include <stdio.h>
#include <stdlib.h>

#include "device/device.h"

#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "WEB_API";


static void restart_task(void *parameter)
{
    (void)parameter;

    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGW(TAG, "Restart ukladu");

    esp_restart();
}


static esp_err_t status_handler(httpd_req_t *request)
{
    char response[96];

    snprintf(
        response,
        sizeof(response),
        "{\"pwm\":[%u,%u,%u,%u]}",
        device_get_pwm(0),
        device_get_pwm(1),
        device_get_pwm(2),
        device_get_pwm(3)
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

    result = device_set_pwm(
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