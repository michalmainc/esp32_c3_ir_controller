idf_component_register(
    SRCS
        "main.c"
        "config/config.c"
        "pwm/pwm.c"
        "device/device.c"
        "wifi/wifi_manager.c"
        "onewire/onewire_utils.c"
        "temperature/temperature_manager.c"
        "web/web_server.c"
        "web/web_pages.c"
        "web/web_api.c"
        "web/web_json.c"
        "web/generated_assets.c"
        "mqtt/mqtt_manager.c"

    INCLUDE_DIRS
        "."
        "config"
        "pwm"
        "device"
        "wifi"
        "onewire"
        "temperature"
        "web"
        "mqtt"

    REQUIRES
        nvs_flash
        esp_wifi
        esp_event
        esp_netif
        esp_http_server

)