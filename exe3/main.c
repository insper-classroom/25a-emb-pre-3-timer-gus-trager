#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile bool btn_down_flag = false;
volatile bool btn_up_flag = false;

void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN_R) {
        if (events & GPIO_IRQ_EDGE_FALL) {
            btn_down_flag = true;
        } else if (events & GPIO_IRQ_EDGE_RISE) {
            btn_up_flag = true;
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    absolute_time_t press_start_time = 0;
    bool aguardando_soltar = false;

    while (true) {
        if (btn_down_flag) {
            press_start_time = get_absolute_time();
            aguardando_soltar = true;
            btn_down_flag = false;
        }
        if (btn_up_flag && aguardando_soltar) {
            absolute_time_t press_end_time = get_absolute_time();
            int64_t diff_us = absolute_time_diff_us(press_start_time, press_end_time);
            if (diff_us >= 500000) {
                bool led_state = gpio_get(LED_PIN_R);
                gpio_put(LED_PIN_R, !led_state);
            }
            aguardando_soltar = false;
            btn_up_flag = false;
        }
    }
    return 0;
}
