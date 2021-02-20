#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include <logger.h>
#include <i2c.h>
#include <spi.h>
#include <oled.h>
#include <adc.h>
#include <bh1750.h>
#include <ds18b20.h>
#include <bme280.h>
#include <nrf24.h>

#define STATUSLED 5

int main(void)
{
    BME280_dev bme280;
    

    DS18B20_dev ds18b20;

    ds18b20_init(&ds18b20);

    NRF24_dev nrf;
    nrf_init(&nrf);
    uint8_t addr[]= "xxxx1";



    logger_init(9600);
    spi_init(SPI_SPEED_1);
    i2c_init();
    bme280_init(&bme280);
    uint8_t status = 0;
    _delay_ms(100);
    nrf_open_writing_channel(&nrf, addr,32);
    nrf_radio_mode(&nrf, TX);

    nrf_print_debug(&nrf);
    while (1)
    {
        printf("trololo\n");
        bme280_get_data(&bme280);
        printf("Hurenson %ld %ld %lu\n", bme280.temperature, bme280.pressure, bme280.humidity);
        printf("DS18B20 test: reg1 0x%02X reg2 0x%02X reg3 0x%02X tem %d\n" ,ds18b20.sensor[0].registers[0], ds18b20.sensor[0].registers[1],ds18b20.sensor[0].registers[2], ds18b20.sensor[0].temperature) ;
        printf("conf delay %d\n", CONV_DELAY);
        ds18b20_get_temp(0, &ds18b20);
        status = nrf_write_payload(&nrf,addr,5);
        printf("Ssend was ? %d\n", status);
        _delay_ms(5000);
       

    }

    return 0;
}



