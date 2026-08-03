#ifndef TEMPERATURE_MANAGER_H
#define TEMPERATURE_MANAGER_H

#include <stdint.h>

#include "esp_err.h"


#define TEMPERATURE_MAX_SENSORS 8


esp_err_t temperature_manager_init(void);

uint8_t temperature_manager_get_sensor_count(void);


#endif