# Controle de Servo Motor e Display OLED com Raspberry Pi Pico

## Descrição
Este projeto implementa o controle de um servo motor e um LED via PWM utilizando o Raspberry Pi Pico. Além disso, um display OLED SSD1306 é utilizado para exibir informações sobre a posição do servo.

## Componentes Utilizados
- Raspberry Pi Pico
- Servo Motor
- LED
- Display OLED SSD1306 (I2C)
- Resistores Pull-up (para o barramento I2C)
- Jumpers para conexão

## Biblioteca Utilizadas
- `pico/stdlib.h` - Biblioteca padrão do Raspberry Pi Pico
- `hardware/pwm.h` - Biblioteca para controle do PWM
- `hardware/irq.h` - Biblioteca para interrupções
- `hardware/clocks.h` - Biblioteca para configuração do clock
- `ssd1306.h` - Biblioteca para controle do display OLED

## Pinagem
| Componente  | GPIO | Descrição |
|------------|------|-------------|
| Servo Motor | 20   | Controle via PWM |
| LED        | 12   | Controle via PWM |
| SDA (I2C)  | 14   | Barramento I2C |
| SCL (I2C)  | 15   | Barramento I2C |

## Configuração do PWM
- Frequência: 50 Hz
- Período: 20 ms
- Posição 0°: 500us
- Posição 90°: 1470us
- Posição 180°: 2400us

## Funcionamento
1. O servo oscila entre 0°, 90° e 180°.
2. O LED acompanha a intensidade do PWM.
3. O display OLED exibe a posição atual do servo.
4. As mensagens no display mudam conforme a posição do servo.

## Instalação e Uso
1. Conecte os componentes conforme a tabela de pinagem.
2. Compile e carregue o programa no Raspberry Pi Pico.
3. O sistema iniciará exibindo uma mensagem de boas-vindas no OLED.
4. O servo motor se moverá automaticamente entre as posições 0°, 90° e 180°.

## Autores
- Gilmaiane

## Licença
Este projeto é de livre uso para fins educacionais e não possui restrição de licenciamento.

