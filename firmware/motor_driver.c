#include "motor_driver.h"
#include "pwm_control.h"

static motor_config_t g_motor = {
    .pwm_hz = 20000,
    .deadtime_us = 2,
    .duty_percent = 0,
    .mode = MOTOR_COAST,
    .enabled = false
};

static uint8_t clamp_u8(uint8_t v, uint8_t lo, uint8_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void motor_init(const motor_config_t *cfg) {
    if (cfg) g_motor = *cfg;

    g_motor.duty_percent = clamp_u8(g_motor.duty_percent, 0, 100);

    motor_platform_gpio_init();
    motor_platform_set_outputs(MOTOR_COAST);

    pwm_config_t pcfg = {
        .pwm_hz = g_motor.pwm_hz,
        .duty_percent = 0,
        .enabled = false
    };
    pwm_init(&pcfg);

    motor_enable(g_motor.enabled);
    motor_set_mode(g_motor.mode);
    motor_set_speed(g_motor.duty_percent);
}

void motor_set_mode(motor_mode_t mode) {
    g_motor.mode = mode;

    // If disabled, force coast regardless of requested mode
    if (!g_motor.enabled) {
        motor_platform_set_outputs(MOTOR_COAST);
        pwm_enable(false);
        return;
    }

    motor_platform_set_outputs(mode);

    // For COAST/BRAKE we typically disable PWM (implementation-dependent)
    if (mode == MOTOR_COAST || mode == MOTOR_BRAKE) {
        pwm_enable(false);
    } else {
        pwm_enable(true);
    }
}

motor_mode_t motor_get_mode(void) {
    return g_motor.mode;
}

void motor_set_speed(uint8_t duty_percent) {
    g_motor.duty_percent = clamp_u8(duty_percent, 0, 100);

    if (!g_motor.enabled) {
        pwm_set_duty(0);
        pwm_enable(false);
        return;
    }

    // Only apply PWM in forward/reverse
    if (g_motor.mode == MOTOR_FORWARD || g_motor.mode == MOTOR_REVERSE) {
        pwm_set_duty(g_motor.duty_percent);
        pwm_enable(true);
    } else {
        pwm_set_duty(0);
        pwm_enable(false);
    }
}

uint8_t motor_get_speed(void) {
    return g_motor.duty_percent;
}

void motor_enable(bool en) {
    g_motor.enabled = en;

    if (!en) {
        pwm_set_duty(0);
        pwm_enable(false);
        motor_platform_set_outputs(MOTOR_COAST);
    } else {
        // restore last requested mode/speed
        motor_set_mode(g_motor.mode);
        motor_set_speed(g_motor.duty_percent);
    }
}

bool motor_is_enabled(void) {
    return g_motor.enabled;
}

void motor_safe_set(motor_mode_t mode, uint8_t duty_percent) {
    duty_percent = clamp_u8(duty_percent, 0, 100);

    // If direction change, ramp down first (simple but effective)
    bool changing_direction =
        (g_motor.mode == MOTOR_FORWARD && mode == MOTOR_REVERSE) ||
        (g_motor.mode == MOTOR_REVERSE && mode == MOTOR_FORWARD);

    if (changing_direction && g_motor.enabled) {
        // Quick linear ramp down
        for (int d = (int)g_motor.duty_percent; d >= 0; d -= 10) {
            motor_set_speed((uint8_t)d);
            motor_platform_delay_us(2000); // 2 ms step
        }

        // Coast briefly to avoid overlap (in addition to deadtime)
        motor_platform_set_outputs(MOTOR_COAST);
        pwm_enable(false);
        motor_platform_delay_us((uint32_t)g_motor.deadtime_us);

        motor_set_mode(mode);

        // Ramp up
        for (int d = 0; d <= (int)duty_percent; d += 10) {
            motor_set_speed((uint8_t)d);
            motor_platform_delay_us(2000);
        }
    } else {
        motor_set_mode(mode);
        motor_set_speed(duty_percent);
    }
}

/* -------------------------
 * Platform hooks (STUBS)
 * -------------------------
 * Replace these with GPIO writes for your wiring:
 * Example pins:
 *   IN1/IN2 for direction
 *   EN/PWM pin for timer PWM channel
 */

void motor_platform_gpio_init(void) {
    // TODO: configure GPIO pins for H-bridge inputs as outputs
}

void motor_platform_set_outputs(motor_mode_t mode) {
    (void)mode;
    // TODO:
    // MOTOR_FORWARD: IN1=1 IN2=0
    // MOTOR_REVERSE: IN1=0 IN2=1
    // MOTOR_BRAKE  : IN1=1 IN2=1 (or both lows depending on driver)
    // MOTOR_COAST  : IN1=0 IN2=0
}

void motor_platform_delay_us(uint32_t us) {
    (void)us;
    // TODO: implement microsecond delay (timer-based busy wait, etc.)
}