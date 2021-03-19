#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h> 

#include "station.h"

#include <i2c.h>
#include <spi.h>
#include <oled.h>
#include <adc.h>
#include <bh1750.h>
#include <ds18b20.h>
#include <bme280.h>
#include <nrf24.h>
#include <veml6070.h>

#ifdef DEBUG
#include <stdio.h>
#include <logger.h>
#endif


const char version[] = "a_0.2";


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// GLOBAL DEVICES
station_data data_struct;
BME280_dev bme280;
DS18B20_dev ds18b20;
NRF24_dev nrf;

// NRF SETTINGS
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
  adc_enable();
  ADC_SWITCH_PORT &= ~(_BV(ADC_SWITCH_PIN));
}

static void station_adc_off(){
  adc_disable();
  ADC_SWITCH_PORT |= _BV(ADC_SWITCH_PIN);
}


// MEASUREMENT WRAPPER FUNCTIONS
void station_start_measurement();

void station_start_measurement(){

  bme280_force_meas(&bme280);

  bh1750_toggle_oneread_h();
  veml6070_power(ON);
  i2c_restart();
  

  for (uint8_t i = 0; i < ds18b20.sensor_count; i++){
    ds18b20_start_temp_conv(&ds18b20.sensor[i]);
  }

}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// MAIN LOOP

