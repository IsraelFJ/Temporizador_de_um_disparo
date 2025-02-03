#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"

#define LED1 11
#define LED2 12
#define LED3 13
#define BUTTON 5

volatile bool timer_active = false;

// Callback para desligar LED3 e ligar apenas LED2
int64_t turn_off_led3(alarm_id_t id, void *user_data) {
    gpio_put(LED3, 0);
    return 3000000; // Próximo callback após 3s
}

// Callback para desligar LED2 e manter apenas LED1 ligado
int64_t turn_off_led2(alarm_id_t id, void *user_data) {
    gpio_put(LED2, 0);
    return 3000000; // Próximo callback após 3s
}

// Callback final para desligar todos os LEDs
int64_t turn_off_led1(alarm_id_t id, void *user_data) {
    gpio_put(LED1, 0);
    timer_active = false; // Permitir novo acionamento pelo botão
    return 0; // Fim da sequência
}

// Callback principal ao pressionar o botão
void button_callback(uint gpio, uint32_t events) {
    if (!timer_active) {
        timer_active = true;
        gpio_put(LED1, 1);
        gpio_put(LED2, 1);
        gpio_put(LED3, 1);
        add_alarm_in_ms(3000, turn_off_led3, NULL, true);
        add_alarm_in_ms(6000, turn_off_led2, NULL, true);
        add_alarm_in_ms(9000, turn_off_led1, NULL, true);
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED1);
    gpio_init(LED2);
    gpio_init(LED3);
    gpio_init(BUTTON);

    gpio_set_dir(LED1, GPIO_OUT);
    gpio_set_dir(LED2, GPIO_OUT);
    gpio_set_dir(LED3, GPIO_OUT);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);

    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_RISE, true, &button_callback);

    while (1) {
        sleep_ms(10); // Pequeno delay para evitar uso excessivo da CPU
    }
}
