#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

#include "esp_err.h"

#define DEVICE_PWM_CHANNELS 4


esp_err_t device_init(void);

esp_err_t device_set_pwm(
    uint8_t channel,
    uint8_t percent
);

uint8_t device_get_pwm(
    uint8_t channel
);


#endif