int main(void)
{
    

/*//////////////////////////////////////////////////////////////////////////////
//            ___       _ _   _       _ _           _   _                     //
//           |_ _|_ __ (_) |_(_) __ _| (_)___  __ _| |_(_) ___  _ __          //
//            | || '_ \| | __| |/ _` | | / __|/ _` | __| |/ _ \| '_ \         //
//            | || | | | | |_| | (_| | | \__ \ (_| | |_| | (_) | | | |        //
//           |___|_| |_|_|\__|_|\__,_|_|_|___/\__,_|\__|_|\___/|_| |_|        //
//                                                                            //
//////////////////////////////////////////////////////////////////////////////*/
    #ifdef DEBUG
    logger_init(1200);
    printf_P(PSTR("********************\nWEATHER STATION STARTUP V: %s\n"), version);
    #endif
// Status Variables
  uint8_t nrf_status = 0;
  


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


//Setup the watchdog - Interrupt but no reset
    cli();
    MCUSR &= ~(_BV(WDRF));
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    // timer not final
    // 0x60 = 4 seconds, 0x61 = 8 seconds
    WDTCSR = 0x60; //0x61
    

// Set Sleep mode
    set_sleep_mode(SLEEP_MODE_STANDBY);

// AVR Peripherals
    i2c_init();
    spi_init(SPI_SPEED_1);

    adc_init(AREF_AVCC);
    for (uint8_t i = 0; i < ADC_COUNT; i++){
      adc_init_pin(i);
    }

    ADC_SWITCH_DDR |= _BV(ADC_SWITCH_PIN);
    ADC_SWITCH_PORT |= _BV(ADC_SWITCH_PIN);
    station_adc_off();

// Setup devices and librarys
// include short delay

    ds18b20_init(&ds18b20);
    bme280_init(&bme280);

    //data_struct.adc_count = ADC_COUNT;
    //data_struct.ds18b20_count = ds18b20.sensor_count;

    nrf_init(&nrf);  
     
    nrf_open_reading_channel(&nrf, rx_address, 32, 1);
    nrf_open_writing_channel(&nrf, tx_address, 32);
    nrf_radio_mode(&nrf, RX);
    nrf_flush_rx();
    nrf_radio_mode(&nrf, TX);
    nrf_flush_tx();
    // OPTIONAL SETTINGS - SET IF NEEDED
    nrf_set_channel(0x00);
//  nrf_set_datarate(uint8_t datarate);
//  nrf_set_rf_power(uint8_t rf_power);
//  nrf_set_lna_gain(uint8_t lna_gain);
//  nrf_set_crc(uint8_t crc);
    nrf_power_mode(&nrf, POWER_DOWN);


    veml6070_init(3);
    veml6070_power(OFF);

    #ifdef DEBUG
    
    nrf_print_debug(&nrf);
    printf_P(PSTR("\nINITIALISATION FINISHED - STARTING MAINLOOP\n********************\n"));
    #endif

    
    sei();


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
       // station_start_measurement();
       // _delay_ms(500);



        if (wdt_counter == (SLEEP_TIME -1)) {
          // start all conversions
          station_start_measurement();
          
          
          
        }
        if (wdt_counter <= SLEEP_TIME) {
          sleep_mode();
          continue;
        }


// MEASUREMENT AND SEND ROUTINE
// gets executed when the wdt_counter is bigger than SLEEP_TIME
// the stations reads all needed data and transmitts it to the server
// after finished routine the system goes onto sleep mode
        #ifdef DEBUG
        printf("BEGIN SEND ROUTINE\n");
        #endif
        nrf_power_mode(&nrf, STANDBY);
        station_adc_on();

        
        

        
        bme280_get_data(&bme280);
        data_struct.bme_temp  = bme280.temperature;
        data_struct.bme_press = bme280.pressure;
        data_struct.bme_hum   = bme280.humidity;

        data_struct.bh1750_data = bh1750_oneread_h_no_delay();       

        
        
        for(uint8_t i = 0; i < ds18b20.sensor_count; i++){
          ds18b20_get_temp_manual(&ds18b20.sensor[i]);
          data_struct.ds18b20_data[i] = ds18b20.sensor[i].temperature;
        }
        data_struct.uv_idx = veml6070_read_uv(); 


        

        
        for (uint8_t i = 0; i < ADC_COUNT; i++){
          data_struct.adc_data[i] = adc_read(i);
        }
        station_adc_off();
        veml6070_power(OFF);


        
        nrf_status = nrf_write_payload(&nrf, (uint8_t*) &data_struct, sizeof(data_struct));
        nrf_power_mode(&nrf, ACTIVE);


        
        station_start_timer();
        uint8_t delay = milliseconds;
        while( nrf_fifo_empty(TX) && milliseconds < 5)
        {
          while (milliseconds == delay){};
          delay = milliseconds;
         };
        
        station_stop_timer();

        if (milliseconds >= 5){
          nrf_flush_tx();
          nrf_status = MAXIMUM_RT_IRQ;
        }


        nrf_power_mode(&nrf, POWER_DOWN);

        
                


        #ifdef DEBUG
        printf("BME MEASUREMENTS %ld %ld %lu\n", data_struct.bme_temp, data_struct.bme_press, data_struct.bme_hum);
        printf("READ %d DS18B20 Sensors:\n", ds18b20.sensor_count);
        for(uint8_t i = 0; i < ds18b20.sensor_count; i++){
          printf("DS18B20 Sensor Nr %d Data: %d \n", i, data_struct.ds18b20_data[i] /*ds18b20.sensor[i].temperature */ );          
        }
        printf("ADC READINGS:");


        for (uint8_t i = 0; i < ADC_COUNT; i++){
          printf("  %d : %04d", i, data_struct.adc_data[i]);
        }
        printf("\nBH1750 DATA: %d   - UV DATA: %d\n", data_struct.bh1750_data, data_struct.uv_idx);
        
        printf("\nNRF Send was: %s - CODE %02X Time: %02ld msec\n", (nrf_status)  ? "NOK" : "OK", nrf_status, milliseconds);
        printf("\nHEX DATA: ");

        uint8_t *ptr = &data_struct;
        for (uint8_t i = 0; i < sizeof(data_struct); i++){
          printf("%02X ", *ptr);
          ptr++;
        }

        printf("\nNRF: %d DataBytes %sTransmitted\n\n", sizeof(data_struct), (nrf_status)  ? "not " : "" );
        #endif

        



        
        //_delay_ms(2500);

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


