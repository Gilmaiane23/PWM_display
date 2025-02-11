#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include <stdio.h>
#include "ssd1306.h"

#define SERVO_GPIO 20
#define LED_GPIO 12
#define PWM_FREQ 50 // Frequência do PWM em Hz
#define PERIOD_US 20000 // Período de 20ms
#define STEP_DELAY 10 // Atraso entre mudanças de posição (ms)

//i2c
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C
ssd1306_t ssd; // Inicializa a estrutura do display


// Função para configurar o ciclo de trabalho (duty cycle) do PWM
void set_pwm_duty(uint gpio, uint16_t duty_us) {
    uint slice_num = pwm_gpio_to_slice_num(gpio); // Obtém o slice do PWM correspondente ao GPIO
    uint wrap = clock_get_hz(clk_sys) / (PWM_FREQ * 64); // Calcula o valor de "wrap" com base na frequência do clock
    pwm_set_wrap(slice_num, wrap); // Define o valor de wrap para o PWM
    pwm_set_chan_level(slice_num, PWM_CHAN_A, (wrap * duty_us) / PERIOD_US); // Define o nível de ciclo ativo do PWM
    pwm_set_enabled(slice_num, true); // Habilita o PWM para o slice correspondente
}

// Manipulador de interrupção para atualizar o ciclo de trabalho do servo e LED
void wrapHandler() {
    static uint16_t duty = 500;
    static bool increasing = true;
    static bool initialized = false;
    uint slice_servo = pwm_gpio_to_slice_num(SERVO_GPIO);
    uint slice_led = pwm_gpio_to_slice_num(LED_GPIO);
    
    pwm_clear_irq(slice_servo); // Limpa a interrupção do PWM do servo
    pwm_clear_irq(slice_led); // Limpa a interrupção do PWM do LED
    
    if (!initialized) {
        duty = 500; // Garante que a oscilação comece a partir de 0 graus
        initialized = true;
    }
    
    set_pwm_duty(SERVO_GPIO, duty); // Define o PWM do servo
    set_pwm_duty(LED_GPIO, duty * duty); // Define o PWM do LED baseado no quadrado do duty cycle para suavizar transições
    
    // Alterna entre aumentar e diminuir o ciclo de trabalho
    if (increasing) {
        duty += 5;
        if (duty >= 2400) increasing = false;
        // Atualiza a mensagem para 180°
            ssd1306_fill(&ssd, false); // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0); // Desenha um retângulo
            char mensagem[30];
            snprintf(mensagem, sizeof(mensagem), "SERVO 180°");
            ssd1306_draw_string(&ssd, mensagem, 10, 30); // Exibe a mensagem no display
            ssd1306_send_data(&ssd); // Atualiza o display
    } else {
        duty -= 5;
        if (duty <= 500) increasing = true;
        // Atualiza a mensagem para 0° quando atingir o mínimo
            ssd1306_fill(&ssd, false); // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0); // Desenha um retângulo
            char mensagem[30];
            snprintf(mensagem, sizeof(mensagem), "SERVO 0°");
            ssd1306_draw_string(&ssd, mensagem, 10, 30); // Exibe a mensagem no display
            ssd1306_send_data(&ssd); // Atualiza o display
    }

     // Atualiza a mensagem para 90° quando o duty atingir o valor intermediário
    if (duty == 1470) {
        ssd1306_fill(&ssd, false); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0); // Desenha um retângulo
        char mensagem[30];
        snprintf(mensagem, sizeof(mensagem), "SERVO 90°");
        ssd1306_draw_string(&ssd, mensagem, 10, 30); // Exibe a mensagem no display
        ssd1306_send_data(&ssd); // Atualiza o display
    }
}

// Configuração do PWM com interrupção
void pwm_setup_irq(uint gpio) {
    uint slice_num = pwm_gpio_to_slice_num(gpio); // Obtém o número do slice do PWM
    gpio_set_function(gpio, GPIO_FUNC_PWM); // Configura o GPIO como saída PWM
    pwm_clear_irq(slice_num); // Limpa qualquer interrupção pendente
    pwm_set_irq_enabled(slice_num, true); // Habilita a interrupção do PWM
    irq_set_exclusive_handler(PWM_IRQ_WRAP, wrapHandler); // Define o manipulador de interrupção
    irq_set_enabled(PWM_IRQ_WRAP, true); // Habilita a interrupção no sistema
    
    pwm_config config = pwm_get_default_config(); // Obtém a configuração padrão do PWM
    pwm_config_set_clkdiv(&config, 64.0); // Define o divisor de clock para ajustar a frequência
    pwm_init(slice_num, &config, true); // Inicializa o PWM com a configuração definida
}

int main() {
    stdio_init_all();

     // Inicialização do I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    
    // Exibição de animação e mensagens no display
    bool cor = true;
    cor = !cor;
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "Bem Vindo", 30, 10); // Exibe a primeira string
    ssd1306_draw_string(&ssd, "Embarcatech", 26, 30); // Exibe a segunda string
    ssd1306_draw_string(&ssd, "Gilmaiane", 30, 48); // Exibe a terceira string
    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(2000);

    gpio_set_function(SERVO_GPIO, GPIO_FUNC_PWM); // Configura o GPIO do servo como saída PWM
    gpio_set_function(LED_GPIO, GPIO_FUNC_PWM); // Configura o GPIO do LED como saída PWM
    uint slice_num_servo = pwm_gpio_to_slice_num(SERVO_GPIO);
    uint slice_num_led = pwm_gpio_to_slice_num(LED_GPIO);
    pwm_set_clkdiv(slice_num_servo, 64.0); // Configura o divisor de clock para o servo
    pwm_set_clkdiv(slice_num_led, 64.0); // Configura o divisor de clock para o LED
    
    // Define posições iniciais do servo e LED
    set_pwm_duty(SERVO_GPIO, 2400); // Servo a 180 graus
    set_pwm_duty(LED_GPIO, 2400);
    // Exibição de mensagem de 180 graus no display
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0); // Desenha um retângulo
    char mensagem[30];
    snprintf(mensagem, sizeof(mensagem), "SERVO 180°"); // Mensagem
    ssd1306_draw_string(&ssd, mensagem, 10, 30); // Exibe a mensagem no display
    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(5000);
    
    set_pwm_duty(SERVO_GPIO, 1470); // Servo a 90 graus
    set_pwm_duty(LED_GPIO, 1470);
    ssd1306_fill(&ssd, false); // Limpa o display
    snprintf(mensagem, sizeof(mensagem), "SERVO 90°"); // Mensagem
    ssd1306_draw_string(&ssd, mensagem, 10, 30); // Exibe a mensagem no display
    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(5000);
    
    set_pwm_duty(SERVO_GPIO, 500); // Servo a 0 graus
    set_pwm_duty(LED_GPIO, 500);
    ssd1306_fill(&ssd, false); // Limpa o display
    snprintf(mensagem, sizeof(mensagem), "SERVO 0°"); // Mensagem
    ssd1306_draw_string(&ssd, mensagem, 10, 30); // Exibe a mensagem no display
    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(5000);
    
    // Configura e ativa as interrupções do PWM
    pwm_setup_irq(SERVO_GPIO);
    pwm_setup_irq(LED_GPIO);
    
    // Loop infinito para manter o programa em execução
    while (true) {
        tight_loop_contents();
    }
}