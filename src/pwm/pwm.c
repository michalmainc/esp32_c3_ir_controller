#include "pwm.h"

#include "driver/ledc.h"
#include "esp_log.h"


#define PWM_FREQUENCY_HZ       400
#define PWM_DUTY_RESOLUTION    LEDC_TIMER_12_BIT
#define PWM_MAX_DUTY           4095
#define PWM_TIMER              LEDC_TIMER_0
#define PWM_SPEED_MODE         LEDC_LOW_SPEED_MODE


static const char *TAG = "PWM";


static const int pwm_gpio[PWM_CHANNEL_COUNT] = {
    2,
    3,
    4,
    5
};


static const ledc_channel_t pwm_ledc_channel[PWM_CHANNEL_COUNT] = {
    LEDC_CHANNEL_0,
    LEDC_CHANNEL_1,
    LEDC_CHANNEL_2,
    LEDC_CHANNEL_3
};


static uint8_t pwm_percent[PWM_CHANNEL_COUNT] = {
    0,
    0,
    0,
    0
};


static uint32_t percent_to_duty(uint8_t percent)
{
    if (percent >= 100)
    {
        return PWM_MAX_DUTY;
    }

    return ((uint32_t)percent * PWM_MAX_DUTY) / 100;
}


esp_err_t pwm_init(void)
{
    ESP_LOGI(
        TAG,
        "Inicjalizacja PWM: %d Hz, 4 kanaly",
        PWM_FREQUENCY_HZ
    );

    ledc_timer_config_t timer_config = {
        .speed_mode = PWM_SPEED_MODE,
        .duty_resolution = PWM_DUTY_RESOLUTION,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FREQUENCY_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };

    esp_err_t result = ledc_timer_config(&timer_config);

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Blad konfiguracji timera PWM: %s",
            esp_err_to_name(result)
        );

        return result;
    }

    for (uint8_t i = 0; i < PWM_CHANNEL_COUNT; i++)
    {
        ledc_channel_config_t channel_config = {
            .gpio_num = pwm_gpio[i],
            .speed_mode = PWM_SPEED_MODE,
            .channel = pwm_ledc_channel[i],
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = PWM_TIMER,
            .duty = 0,
            .hpoint = 0,
            .flags.output_invert = 0
        };

        result = ledc_channel_config(&channel_config);

        if (result != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Blad konfiguracji PWM%d na GPIO%d: %s",
                i + 1,
                pwm_gpio[i],
                esp_err_to_name(result)
            );

            return result;
        }

        pwm_percent[i] = 0;

        ESP_LOGI(
            TAG,
            "PWM%d: GPIO%d, wartosc poczatkowa 0%%",
            i + 1,
            pwm_gpio[i]
        );
    }

    ESP_LOGI(TAG, "Wszystkie kanaly PWM uruchomione");

    return ESP_OK;
}


esp_err_t pwm_set_percent(
    uint8_t channel_index,
    uint8_t percent
)
{
    if (channel_index >= PWM_CHANNEL_COUNT)
    {
        ESP_LOGE(
            TAG,
            "Nieprawidlowy numer kanalu: %d",
            channel_index
        );

        return ESP_ERR_INVALID_ARG;
    }

    if (percent > 100)
    {
        ESP_LOGE(
            TAG,
            "Nieprawidlowa wartosc PWM: %d%%",
            percent
        );

        return ESP_ERR_INVALID_ARG;
    }

    uint32_t duty = percent_to_duty(percent);

    esp_err_t result = ledc_set_duty(
        PWM_SPEED_MODE,
        pwm_ledc_channel[channel_index],
        duty
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna ustawic PWM%d: %s",
            channel_index + 1,
            esp_err_to_name(result)
        );

        return result;
    }

    result = ledc_update_duty(
        PWM_SPEED_MODE,
        pwm_ledc_channel[channel_index]
    );

    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Nie mozna zatwierdzic PWM%d: %s",
            channel_index + 1,
            esp_err_to_name(result)
        );

        return result;
    }

    pwm_percent[channel_index] = percent;

    ESP_LOGI(
        TAG,
        "PWM%d GPIO%d ustawiono na %d%%, duty=%lu",
        channel_index + 1,
        pwm_gpio[channel_index],
        percent,
        (unsigned long)duty
    );

    return ESP_OK;
}


uint8_t pwm_get_percent(uint8_t channel_index)
{
    if (channel_index >= PWM_CHANNEL_COUNT)
    {
        return 0;
    }

    return pwm_percent[channel_index];
}