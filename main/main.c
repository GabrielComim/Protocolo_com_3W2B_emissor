/**
 * Utilizado um ESP32 para fazer a comunicação com outro ESP32 por protocolo proprietário 3W2B
 * Protocolo serial - Sinais de clk, sync, data
 * Envia 2 byte por vez - 16 bits
 * 
 * Este é o programa do ESP32 que envia os dados
*/

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"

gpio_num_t CLK = GPIO_NUM_23;
gpio_num_t SYNC = GPIO_NUM_22;
gpio_num_t DATA = GPIO_NUM_21;

void gpio_cfg(gpio_num_t GPIO_NUM)
{
    gpio_config_t gpio_cfg = 
    {
        .pin_bit_mask = (1 << GPIO_NUM),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = false
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
}

/* Envia 2 bytes seguindo o protocolo 3w2b */
void send_3w2b(uint16_t dataTX)
{
    int i;
    uint16_t mask = 0b1000000000000000;

    for(i = 0; i < 16; i++)
    {
        gpio_set_level(DATA, (dataTX << i) & mask);
        vTaskDelay(pdMS_TO_TICKS(1));
        gpio_set_level(CLK, 1);
        vTaskDelay(pdMS_TO_TICKS(1));
        gpio_set_level(CLK, 0);
        vTaskDelay(pdMS_TO_TICKS(1));
    }   /* END FOR */

    gpio_set_level(DATA, 0);
    gpio_set_level(SYNC, 1);
    vTaskDelay(pdMS_TO_TICKS(3));
    gpio_set_level(SYNC, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
}

void app_main(void)
{
    uint16_t aux = 0b0100101100111011;

    // for(int i = 0; i < 16; i++)
    // {
        // ESP_LOGI("RESULT", "[%d] = %d", i, (aux << i) & mask);    
    // }

    /* Configura as GPIOs */
    gpio_cfg(CLK);
    gpio_cfg(SYNC);
    gpio_cfg(DATA);
    
    while(true)
    {
        /* Chama a função que envia os dados via serial */
        send_3w2b(aux);
        // ESP_LOGI("MAIN", "DATA = %d", aux);
    }
}
