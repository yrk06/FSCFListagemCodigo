#include "driver/gpio.h"


#define RESPONSE_LENGTH 4600
#define SHORT_LENGTH 10
#define LONG_LENGTH 20

const char names[5] = { "RH_INT", "RH_DECIMAL", "TEMP_INT", "TEMP_DECIMAL", "CHECKSUM" };

class DHT11 {
    gpio_num_t _dataPin;
    float lastReading[2];

    public:
        DHT11(gpio_num_t pin);
        void readData(float &outTemp, float &outRH);
}