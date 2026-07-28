#include "web_server.h"

#include <stdio.h>
#include <stdlib.h>

#include "pwm/pwm.h"

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "WEB";


static const char index_html[] =
    "<!DOCTYPE html>"
    "<html lang=\"pl\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
    "<title>ESP32-C3 Controller</title>"

    "<style>"
    "*{box-sizing:border-box;}"

    "body{"
        "margin:0;"
        "min-height:100vh;"
        "display:flex;"
        "align-items:center;"
        "justify-content:center;"
        "padding:20px;"
        "background:#101216;"
        "color:#eeeeee;"
        "font-family:Arial,sans-serif;"
    "}"

    ".panel{"
        "width:100%;"
        "max-width:560px;"
        "padding:24px;"
        "background:#1b1e24;"
        "border:1px solid #343943;"
        "border-radius:14px;"
        "box-shadow:0 10px 35px rgba(0,0,0,.4);"
    "}"

    "h1{"
        "margin:0 0 24px;"
        "text-align:center;"
        "font-size:25px;"
    "}"

    ".status{"
        "padding:16px;"
        "margin-bottom:20px;"
        "background:#252932;"
        "border-radius:10px;"
        "line-height:1.7;"
    "}"

    ".online{"
        "color:#58d68d;"
        "font-weight:bold;"
    "}"

    ".pwm-card{"
        "padding:16px;"
        "margin-bottom:12px;"
        "background:#252932;"
        "border-radius:10px;"
    "}"

    ".pwm-header{"
        "display:flex;"
        "justify-content:space-between;"
        "align-items:center;"
        "margin-bottom:10px;"
        "font-size:17px;"
    "}"

    ".pwm-value{"
        "font-size:20px;"
        "font-weight:bold;"
        "min-width:55px;"
        "text-align:right;"
    "}"

    "input[type=range]{"
        "width:100%;"
        "height:32px;"
        "cursor:pointer;"
    "}"

    ".pwm-info{"
        "margin-top:6px;"
        "font-size:13px;"
        "color:#aeb6c2;"
    "}"

    "button{"
        "width:100%;"
        "padding:15px;"
        "margin-top:10px;"
        "border:0;"
        "border-radius:10px;"
        "background:#c94b4b;"
        "color:white;"
        "font-size:17px;"
        "font-weight:bold;"
        "cursor:pointer;"
    "}"

    "button:disabled{"
        "opacity:.6;"
        "cursor:default;"
    "}"

    "#message{"
        "min-height:24px;"
        "margin-top:16px;"
        "text-align:center;"
        "color:#b8c0cc;"
    "}"
    "</style>"
    "</head>"

    "<body>"
    "<main class=\"panel\">"

    "<h1>ESP32-C3 Controller</h1>"

    "<div class=\"status\">"
    "Stan urządzenia: <span class=\"online\">ONLINE</span><br>"
    "PWM: 400 Hz"
    "</div>"

    "<div class=\"pwm-card\">"
        "<div class=\"pwm-header\">"
            "<span>PWM1</span>"
            "<span class=\"pwm-value\" id=\"pwmValue0\">0%</span>"
        "</div>"
        "<input id=\"pwmSlider0\" type=\"range\" "
        "min=\"0\" max=\"100\" step=\"1\" value=\"0\">"
        "<div class=\"pwm-info\">GPIO2 / D0</div>"
    "</div>"

    "<div class=\"pwm-card\">"
        "<div class=\"pwm-header\">"
            "<span>PWM2</span>"
            "<span class=\"pwm-value\" id=\"pwmValue1\">0%</span>"
        "</div>"
        "<input id=\"pwmSlider1\" type=\"range\" "
        "min=\"0\" max=\"100\" step=\"1\" value=\"0\">"
        "<div class=\"pwm-info\">GPIO3 / D1</div>"
    "</div>"

    "<div class=\"pwm-card\">"
        "<div class=\"pwm-header\">"
            "<span>PWM3</span>"
            "<span class=\"pwm-value\" id=\"pwmValue2\">0%</span>"
        "</div>"
        "<input id=\"pwmSlider2\" type=\"range\" "
        "min=\"0\" max=\"100\" step=\"1\" value=\"0\">"
        "<div class=\"pwm-info\">GPIO4 / D2</div>"
    "</div>"

    "<div class=\"pwm-card\">"
        "<div class=\"pwm-header\">"
            "<span>PWM4</span>"
            "<span class=\"pwm-value\" id=\"pwmValue3\">0%</span>"
        "</div>"
        "<input id=\"pwmSlider3\" type=\"range\" "
        "min=\"0\" max=\"100\" step=\"1\" value=\"0\">"
        "<div class=\"pwm-info\">GPIO5 / D3</div>"
    "</div>"

    "<button id=\"restartButton\" onclick=\"restartDevice()\">"
    "Restart ESP32"
    "</button>"

    "<div id=\"message\"></div>"

    "</main>"

    "<script>"
    "const message=document.getElementById('message');"
    "const pwmTimers=[null,null,null,null];"

    "for(let channel=0;channel<4;channel++){"
        "const slider=document.getElementById('pwmSlider'+channel);"
        "const valueText=document.getElementById('pwmValue'+channel);"

        "slider.addEventListener('input',function(){"
            "const value=slider.value;"
            "valueText.textContent=value+'%';"

            "clearTimeout(pwmTimers[channel]);"

            "pwmTimers[channel]=setTimeout(function(){"
                "setPwm(channel,value);"
            "},100);"
        "});"
    "}"

    "async function setPwm(channel,value){"
        "try{"
            "const response=await fetch("
                "'/api/pwm?channel='+channel+'&value='+value,"
                "{method:'POST'}"
            ");"

            "if(!response.ok){"
                "throw new Error('HTTP '+response.status);"
            "}"

            "message.textContent="
                "'PWM'+(channel+1)+' ustawiono na '+value+'%';"
        "}catch(error){"
            "message.textContent="
                "'Błąd ustawiania PWM'+(channel+1);"
        "}"
    "}"

    "async function restartDevice(){"
        "const button=document.getElementById('restartButton');"

        "button.disabled=true;"
        "message.textContent='Restartowanie urządzenia...';"

        "try{"
            "await fetch('/api/restart',{method:'POST'});"
            "message.textContent='ESP32 uruchamia się ponownie.';"
        "}catch(error){"
            "message.textContent='Połączenie przerwane — trwa restart.';"
        "}"

        "setTimeout(function(){"
            "window.location.reload();"
        "},6000);"
    "}"
    "</script>"

    "</body>"
    "</html>";


