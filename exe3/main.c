#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;


volatile absolute_time_t press_time;
volatile bool button_pressed = false;
volatile bool flag_f_r = false;

void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN_R) {
        if (events == GPIO_IRQ_EDGE_FALL) { 
            press_time = get_absolute_time();
            button_pressed = true;
        } else if (events == GPIO_IRQ_EDGE_RISE) { 
            if (button_pressed) {
                int64_t elapsed_time = absolute_time_diff_us(press_time, get_absolute_time());
                if (elapsed_time > 500000) { // 500ms em microssegundos
                    flag_f_r = !flag_f_r; 
                }
                button_pressed = false;
            }
        }
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

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

        while (true) {
            gpio_put(LED_PIN_R, flag_f_r);
        }
}
