#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#include "esp_err.h"


#define PWM_CHANNEL_COUNT 4


/**
 * Inicjalizuje cztery kanały PWM.
 *
 * GPIO2 - PWM1
 * GPIO3 - PWM2
 * GPIO4 - PWM3
 * GPIO5 - PWM4
 *
 * Częstotliwość: 400 Hz
 * Wartość początkowa: 0%
 */
esp_err_t pwm_init(void);


/**
 * Ustawia wypełnienie wybranego kanału.
 *
 * channel_index:
 * 0 - PWM1
 * 1 - PWM2
 * 2 - PWM3
 * 3 - PWM4
 *
 * percent:
 * zakres 0-100
 */
esp_err_t pwm_set_percent(
    uint8_t channel_index,
    uint8_t percent
);


/**
 * Zwraca aktualną wartość PWM w procentach.
 *
 * W przypadku nieprawidłowego kanału zwraca 0.
 */
uint8_t pwm_get_percent(uint8_t channel_index);


#endif