static void restart_task(void *parameter)
{
    (void)parameter;

    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGW(TAG, "Restart ukladu");

    esp_restart();
}


static esp_err_t index_handler(httpd_req_t *request)
{
    httpd_resp_set_type(
        request,
        "text/html; charset=utf-8"
    );

    return httpd_resp_send(
        request,
        index_html,
        HTTPD_RESP_USE_STRLEN
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
        channel < 0 ||
        channel >= PWM_CHANNEL_COUNT
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

    result = pwm_set_percent(
        (uint8_t)channel,
        (uint8_t)percent
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Blad ustawiania PWM%ld: %s",
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


esp_err_t web_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    ESP_LOGI(TAG, "Uruchamianie serwera WWW");

    esp_err_t result = httpd_start(
        &server,
        &config
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Blad uruchamiania WWW: %s",
            esp_err_to_name(result)
        );

        return result;
    }

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL
    };

    httpd_uri_t pwm_uri = {
        .uri = "/api/pwm",
        .method = HTTP_POST,
        .handler = pwm_handler,
        .user_ctx = NULL
    };

    httpd_uri_t restart_uri = {
        .uri = "/api/restart",
        .method = HTTP_POST,
        .handler = restart_handler,
        .user_ctx = NULL
    };

    result = httpd_register_uri_handler(
        server,
        &index_uri
    );

    if (result != ESP_OK)
    {
        httpd_stop(server);
        return result;
    }

    result = httpd_register_uri_handler(
        server,
        &pwm_uri
    );

    if (result != ESP_OK)
    {
        httpd_stop(server);
        return result;
    }

    result = httpd_register_uri_handler(
        server,
        &restart_uri
    );

    if (result != ESP_OK)
    {
        httpd_stop(server);
        return result;
    }

    ESP_LOGI(
        TAG,
        "Serwer WWW uruchomiony na porcie 80"
    );

    return ESP_OK;
}