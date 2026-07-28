idf_component_register(
    SRCS
        "main.c"
        "config/config.c"
        "pwm/pwm.c"
        "wifi/wifi_manager.c"
        "web/web_server.c"

    INCLUDE_DIRS
        "."
        "config"
        "pwm"
        "wifi"
        "web"

    REQUIRES
        nvs_flash
        esp_wifi
        esp_event
        esp_netif
        esp_http_server
)