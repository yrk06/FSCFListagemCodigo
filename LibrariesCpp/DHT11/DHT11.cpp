#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include <string>
#include <cmath>
#include "esp32/rom/ets_sys.h"
#include "soc/soc.h"
#include "DHT11.h"

void DHT11::readData(float* RH,float* T){

    
    bool* response = (bool*)malloc(RESPONSE_LENGTH*sizeof(bool));

    for(int b = 0;b<RESPONSE_LENGTH;b++){
        response[b] = 0;
    }
    gpio_set_direction(_dataPin,GPIO_MODE_OUTPUT);
    gpio_set_level(_dataPin,1);
    //Send Start signal
    gpio_set_level(_dataPin,0);
    //Hold for 20 ms
    ets_delay_us(20 * 1000);
    //Pull up
    gpio_set_level(_dataPin,1);
    ets_delay_us(40);
    gpio_set_direction(_dataPin,GPIO_MODE_INPUT);

    for(int b = 0;b < RESPONSE_LENGTH; b++){
        ets_delay_us(1);
        response[b] = gpio_get_level(_dataPin);
    }

    //interpret result
    bool state = response[0];
    int cicles = 1;
    bool results[40];
    for(int b = 0; b<40;b++){
        results[b] = 0b0;
    }
    int bit = 0;
    for(int rb = 1; rb < RESPONSE_LENGTH; rb++){
        if(response[rb] == state){
            cicles++;
        } else  {
            if (state == 1){
            if(cicles < 59) {
                if(cicles < 20){
                    results[bit] = 0;
                }
                else {
                    results[bit] = 1;
                }
                bit++;
                if(bit >= 40){break;}
                }
            }
            state = response[rb];
            cicles = 1;
        }
    }

   
   
    int result_bytes[5] = {0,0,0,0,0};

    for(int bit = 0; bit<40;bit++){
        result_bytes[bit/8] = result_bytes[bit/8] | (results[bit] << (7-(bit %8)));
    }
    uint8_t checksum = result_bytes[0] + result_bytes[1] + result_bytes[2] + result_bytes[3];

    if(checksum != result_bytes[4]){
        *RH = -1;
        *T = -1;
        return;
    }

    float temp_int = result_bytes[2];
    int temp_dec = result_bytes[3];
    float temp = temp_int + ((float)(temp_dec%10))/10;

    *RH = (float)result_bytes[0];
    *T = temp;

    free(response);
    return;
}

void DHT11::DHT11(gpio_num_t pin){
    _dataPin = pin

    gpio_config_t config_data;
    config_data.intr_type = GPIO_INTR_DISABLE;
    config_data.mode = GPIO_MODE_OUTPUT;
    config_data.pin_bit_mask = 1 << _dataPin;
    config_data.pull_up_en = GPIO_PULLUP_DISABLE;
    config_data.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&config_data);


    gpio_set_direction(_dataPin,GPIO_MODE_OUTPUT);
    gpio_set_level(_dataPin,1);//Colocar pino no alto
}