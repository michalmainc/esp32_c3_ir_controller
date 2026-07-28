#include <stdio.h>

#include "config/config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void heartbeat_task(void *pvParameter)
{
    int counter = 0;

    while (1)
    {
        counter++;

        printf("Heartbeat: %d\n", counter);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


void app_main(void)
{

    config_init();


    device_config_t *cfg =
        config_get();


    printf("Device: %s\n",
           cfg->device_name);


    printf("IP: %s\n",
           cfg->ip);


    printf("MQTT: %s:%d\n",
           cfg->mqtt_host,
           cfg->mqtt_port);

}