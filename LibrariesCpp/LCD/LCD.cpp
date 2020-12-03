/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include <string>
#include <cstring>
#include <cmath>
#include "esp32/rom/ets_sys.h"
#include "soc/soc.h"
#include "LCD.h"

lcd::lcd(gpio_num_t RS, gpio_num_t EN, gpio_num_t D4, gpio_num_t D5, gpio_num_t D6, gpio_num_t D7){
    _pin_RS = RS;
    _pin_EN = EN;
    _dataPins[0] = D4;
    _dataPins[1] = D5;
    _dataPins[2] = D6;
    _dataPins[3] = D7;

    gpio_config_t config;
    config.mode = GPIO_MODE_OUTPUT;
    config.intr_type = GPIO_INTR_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pin_bit_mask = (1 << RS) | (1 << EN) | (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7);


    gpio_pad_select_gpio(_pin_RS);
    gpio_pad_select_gpio(_pin_EN);
    gpio_pad_select_gpio(_dataPins[0]);
    gpio_pad_select_gpio(_dataPins[1]);
    gpio_pad_select_gpio(_dataPins[2]);
    gpio_pad_select_gpio(_dataPins[3]);

    gpio_config(&config);

    gpio_set_level(_pin_RS,0);
    gpio_set_level(_pin_EN,0);
    gpio_set_level(_dataPins[0],0);
    gpio_set_level(_dataPins[1],0);
    gpio_set_level(_dataPins[2],0);
    gpio_set_level(_dataPins[3],0);
    init();
}

void lcd::setCursor(int column, int line){
    /*
    A DDRAM tem 80 endereços para colocar caracteres
        * linha 1: 0-39
        * linha 2: 40-79
    */
   if(column > 39){
       // Como o máximo são 40 colunas, não é possivel acessar
       column = 0;
   }
   if(line > 1){
       // Como o máximo são 2 colunas, não é possivel acessar
       line = 0;
   }

   int address = line * 64 + column;

   sendInstruction((1 << 7) | address);
}

void lcd::write(int data){
    gpio_set_level(_pin_RS,1);
    send4bits((data >> 4) & 0xF);
    send4bits((data & 0xF));
}

void lcd::sendInstruction(int instruction){
    gpio_set_level(_pin_RS,0);
    send4bits((instruction >> 4) & 0xF);
    send4bits((instruction & 0xF));
}

void lcd::pulseEnable(){
    gpio_set_level(_pin_EN,0);
    ets_delay_us(1);
    gpio_set_level(_pin_EN,1);
    ets_delay_us(1);
    gpio_set_level(_pin_EN,0);
    ets_delay_us(100);
}

void lcd::send4bits(int bits){

    for(int b = 0; b < 4; b++){
        gpio_set_level(_dataPins[b], (bits >> b) &1);
    }

    pulseEnable();
}

void lcd::init(){
    /* De acordo com o Datasheet, precisamos enviar a função
    set 3x, esperando entre elas e depois fazer a configuração
    para a interface de 4bits */

    /* Inicialização por instrução*/
    ets_delay_us(50 * 1000); 
    gpio_set_level(_pin_RS,0);
    //Primeira instrução
    send4bits(0b0011);
    ets_delay_us(45 * 100);

    send4bits(0b0011);
    ets_delay_us(45 * 100);
    

    send4bits(0b0011);
    ets_delay_us(150);

    //Alternado para modo de 4bits
    send4bits(0b0010);


    /*Instruções de Setup*/

    sendInstruction(0b00101000); //Set 4bit mode; 2 linhas; 5x8 font
    sendInstruction( DISPLAY_ON_OFF | DISPLAY_ON); // Display on \ off
    clearDisplay();
    sendInstruction(ENTRY_MODE_SET | INCREMENT); // Entry mode increment sem shift 
    
}

void lcd::printString(char* pstring){
    int length = strlen(pstring);
    if(length > 16){
        printString("Erro");
        return;
    }
    for(int p = 0; p < length; p++ ){
        write(pstring[p]);
    }
}

void lcd::printBinaryChar(int character){
    write(character);
}

void lcd::clearDisplay(){
    sendInstruction(CLEAR_DISPLAY); // Limpa a tela
    ets_delay_us(1500); //Isso leva um bom tempo
}

void lcd::setCustomChar(int number,int pattern[8]){
    int address = 0x40 + number*8;
    sendInstruction(address);
    for(int byte=0; byte < 8; byte++){
        write(pattern[byte]);
    }
    setCursor(0,0);
}