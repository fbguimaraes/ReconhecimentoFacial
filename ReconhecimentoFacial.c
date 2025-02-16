#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"


#define LED_VERDE 11   // Ajuste os pinos conforme necessário
#define LED_VERMELHO 13

#define BUZZER_PIN 21
#define BUZZER_FREQ_HIGH 2000  // Frequência do buzzer em Hz

void configurar_buzzer() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config config = pwm_get_default_config();
    float clk_freq = (float)clock_get_hz(clk_sys);
    pwm_config_set_clkdiv(&config, clk_freq / (BUZZER_FREQ_HIGH * 4096.0));

    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

// Função para tocar o buzzer
void tocar_buzzer(int duracao_ms) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_gpio_level(BUZZER_PIN, 2048); // 50% duty cycle
    sleep_ms(duracao_ms);
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

int main() {
    stdio_init_all();

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    configurar_buzzer();  // Inicializa o buzzer corretamente

    while (true) {
        int ch = getchar_timeout_us(1000000); // Espera um caractere na UART

        if (ch == '1') {
            gpio_put(LED_VERDE, 1);
            gpio_put(LED_VERMELHO, 0);
            tocar_buzzer(500);  // Opcional: Som curto ao ativar LED verde
        } else if (ch == '0') {
            gpio_put(LED_VERDE, 0);
            gpio_put(LED_VERMELHO, 1);
              // Opcional: Som mais longo ao ativar LED vermelho
        }
    }
}
