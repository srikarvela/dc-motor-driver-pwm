#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Platform-agnostic PWM interface.
 * You can port the "pwm_platform_*" functions to:
 *  - STM32 HAL (TIMx PWM)
 *  - ESP32 LEDC
 *  - AVR Timer/Counter
 *  - Any MCU timer peripheral
 */

typedef struct {
    uint32_t pwm_hz;        // PWM frequency
    uint8_t  duty_percent;  // 0..100
    bool     enabled;
} pwm_config_t;

void pwm_init(const pwm_config_t *cfg);
void pwm_set_duty(uint8_t duty_percent);  // clamps to 0..100
uint8_t pwm_get_duty(void);

void pwm_enable(bool en);
bool pwm_is_enabled(void);

/* Platform hooks (implement per target MCU) */
void pwm_platform_timer_init(uint32_t pwm_hz);
void pwm_platform_timer_set_duty(uint8_t duty_percent);
void pwm_platform_timer_enable(bool en);

#endif