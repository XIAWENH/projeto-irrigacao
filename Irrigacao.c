#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define NUM_LEITURA 10
#define SENSOR 26
#define RELE_5V 15
#define LIMITE_UMIDADE 30.0f 


float ler_umidade_media(uint16_t *raw_adc) {
    uint32_t soma = 0;
    for (int i = 0; i < NUM_LEITURA; i++) {
        adc_select_input(SENSOR - 26); 
        soma += adc_read();
        sleep_ms(10);
    }
    float media = soma / (float)NUM_LEITURA;
    if (raw_adc) *raw_adc = (uint16_t)media;
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
        uint16_t adc_raw = 0;
        float umidade = ler_umidade_media(&adc_raw);
        printf("Umidade: %.1f%% | ADC medio: %u\n", umidade, adc_raw);

        if(umidade < LIMITE_UMIDADE){
            gpio_put(RELE_5V, 1);
            printf("Solo seco! Irrigação LIGADA.\n");
        }
        else{
            gpio_put(RELE_5V, 0);
            printf("Solo úmido. Irrigação DESLIGADA.\n");
        }
        sleep_ms(1000);
    }
}
