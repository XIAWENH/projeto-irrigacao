#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define NUM_LEITURA   10
#define SENSOR        26
#define RELE_5V       15
#define LIMITE_UMIDADE 30.0f
#define INTERVALO_MINUTOS 10        
#define QTD_VERIFICACOES 5          

float ler_umidade_media() {
    uint32_t soma = 0;
    for (int i = 0; i < NUM_LEITURA; i++) {
        adc_select_input(SENSOR - 26);
        soma += adc_read();
        sleep_ms(10);
    }
    float media = soma / (float)NUM_LEITURA;
    float umidade = 100.0f * (4095.0f - media) / 4095.0f;
    return umidade;
}

int main()
{
    stdio_init_all();
    gpio_init(RELE_5V);
    gpio_set_dir(RELE_5V, GPIO_OUT);
    adc_init();
    adc_gpio_init(SENSOR);

    while (true) {
        int solo_seco_detectado = 0;
        printf("Acordando para verificar o solo...\n");
        
        for (int i = 0; i < QTD_VERIFICACOES; i++) {
            float umidade = ler_umidade_media();
            printf("Verificação %d: Umidade = %.1f%%\n", i+1, umidade);
            if (umidade < LIMITE_UMIDADE) {
                solo_seco_detectado = 1; // Solo seco detectado!
            }
            sleep_ms(1000); // 1 segundo entre verificações
        }

        if (solo_seco_detectado) {
            gpio_put(RELE_5V, 1);  // Liga irrigação
            printf("Solo seco detectado! Irrigação LIGADA.\n");
            // Mantenha irrigação ligada por, ex, 1 minuto:
            sleep_ms(60000); // 60.000 ms = 1 minuto
            gpio_put(RELE_5V, 0);  // Desliga irrigação
            printf("Irrigação DESLIGADA. Dormindo até próxima checagem...\n");
        } else {
            printf("Solo úmido. Dormindo até próxima checagem...\n");
        }
        // Dorme por 10 minutos (600.000 ms)
        sleep_ms(INTERVALO_MINUTOS * 60 * 1000);
    }
}
