#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"


esp_err_t wifi_manager_init(void);

bool wifi_manager_is_connected(void);

esp_err_t wifi_manager_get_ip(
    char *buffer,
    size_t buffer_size
);

esp_err_t wifi_manager_get_rssi(
    int8_t *rssi
);

esp_err_t wifi_manager_wait_connected(
    uint32_t timeout_ms
);

#endif