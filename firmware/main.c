#include "motor_driver.h"

/*
 * Demo firmware loop (platform-agnostic).
 * Replace motor_platform_* + pwm_platform_* hooks to run on real hardware.
 */

int main(void) {
    motor_config_t cfg = {
        .pwm_hz = 20000,
        .deadtime_us = 5,
        .duty_percent = 0,
        .mode = MOTOR_COAST,
        .enabled = true
    };

    motor_init(&cfg);

    // Forward ramp
    motor_safe_set(MOTOR_FORWARD, 60);

    // Simulate some runtime (placeholder)
    for (volatile uint32_t i = 0; i < 2000000; i++) { }

    // Reverse safely
    motor_safe_set(MOTOR_REVERSE, 50);

    for (volatile uint32_t i = 0; i < 2000000; i++) { }

    // Brake then coast
    motor_set_mode(MOTOR_BRAKE);
    for (volatile uint32_t i = 0; i < 500000; i++) { }
    motor_set_mode(MOTOR_COAST);

    while (1) {
        // Idle
    }
}