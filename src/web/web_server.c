#include "web_server.h"

#include "web_api.h"
#include "web_pages.h"

#include "esp_http_server.h"
#include "esp_log.h"


static const char *TAG = "WEB_SERVER";


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

    result = web_pages_register(server);

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna zarejestrowac stron WWW: %s",
            esp_err_to_name(result)
        );

        httpd_stop(server);
        return result;
    }

    result = web_api_register(server);

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna zarejestrowac API: %s",
            esp_err_to_name(result)
        );

        httpd_stop(server);
        return result;
    }

    ESP_LOGI(
        TAG,
        "Serwer WWW uruchomiony na porcie 80"
    );

    return ESP_OK;
}