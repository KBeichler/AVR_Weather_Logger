#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h> 

#include "main.h"

#include <i2c.h>
#include <spi.h>
#include <oled.h>
#include <adc.h>
#include <bh1750.h>
#include <ds18b20.h>
#include <bme280.h>
#include <nrf24.h>

#ifdef DEBUG
#include <stdio.h>
#include <logger.h>
#endif

const char version[] = "a_0.1";


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
station_data data_struct;

BME280_dev bme280;
DS18B20_dev ds18b20;    

// NRF SETTINGS
NRF24_dev nrf;
uint8_t tx_address[] = "xxxx1";
uint8_t rx_address[] = "xxxx2";

// COUNTER VARIABLES FOR INTERRUPTS
volatile uint32_t milliseconds = 0;
volatile uint32_t wdt_counter = 0;




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS

// TIMER FUNCTIONS TO CORRECTLY TIME SENSOR READINGS
static void station_start_timer();
static void station_stop_timer();

static void station_start_timer(){
  milliseconds = 0;
  TCNT0 = 0;
  TIMSK0 |= _BV(OCIE0A);
}

static void station_stop_timer(){
  TIMSK0 &= ~(_BV(OCIE0A));
}


// ADDC OF FUNCTIONS FOR POWER SAVING
static void station_adc_on();
static void station_adc_off();

static void station_adc_on(){
  ADCSRA |= _BV(ADEN);
  PORTB &= ~(_BV(0));
}

static void station_adc_off(){
  ADCSRA &= ~(_BV(ADEN));
  PORTB |= _BV(0);
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// MAIN LOOP

int main(void)
{
    uint8_t status = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// INITIALISATION 

// AVR TIMER AND WATCHDOG
//Setup Timer1
    // interup based - counts in 1 ms steps
    // compare agains COM0A
    TCCR0A |= _BV(COM0A0);
    //set prescaler to 64
    TCCR0B =  _BV(1) | _BV(0);
    //set comapare register to 250
    OCR0A = 250;
    //enable interrupt OCIE0A
    TIMSK0 |= _BV(OCIE0A);


//Setup the watchdog
    cli();
    MCUSR &= ~(_BV(WDRF));
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    // timer not final
    WDTCSR = 0x60; //0x61
    sei();

// Set Sleep mode
    set_sleep_mode(SLEEP_MODE_STANDBY);

// AVR Peripherals
    i2c_init();
    spi_init(SPI_SPEED_1);

// Setup devices and librarys
    ds18b20_init(&ds18b20);
    bme280_init(&bme280);

    nrf_init(&nrf);    
    nrf_open_reading_channel(&nrf, rx_address, 32, 1);
    nrf_open_writing_channel(&nrf, tx_address, 32);
    nrf_radio_mode(&nrf, RX);
    nrf_flush_rx();
    nrf_radio_mode(&nrf, TX);
    nrf_flush_tx();
    // OPTIONAL SETTINGS - SET IF NEEDED
//  nrf_set_channel(uint8_t channel);
//  nrf_set_datarate(uint8_t datarate);
//  nrf_set_rf_power(uint8_t rf_power);
//  nrf_set_lna_gain(uint8_t lna_gain);
//  nrf_set_crc(uint8_t crc);

    nrf_power_mode(&nrf, POWER_DOWN);

    #ifdef DEBUG
    logger_init(9600);
    printf_P(PSTR("********************\nWEATHER STATION STARTUP V: %s\n"), version);
    nrf_print_debug(&nrf);
    #endif

    _delay_ms(100);


///////////////////////////////////////////////////////////////////////////////
//           ____  _   _ ____  _____ ____  _     ___   ___  ____             //
//          / ___|| | | |  _ \| ____|  _ \| |   / _ \ / _ \|  _ \            //
//          \___ \| | | | |_) |  _| | |_) | |  | | | | | | | |_) |           //
//           ___) | |_| |  __/| |___|  _ <| |__| |_| | |_| |  __/            //
//          |____/ \___/|_|   |_____|_| \_\_____\___/ \___/|_|               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

    while (1)
    {


// WATCHDOG HANDLING
// Watchdog wakes up every (8) seconds, incremets the wdt_counter and checks the counter agains SLEEP_TIME
// on SLEEPT_TIME - 1 the stations activates all sensors and starts the tem,perature conversion (Takes some time on different sensors) 

        if (wdt_counter == (SLEEP_TIME -1)) {
            // start all conversions
        }
        if (wdt_counter <= SLEEP_TIME) {
            sleep_mode();
            continue;
        }


// MEASUREMENT AND SEND ROUTINE
// gets executed when the wdt_counter is bigger than SLEEP_TIME
// the stations reads all needed data and transmitts it to the server
// after finished routine the system goes onto sleep mode

        printf("BEGIN LOOP\n");
        bme280_get_data(&bme280);
        printf("TEST %ld %ld %lu\n", bme280.temperature, bme280.pressure, bme280.humidity);
        printf("DS18B20 test: reg1 0x%02X reg2 0x%02X reg3 0x%02X tem %d\n" ,ds18b20.sensor[0].registers[0], ds18b20.sensor[0].registers[1],ds18b20.sensor[0].registers[2], ds18b20.sensor[0].temperature) ;
        printf("conf delay %d\n", CONV_DELAY);
        ds18b20_get_temp(0, &ds18b20);
        status = nrf_write_payload(&nrf,tx_address,5);
        printf("send was ? %d\n", status);
        printf("sizeof %d", sizeof(data_struct));
        _delay_ms(5000);













        wdt_counter = 0;  
       

    }

    return 0;
}


















///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// INTERRUPT ROUTINES


ISR (TIMER0_COMPA_vect){
  milliseconds++;
}

ISR(WDT_vect){
  wdt_counter++;
}

ISR(__vector_default){};


