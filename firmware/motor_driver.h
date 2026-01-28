#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MOTOR_COAST = 0,   // outputs off (Hi-Z if driver supports it)
    MOTOR_BRAKE,       // both low-side on (or short motor terminals) depending on topology
    MOTOR_FORWARD,
    MOTOR_REVERSE
} motor_mode_t;

typedef struct {
    uint32_t pwm_hz;          // PWM frequency
    uint16_t deadtime_us;     // optional deadtime insertion (platform dependent)
    uint8_t  duty_percent;    // 0..100
    motor_mode_t mode;
    bool enabled;
} motor_config_t;

void motor_init(const motor_config_t *cfg);

/* High-level control */
void motor_set_mode(motor_mode_t mode);
motor_mode_t motor_get_mode(void);

void motor_set_speed(uint8_t duty_percent); // 0..100
uint8_t motor_get_speed(void);

void motor_enable(bool en);
bool motor_is_enabled(void);

/*
 * Safe update: ramps duty down before switching direction to reduce shoot-through risk.
 * (Still recommend real dead-time at the gate driver / timer level.)
 */
void motor_safe_set(motor_mode_t mode, uint8_t duty_percent);

/* Platform hooks (implement per target MCU / driver wiring) */
void motor_platform_gpio_init(void);
void motor_platform_set_outputs(motor_mode_t mode);
void motor_platform_delay_us(uint32_t us);

#endif