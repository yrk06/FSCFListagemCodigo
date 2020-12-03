#include "driver/gpio.h"


//Functions
#define CLEAR_DISPLAY  0b1
#define RETURN_HOME  0b10
#define ENTRY_MODE_SET  0b100
#define DISPLAY_ON_OFF  0b1000
#define CURSOR_DISPLAY_SHIFT  0b10000
#define SET  0b100000
#define SET_CGRAM_ADDRESS  0b1000000
#define SET_DDRAM_ADDRESS  0b10000000

//Parameters
#define INCREMENT  0b10
#define DISPLAY_SHIFT_ON   0b1
#define DISPLAY_ON  0b100
#define CURSOR_ON  0b10
#define BLINK_ON  0b1
#define DISPLAY_SHIFT  0b1000
#define SHIFT_TO_RIGHT  0b100
/* Esses parametros não mudam */
#define DATA_LENGTH_8_BITS  0b10000
#define NUMBER_LINES  0b1000
#define CHARACTER_FONT 0b100

class lcd {

    gpio_num_t _pin_RS;
    gpio_num_t _pin_EN;
    gpio_num_t _dataPins[4]; 

    public:
        lcd(gpio_num_t RS, gpio_num_t EN, gpio_num_t D4, gpio_num_t D5, gpio_num_t D6, gpio_num_t D7);

        void printString(char* pstring);
        void setCursor(int column, int line);
        void printBinaryChar(int character);
        void clearDisplay();
        void setCustomChar(int number,int pattern[8]);
    /* Funções de baixo nivel controladas pela classe*/
    private:
        void init();
        void send4bits(int bits);
        void sendInstruction(int instruction);
        void write(int data);
        void pulseEnable();
    
};