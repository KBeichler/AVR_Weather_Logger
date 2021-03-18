/*  VEML 6070 - UV Sensor Lib
    * Simple Lib for the VEML 6070 UV Light Sensor
    * lib was tested with the adafruit breaktout board feturing and 270k SET Resistor!
    * check the datasheet for more information
    * 
*/

#include <avr/io.h>
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  COMMANDS
#define WRITE_ADDR          0x70
#define READ_LSB_ADDR       0x71
#define READ_MSB_ADDR       0x73
#define READ_ARA_ADDR       0x18

// BITMASKS
#define SD_BIT          0
#define IT_BIT          2
#define ACK_THD_BIT     4
#define ACK             5

typedef enum VEML_POWER{
    ON = 0,
    OFF
} VEML_POWER;


void veml6070_init(uint8_t sensitivity);



void veml6070_power(VEML_POWER);


uint16_t veml6070_read_uv(void);
/*
off 

on 

read 


set_it 

set_ack 


*/