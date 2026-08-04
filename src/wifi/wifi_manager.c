#include "wifi_manager.h"

#include <stdio.h>
#include <string.h>

#include "secrets.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#define WIFI_CONNECTED_BIT BIT0


static const char *TAG = "WIFI";

static EventGroupHandle_t wifi_event_group;
static esp_netif_t *wifi_netif = NULL;

static bool wifi_connected = false;


static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
)
{
    (void)arg;

    if (
        event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START
    )
    {
        ESP_LOGI(
            TAG,
            "Proba polaczenia z siecia %s",
            WIFI_SSID
        );

        esp_wifi_connect();
    }
    else if (
        event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_DISCONNECTED
    )
    {
        wifi_connected = false;

        xEventGroupClearBits(
            wifi_event_group,
            WIFI_CONNECTED_BIT
        );

        ESP_LOGW(
            TAG,
            "Rozlaczono z Wi-Fi, ponawiam polaczenie"
        );

        esp_wifi_connect();
    }
    else if (
        event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP
    )
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;

        wifi_connected = true;

        ESP_LOGI(
            TAG,
            "Polaczono, adres IP: " IPSTR,
            IP2STR(&event->ip_info.ip)
        );

        xEventGroupSetBits(
            wifi_event_group,
            WIFI_CONNECTED_BIT
        );
    }
}


esp_err_t wifi_manager_init(void)
{
    ESP_LOGI(TAG, "Start Wi-Fi");

    wifi_event_group = xEventGroupCreate();

    if (wifi_event_group == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = esp_netif_init();

    if (
        err != ESP_OK &&
        err != ESP_ERR_INVALID_STATE
    )
    {
        return err;
    }

    err = esp_event_loop_create_default();

    if (
        err != ESP_OK &&
        err != ESP_ERR_INVALID_STATE
    )
    {
        return err;
    }

    wifi_netif =
        esp_netif_create_default_wifi_sta();

    if (wifi_netif == NULL)
    {
        return ESP_FAIL;
    }

    wifi_init_config_t wifi_init_config =
        WIFI_INIT_CONFIG_DEFAULT();

    err = esp_wifi_init(
        &wifi_init_config
    );

    if (err != ESP_OK)
    {
        return err;
    }

    err = esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        wifi_event_handler,
        NULL
    );

    if (err != ESP_OK)
    {
        return err;
    }

    err = esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        wifi_event_handler,
        NULL
    );

    if (err != ESP_OK)
    {
        return err;
    }

    wifi_config_t wifi_config = {0};

    strncpy(
        (char *)wifi_config.sta.ssid,
        WIFI_SSID,
        sizeof(wifi_config.sta.ssid) - 1
    );

    strncpy(
        (char *)wifi_config.sta.password,
        WIFI_PASSWORD,
        sizeof(wifi_config.sta.password) - 1
    );

    wifi_config.sta.threshold.authmode =
        WIFI_AUTH_WPA2_PSK;

    err = esp_wifi_set_mode(
        WIFI_MODE_STA
    );

    if (err != ESP_OK)
    {
        return err;
    }

    err = esp_wifi_set_config(
        WIFI_IF_STA,
        &wifi_config
    );

    if (err != ESP_OK)
    {
        return err;
    }

    err = esp_wifi_start();

    if (err != ESP_OK)
    {
        return err;
    }

    ESP_LOGI(
        TAG,
        "Sterownik Wi-Fi uruchomiony"
    );

    return ESP_OK;
}


bool wifi_manager_is_connected(void)
{
    return wifi_connected;
}


esp_err_t wifi_manager_get_ip(
    char *buffer,
    size_t buffer_size
)
{
    if (
        buffer == NULL ||
        buffer_size < 16
    )
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (
        !wifi_connected ||
        wifi_netif == NULL
    )
    {
        buffer[0] = '\0';

        return ESP_ERR_INVALID_STATE;
    }

    esp_netif_ip_info_t ip_info = {0};

    esp_err_t result = esp_netif_get_ip_info(
        wifi_netif,
        &ip_info
    );

    if (result != ESP_OK)
    {
        buffer[0] = '\0';

        return result;
    }

    snprintf(
        buffer,
        buffer_size,
        IPSTR,
        IP2STR(&ip_info.ip)
    );

    return ESP_OK;
}


esp_err_t wifi_manager_get_rssi(
    int8_t *rssi
)
{
    if (rssi == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (!wifi_connected)
    {
        *rssi = 0;

        return ESP_ERR_INVALID_STATE;
    }

    wifi_ap_record_t access_point = {0};

    esp_err_t result =
        esp_wifi_sta_get_ap_info(
            &access_point
        );

    if (result != ESP_OK)
    {
        *rssi = 0;

        return result;
    }

    *rssi = access_point.rssi;

    return ESP_OK;
}

esp_err_t wifi_manager_wait_connected(
    uint32_t timeout_ms
)
{
    if (wifi_event_group == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    EventBits_t bits = xEventGroupWaitBits(
        wifi_event_group,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        pdMS_TO_TICKS(timeout_ms)
    );

    if ((bits & WIFI_CONNECTED_BIT) == 0)
    {
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}