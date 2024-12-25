/*
Nome: Joamerson Islan Santos Amaral
Matrícula: 202421511721050
*/
                     
#include "hardware/clocks.h"  
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <stdio.h>

// Pinos do Raspberry 
#define PINO_LED_CARROS_VERDE 0
#define PINO_LED_CARROS_AMARELO 2
#define PINO_LED_CARROS_VERMELHO 5
#define PINO_LED_PEDESTRES_VERDE 8
#define PINO_BOTAO_PEDESTRES 10
#define PINO_BUZZER 12

#define FREQUENCIA_BUZZER 1000 // Frequência buzzer -> 1000 Hz

// Função para inicializar os pinos do Raspberry
void inicializar_pinos() {
  stdio_init_all();
  gpio_init(PINO_LED_CARROS_VERDE);
  gpio_init(PINO_LED_CARROS_AMARELO);
  gpio_init(PINO_LED_CARROS_VERMELHO);
  gpio_init(PINO_LED_PEDESTRES_VERDE);
  gpio_init(PINO_BOTAO_PEDESTRES);
  gpio_init(PINO_BUZZER);

  // Configuram os pinos dos LEDs e do buzzer no modo saída
  gpio_set_dir(PINO_LED_CARROS_VERDE, GPIO_OUT);
  gpio_set_dir(PINO_LED_CARROS_AMARELO, GPIO_OUT);
  gpio_set_dir(PINO_LED_CARROS_VERMELHO, GPIO_OUT);
  gpio_set_dir(PINO_LED_PEDESTRES_VERDE, GPIO_OUT);
  gpio_set_dir(PINO_BUZZER, GPIO_OUT);

  gpio_set_dir(PINO_BOTAO_PEDESTRES, GPIO_IN); // Configura o botão do pedestre como um input GPIO
  gpio_pull_up(PINO_BOTAO_PEDESTRES); // Ativa o resistor pull-up no botão do pedestre
}

// Função para inicializar o PWM no pino do buzzer
void iniciar_pwm_buzzler(uint pino) {
  gpio_set_function(pino, GPIO_FUNC_PWM); 
  uint slice_num = pwm_gpio_to_slice_num(pino); 

  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (FREQUENCIA_BUZZER * 4096)); 
  pwm_init(slice_num, &config, true); 
  pwm_set_gpio_level(pino, 0);
}

// Função para emitir um som no buzzer durante um período de tempo
void emitir_som(uint pino, uint duracao) {
  uint slice_num = pwm_gpio_to_slice_num(pino); 
  pwm_set_gpio_level(pino, 1024);
  sleep_ms(duracao);
  pwm_set_gpio_level(pino, 0);
  sleep_ms(100);
}

// Função para alternar os LEDs referentes ao semáforo dos carros
void alternar_leds_carros() {
  gpio_put(PINO_LED_CARROS_VERDE, 1); // Acende o LED verde do sinal para carros
  // Loop for para contabilizar 8 segundos
  for(int i=0;i<8000;i++){
    sleep_ms(1);
    if (gpio_get(PINO_BOTAO_PEDESTRES) == 0){
      gpio_put(PINO_LED_CARROS_VERDE, 0); // Apaga o verde dos carros imediatamente
      controlar_sequencia_pedestres();
      return;
    }
  }

  gpio_put(PINO_LED_CARROS_VERDE, 0); // Desliga o LED verde do sinal para carros
  gpio_put(PINO_LED_CARROS_AMARELO, 1); // Liga o LED amarelo do sinal para carros
  // Loop for para contabilizar 2 segundos
  for(int i=0;i<2000;i++){
    sleep_ms(1);
    if (gpio_get(PINO_BOTAO_PEDESTRES) == 0){
      gpio_put(PINO_LED_CARROS_VERDE, 0); 
      controlar_sequencia_pedestres();
      return;
    }
  }

  gpio_put(PINO_LED_CARROS_AMARELO, 0); // Desliga o LED amarelo do sinal para carros
  gpio_put(PINO_LED_CARROS_VERMELHO, 1); // Liga o LED vermelho do sinal para carros
  // Loop for para contabilizar 10 segundos
  for(int i=0;i<10000;i++){
    sleep_ms(1);
    if (gpio_get(PINO_BOTAO_PEDESTRES) == 0){
      gpio_put(PINO_LED_CARROS_VERDE, 0); 
      controlar_sequencia_pedestres();
      return;
    }
  }

  gpio_put(PINO_LED_CARROS_VERMELHO, 0); // Desliga o LED vermelho do sinal para carros
}

// Função que controla o translado dos pedestres
void controlar_sequencia_pedestres() {
  gpio_put(PINO_LED_CARROS_VERDE, false);
  gpio_put(PINO_LED_CARROS_AMARELO, true);
  gpio_put(PINO_LED_CARROS_VERMELHO, false);
  sleep_ms(5000);
  gpio_put(PINO_LED_PEDESTRES_VERDE, true);
  gpio_put(PINO_LED_CARROS_VERDE, false);
  gpio_put(PINO_LED_CARROS_AMARELO, false);
  gpio_put(PINO_LED_CARROS_VERMELHO, true);  
  emitir_som(PINO_BUZZER, 15000);
  gpio_put(PINO_LED_CARROS_VERMELHO, false); 
  gpio_put(PINO_LED_PEDESTRES_VERDE, false);
}

// Função principal do programa, responsável por orquestrar as demais
int main() {
  inicializar_pinos(); // Configura os pinos
  iniciar_pwm_buzzler(PINO_BUZZER); // Inicializa o PWM para o buzzer

  while(true) {
    alternar_leds_carros();  // Controla os LEDs referentes aos semáforos dos carros
  }

  return 0;  
}