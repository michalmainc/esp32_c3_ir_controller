#ifndef DEVICE_H
#define DEVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#include "config/config.h"


typedef struct
{
    struct
    {
        bool connected;
        int8_t rssi;
        char ip[16];

    } wifi;

    struct
    {
        uint8_t pwm[PWM_CHANNELS];

    } outputs;

    struct
    {
        float ds18b20[8];
        uint8_t count;

    } temperature;

    struct
    {
        bool connected;

    } mqtt;

    struct
    {
        uint32_t uptime;
        size_t free_heap;

    } system;

} device_state_t;


esp_err_t device_init(void);

esp_err_t device_set_pwm(
    uint8_t channel,
    uint8_t percent
);

uint8_t device_get_pwm(
    uint8_t channel
);

const device_state_t *device_get_state(void);


#endif