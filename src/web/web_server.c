#include "web_server.h"

#include "config/config.h"

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "WEB";


static const char INDEX_HTML[] =
"<!DOCTYPE html>"
"<html lang='pl'>"
"<head>"
"  <meta charset='UTF-8'>"
"  <meta name='viewport' content='width=device-width,initial-scale=1'>"
"  <title>ESP32-C3 Controller</title>"
"  <style>"
"    * { box-sizing: border-box; }"
"    body {"
"      margin: 0;"
"      min-height: 100vh;"
"      background: #101216;"
"      color: #e8e8e8;"
"      font-family: Arial, sans-serif;"
"      display: flex;"
"      justify-content: center;"
"      padding: 24px;"
"    }"
"    .panel {"
"      width: 100%;"
"      max-width: 520px;"
"      background: #1a1d23;"
"      border: 1px solid #30343d;"
"      border-radius: 14px;"
"      padding: 24px;"
"      box-shadow: 0 8px 30px rgba(0,0,0,0.35);"
"    }"
"    h1 {"
"      margin-top: 0;"
"      font-size: 24px;"
"      text-align: center;"
"    }"
"    .status {"
"      background: #22262e;"
"      border-radius: 10px;"
"      padding: 16px;"
"      margin: 20px 0;"
"      line-height: 1.7;"
"    }"
"    button {"
"      width: 100%;"
"      border: 0;"
"      border-radius: 10px;"
"      padding: 15px;"
"      font-size: 17px;"
"      font-weight: bold;"
"      cursor: pointer;"
"      background: #d64b4b;"
"      color: white;"
"    }"
"    button:hover {"
"      background: #ee5b5b;"
"    }"
"    button:disabled {"
"      opacity: 0.6;"
"      cursor: default;"
"    }"
"    #message {"
"      text-align: center;"
"      min-height: 24px;"
"      margin-top: 16px;"
"      color: #aab2c0;"
"    }"
"  </style>"
"</head>"
"<body>"
"  <main class='panel'>"
"    <h1>ESP32-C3 Controller</h1>"
"    <div class='status'>"
"      <div>Stan urządzenia: <strong>online</strong></div>"
"      <div>Adres IP: <strong>172.16.5.5</strong></div>"
"    </div>"
"    <button id='restartButton' onclick='restartDevice()'>"
"      Restart ESP32"
"    </button>"
"    <div id='message'></div>"
"  </main>"
"  <script>"
"    async function restartDevice() {"
"      const button = document.getElementById('restartButton');"
"      const message = document.getElementById('message');"
"      button.disabled = true;"
"      message.textContent = 'Restartowanie urządzenia...';"
"      try {"
"        await fetch('/api/restart', { method: 'POST' });"
"        message.textContent = 'ESP32 uruchamia się ponownie.';"
"      } catch (error) {"
"        message.textContent = 'Połączenie przerwane — trwa restart.';"
"      }"
"      setTimeout(() => window.location.reload(), 5000);"
"    }"
"  </script>"
"</body>"
"</html>";


static void restart_task(void *parameter)
{
    (void)parameter;

    vTaskDelay(pdMS_TO_TICKS(700));

    ESP_LOGW(TAG, "Restart ESP32");

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
        INDEX_HTML,
        HTTPD_RESP_USE_STRLEN
    );
}


static esp_err_t restart_handler(httpd_req_t *request)
{
    ESP_LOGW(TAG, "Odebrano polecenie restartu przez WWW");

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
        ESP_LOGE(TAG, "Nie mozna utworzyc zadania restartu");
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
            "Nie mozna uruchomic serwera: %s",
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

    ESP_ERROR_CHECK(
        httpd_register_uri_handler(
            server,
            &index_uri
        )
    );

    ESP_ERROR_CHECK(
        httpd_register_uri_handler(
            server,
            &restart_uri
        )
    );

    device_config_t *cfg = config_get();

    ESP_LOGI(
        TAG,
        "Strona dostepna pod adresem: http://%s",
        cfg->ip
    );

    return ESP_OK;
}