#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stddef.h>

#include "esp_err.h"


#define PWM_CHANNELS 4


typedef struct
{
    char device_name[32];

    float latitude;
    float longitude;

    char mqtt_host[64];
    uint16_t mqtt_port;

    char pwm_name[PWM_CHANNELS][32];
    uint8_t pwm_value[PWM_CHANNELS];

    char temperature_name[32];

} device_config_t;


void config_init(void);

device_config_t *config_get(void);

void config_save(void);

/*
 * Zmienia zapisaną wartość PWM i uruchamia
 * opóźniony zapis do NVS po 5 sekundach.
 */
esp_err_t config_set_pwm_value(
    uint8_t channel,
    uint8_t value
);

esp_err_t config_get_relay_name(
    uint8_t channel,
    char *name,
    size_t name_size
);

esp_err_t config_set_relay_name(
    uint8_t channel,
    const char *name
);

esp_err_t config_get_temperature_name(
    const char *address,
    char *name,
    size_t name_size
);

esp_err_t config_set_temperature_name(
    const char *address,
    const char *name
);

#endif