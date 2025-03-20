#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"


// Definição dos pinos dos LEDs
#define LED_VERDE 11     
#define LED_VERMELHO 13  

// Pino do buzzer e frequência
#define BUZZER_PIN 21
#define BUZZER_FREQ_HIGH 2000  

// Estados para os LEDs
#define LIGADO 1
#define DESLIGADO 0


#define WIFI_SSID "Labirang"
#define WIFI_PASS "1fp1*007"
// Configura o buzzer para fazer som
void configurar_buzzer() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    
    pwm_config config = pwm_get_default_config();
    float clk_freq = (float)clock_get_hz(clk_sys);
    pwm_config_set_clkdiv(&config, clk_freq / (BUZZER_FREQ_HIGH * 4096.0));

    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(BUZZER_PIN, 0); // Começa desligado
}

// Faz o buzzer tocar por um tempo determinado
void tocar_buzzer(int duracao_ms) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_gpio_level(BUZZER_PIN, 2048); // Ativa o som
    sleep_ms(duracao_ms);
    pwm_set_gpio_level(BUZZER_PIN, 0); // Desliga o som
}

// Configura os LEDs como saída
void configurar_leds() {
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, DESLIGADO);

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, DESLIGADO);
}



int main() {
    stdio_init_all(); // Inicializa comunicação

    
    configurar_leds();    
    configurar_buzzer(); 


    while (true) {
        int ch = getchar_timeout_us(1000000); // Espera um comando por 1 segundo

        if (ch == PICO_ERROR_TIMEOUT) {
            continue; // Se não receber nada, tenta de novo
        }

        if (ch == '1') { // Se receber '1', liga o LED verde e toca o buzzer
            gpio_put(LED_VERDE, LIGADO);
            gpio_put(LED_VERMELHO, DESLIGADO);
            tocar_buzzer(500);
        } else if (ch == '0') { // Se receber '0', liga o LED vermelho e toca o buzzer por mais tempo
            gpio_put(LED_VERDE, DESLIGADO);
            gpio_put(LED_VERMELHO, LIGADO);
            
        }

        sleep_ms(10); // Pequeno atraso para evitar problemas na leitura
    }
}
