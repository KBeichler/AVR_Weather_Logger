/*  1 Wire Protocol implementation fpr DS18B20 Sensor

Simple implementation of the OneWire protocol for DS18B20 Sesnors from MaxinIntegrated

Timing of the protocoll is controlled via the AVS _delay_us function
Correct setting clockspeed is therefore important

Check these define statements. set to corrospndent IO port
#define WIRE_PIN    2
#define WIRE_PORT   PORTD
#define WIRE_DDR    DDRD
                
*/
#include <avr/io.h>

/////////////////////////////////////////////////////////////////
// CONFIGURATION

// Temperature resolution
//value     resolution  convTime
// 0x00       9bit        93.75ms
// 0x01       10bit       187.5ms
// 0x02       11bit       375ms
// 0x03       12bit       750ms    
#define RESOLUTION 0x01
#define CONV_DELAY ((750 >> ( ~(RESOLUTION) & 0x03)) + 50)

#define MAX_SENSOR_COUNT    8

#define WIRE_PIN    2
#define WIRE_PORT   PORTD
#define WIRE_DDR    DDRD

#define PORT_SET_INPUT WIRE_DDR &= ~(_BV(WIRE_PIN))
#define PORT_SET_OUTPUT WIRE_DDR |= (_BV(WIRE_PIN))

#define PORT_HIGH WIRE_PORT |= _BV(WIRE_PIN)
#define PORT_LOW  WIRE_PORT &= ~(_BV(WIRE_PIN))


/////////////////////////////////////////////////////////////////
// OneWire TIMINGS

#define SAMPLE_TIME 10
#define TIME_W 70
#define REST_TIME   5

/////////////////////////////////////////////////////////////////
// COMMAND DEFINITIONS
#define SRCH_ROM        0xF0
#define READ_ROM        0x33
#define MATCH_ROM       0x55
#define SKIP_ROM        0xCC
#define ALARM_SRCH      0xEC

#define CONVERT_T       0x44
#define WRT_SCRPAD      0x4E
#define RD_SCRPAD       0xBE
#define CP_SCRPAD       0x48
#define RECALL_E2       0xB8
#define RD_PWR_SUP      0xB4



typedef struct {
    uint8_t address;
    uint8_t rom_code[8];
    uint8_t registers[3];
    int16_t temperature;
} DS18B20_sensor;

typedef struct {
    uint8_t sensor_count;
    DS18B20_sensor sensor[MAX_SENSOR_COUNT];
    uint8_t resolution;
} DS18B20_dev;


//init function
uint8_t ds18b20_init(DS18B20_dev *dev);


// Onewire pulse functions
int8_t _ow_rest();
void _ow_write_bit(uint8_t);
int8_t _ow_read_bit();

// byte functions
void    ds18b20_write_byte(uint8_t);
uint8_t ds18b20_read_byte();

// send match ROM command to given index.
// all ROM codes are stored in a global array on startup
void _match_rom(DS18B20_sensor *sensor);


// Public functions
//set resolution for temperature measurement

void ds18b20_set_resolution(DS18B20_sensor *sensor);

// read temperature of sensor
// needs the dev struct pointer and the index of the sensor
void ds18b20_get_temp( uint8_t idx, DS18B20_dev *dev);


void ds18b20_get_temp_manual( uint8_t idx, DS18B20_dev *dev);
//misc functions

//converts temprature in readable format
int16_t _conv_temp(uint8_t *raw, DS18B20_dev *dev);

void ds18b20_start_temp_conv(DS18B20_sensor *sensor);
void _wait_conv_delay();

void _read_scratchpad(DS18B20_sensor *sensor);

//performst ROM search. 
//returns number of found devices and saves rom codes in global rom_code array
uint8_t ds18b20_search_sensors(DS18B20_dev *dev);





