#include "web_server.h"

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
        "max-width:480px;"
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
    "button{"
        "width:100%;"
        "padding:15px;"
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
    "Połączenie: Wi-Fi DHCP"
    "</div>"
    "<button id=\"restartButton\" onclick=\"restartDevice()\">"
    "Restart ESP32"
    "</button>"
    "<div id=\"message\"></div>"
    "</main>"
    "<script>"
    "async function restartDevice(){"
        "const button=document.getElementById('restartButton');"
        "const message=document.getElementById('message');"
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