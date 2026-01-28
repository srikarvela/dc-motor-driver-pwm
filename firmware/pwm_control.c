#include "pwm_control.h"

static pwm_config_t g_pwm = {
    .pwm_hz = 20000,        // default 20 kHz (inaudible for most motors)
    .duty_percent = 0,
    .enabled = false
};

static uint8_t clamp_u8(uint8_t v, uint8_t lo, uint8_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void pwm_init(const pwm_config_t *cfg) {
    if (cfg) {
        g_pwm.pwm_hz = cfg->pwm_hz;
        g_pwm.duty_percent = clamp_u8(cfg->duty_percent, 0, 100);
        g_pwm.enabled = cfg->enabled;
    }

    pwm_platform_timer_init(g_pwm.pwm_hz);
    pwm_platform_timer_set_duty(g_pwm.duty_percent);
    pwm_platform_timer_enable(g_pwm.enabled);
}

void pwm_set_duty(uint8_t duty_percent) {
    g_pwm.duty_percent = clamp_u8(duty_percent, 0, 100);
    pwm_platform_timer_set_duty(g_pwm.duty_percent);
}

uint8_t pwm_get_duty(void) {
    return g_pwm.duty_percent;
}

void pwm_enable(bool en) {
    g_pwm.enabled = en;
    pwm_platform_timer_enable(en);
}

bool pwm_is_enabled(void) {
    return g_pwm.enabled;
}

/* -------------------------
 * Platform hooks (STUBS)
 * -------------------------
 * These stubs make the project compile conceptually.
 * Replace with target-specific timer code.
 */

void pwm_platform_timer_init(uint32_t pwm_hz) {
    (void)pwm_hz;
    // TODO: Configure hardware timer for PWM frequency and mode.
}

void pwm_platform_timer_set_duty(uint8_t duty_percent) {
    (void)duty_percent;
    // TODO: Write duty to timer compare register.
}

void pwm_platform_timer_enable(bool en) {
    (void)en;
    // TODO: Enable/disable PWM output pin or timer channel.
}