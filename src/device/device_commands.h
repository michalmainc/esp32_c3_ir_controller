#ifndef DEVICE_COMMANDS_H
#define DEVICE_COMMANDS_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"


esp_err_t device_command_set_pwm(
    uint8_t channel,
    uint8_t percent
);


esp_err_t device_command_set_relay(
    uint8_t channel,
    bool enabled
);


#endif
