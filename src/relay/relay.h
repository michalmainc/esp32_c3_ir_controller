#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"


#define RELAY_CHANNELS 1


esp_err_t relay_init(void);

esp_err_t relay_set(
    uint8_t channel,
    bool enabled
);

bool relay_get(
    uint8_t channel
);


#endif