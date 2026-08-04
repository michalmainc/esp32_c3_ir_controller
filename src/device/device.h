#ifndef DEVICE_H
#define DEVICE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#include "config/config.h"


#define DEVICE_MAX_TEMPERATURE_SENSORS 8


typedef struct
{
    uint64_t address;
    float value;
    bool present;

} device_temperature_sensor_t;


typedef struct
{
    struct
    {
        char name[32];
        char serial[24];

    } device;

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
        uint8_t count;

        device_temperature_sensor_t sensors[
            DEVICE_MAX_TEMPERATURE_SENSORS
        ];

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


void device_set_temperature_count(
    uint8_t count
);


void device_update_temperature_sensor(
    uint8_t index,
    uint64_t address,
    float value,
    bool present
);


const device_state_t *device_get_state(void);

void device_set_mqtt_connected(
    bool connected
);

#endif