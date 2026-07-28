#ifndef CONFIG_H
#define CONFIG_H


#include <stdint.h>


#define PWM_CHANNELS 4


typedef struct
{
    char device_name[32];


    char ip[16];
    char gateway[16];
    char netmask[16];


    float latitude;
    float longitude;


    char mqtt_host[64];
    uint16_t mqtt_port;


    char pwm_name[PWM_CHANNELS][32];


    char temperature_name[32];


} device_config_t;



void config_init(void);


device_config_t *config_get(void);


void config_save(void);


#endif