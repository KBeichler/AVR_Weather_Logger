#ifndef STATION
#define STATION

//#define DEBUG


#define SLEEP_TIME      37
#define ADC_COUNT       4
#define DS18B20_COUNT   4

#define ADC_SWITCH_PORT PORTB
#define ADC_SWITCH_DDR  DDRB
#define ADC_SWITCH_PIN 0


typedef struct station_data {
    uint16_t adc_data[ADC_COUNT];

    uint16_t ds18b20_data[DS18B20_COUNT];
    int32_t bme_temp;
    uint32_t bme_press;
    uint32_t bme_hum;
    uint16_t bh1750_data;
    uint16_t uv_idx;


}station_data;





#endif