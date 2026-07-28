#include "config.h"

#include <string.h>

#include "nvs.h"
#include "nvs_flash.h"


#define NVS_NAMESPACE "config"



static device_config_t device_config;



static void load_defaults(void)
{

    strcpy(device_config.device_name,
           "esp32_c3");


    strcpy(device_config.ip,
           "172.16.5.5");


    strcpy(device_config.gateway,
           "172.16.5.1");


    strcpy(device_config.netmask,
           "255.255.255.0");


    device_config.latitude =
        52.8597009;


    device_config.longitude =
        16.0817148;


    strcpy(device_config.mqtt_host,
           "192.168.1.182");


    device_config.mqtt_port = 1883;


    strcpy(device_config.pwm_name[0],
           "PWM1");


    strcpy(device_config.pwm_name[1],
           "PWM2");


    strcpy(device_config.pwm_name[2],
           "PWM3");


    strcpy(device_config.pwm_name[3],
           "PWM4");


    strcpy(device_config.temperature_name,
           "DS18B20");

}



void config_init(void)
{

    nvs_flash_init();


    nvs_handle_t handle;


    esp_err_t err =
        nvs_open(
            NVS_NAMESPACE,
            NVS_READWRITE,
            &handle
        );


    if(err != ESP_OK)
    {
        load_defaults();
        return;
    }



    size_t size =
        sizeof(device_config_t);



    err =
        nvs_get_blob(
            handle,
            "device",
            &device_config,
            &size
        );


    if(err != ESP_OK)
    {
        load_defaults();

        nvs_set_blob(
            handle,
            "device",
            &device_config,
            sizeof(device_config_t)
        );


        nvs_commit(handle);
    }



    nvs_close(handle);

}



device_config_t *config_get(void)
{
    return &device_config;
}



void config_save(void)
{

    nvs_handle_t handle;


    if(nvs_open(
        NVS_NAMESPACE,
        NVS_READWRITE,
        &handle
    ) == ESP_OK)
    {

        nvs_set_blob(
            handle,
            "device",
            &device_config,
            sizeof(device_config_t)
        );


        nvs_commit(handle);


        nvs_close(handle);
    }

}