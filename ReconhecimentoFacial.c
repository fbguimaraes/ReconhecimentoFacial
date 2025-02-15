#include <stdio.h>
#include "pico/stdlib.h"

#define LED_VERDE 11  // Ajuste os pinos conforme necessário
#define LED_VERMELHO 13

int main() {
    stdio_init_all();
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    while (true) {
        int ch = getchar_timeout_us(1000000); // Espera um caractere na UART

        if (ch == '1') {
            gpio_put(LED_VERDE, 1);
            gpio_put(LED_VERMELHO, 0);
        } else if (ch == '0') {
            gpio_put(LED_VERDE, 0);
            gpio_put(LED_VERMELHO, 1);
        }
    }
}
