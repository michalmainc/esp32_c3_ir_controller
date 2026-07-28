idf_component_register(
    SRCS
        "main.c"
        "config/config.c"
        "pwm/pwm.c"
        "wifi/wifi_manager.c"

    INCLUDE_DIRS
        "."
        "config"
        "pwm"
        "wifi"

    REQUIRES
        nvs_flash
        esp_wifi
        esp_event
        esp_netif
)