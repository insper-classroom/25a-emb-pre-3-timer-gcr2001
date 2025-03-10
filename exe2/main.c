#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int timer_running_r = 0;
volatile int timer_running_g = 0;
repeating_timer_t timer_r;
repeating_timer_t timer_g;

bool led_r_callback(repeating_timer_t *rt) {
    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
    return true;
}

bool led_g_callback(repeating_timer_t *rt) {
    gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        if (flag_r) {
            flag_r = 0;
            if (timer_running_r) {
                cancel_repeating_timer(&timer_r);
                gpio_put(LED_PIN_R, 0); 
                timer_running_r = 0;
            } else {
                add_repeating_timer_ms(500, led_r_callback, NULL, &timer_r);
                timer_running_r = 1;
            }
        }

        if (flag_g) {
            flag_g = 0;
            if (timer_running_g) {
                cancel_repeating_timer(&timer_g);
                gpio_put(LED_PIN_G, 0); 
                timer_running_g = 0;
            } else {
                add_repeating_timer_ms(250, led_g_callback, NULL, &timer_g);
                timer_running_g = 1;
            }
        }
    }
}
