#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int timer_running = 0;
repeating_timer_t timer;

bool led_callback(repeating_timer_t *rt) {
    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
    return true; 
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) { // fall edge
        flag_f_r = 1;
    } else if (events == 0x8) { // rise edge
    }
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    while (true) {
        if (flag_f_r) {
            flag_f_r = 0;
            if (timer_running) {
                cancel_repeating_timer(&timer);
                gpio_put(LED_PIN_R, 0); // LED sempre termina apagado
                timer_running = 0;
            } else {
                add_repeating_timer_ms(500, led_callback, NULL, &timer);
                timer_running = 1;
            }
        }
    }
}
