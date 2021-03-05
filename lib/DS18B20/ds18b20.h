/*  1 Wire Protocol implementation fpr DS18B20 Sensor

Simple implementation of the OneWire protocol for DS18B20 Sesnors from MaxinIntegrated

Timing of the protocoll is controlled via the AVS _delay_us function
Correct setting clockspeed is therefore important

Driving the Sensor in parasite mode should be spossible, but its not tested!


The following define statements set up the Port for the OneWire Bus
#define WIRE_PIN    2
#define WIRE_PORT   PORTD
#define WIRE_DDR    DDRD
                
*/
#include <avr/io.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
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


// Define OneWire Port and functions
#define WIRE_PIN    2
#define WIRE_PORT   PORTD
#define WIRE_DDR    DDRD

#define PORT_SET_INPUT WIRE_DDR &= ~(_BV(WIRE_PIN))
#define PORT_SET_OUTPUT WIRE_DDR |= (_BV(WIRE_PIN))

#define PORT_HIGH WIRE_PORT |= _BV(WIRE_PIN)
#define PORT_LOW  WIRE_PORT &= ~(_BV(WIRE_PIN))


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Device and Sensor struct


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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// INIT FUNCTION
/**
 * Searches all sensors on the OneWire bus and initilises them (set resolution)
 * Sets the count of found devices in the device struct
 * 
 *
 * @param: pointer to device struct 
 * 
 * @return: 1
 * 
*/
uint8_t ds18b20_init(DS18B20_dev *dev);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// LOW LEVEL ONWIRE FUNCTIONS
/**
 * Exectues a reset pulse (about 500us high + 500us low) 
 * 
 * 
 * @return: if a sensor sends a lifebit return 1
*/
int8_t _ow_rest();

/**
 * Write 1 bit on the bus
 * 
 * @param: bit to write (0 or 1)
 * 
 * */
void _ow_write_bit(uint8_t);

/**
 * read one bit from the bus
 * 
 * @return: returns the read bit
 * */
int8_t _ow_read_bit();


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HIGH LEVEL ONWIRE FUNCTIONS
/**
 * Writes one byte to the bus
 * 
 * @param: byte to write
 * 
 * */
void    ds18b20_write_byte(uint8_t);

/**
 * read one byte from the bus
 * 
 * @return: returns the rad byte
 * */
uint8_t ds18b20_read_byte();

/**
 * send a match ROM command to the given sensor
 * with match ROM we select a device on the bus
 * 
 * @param: pointer to sensor struct! (member for sensor array in device struct)
 * 
 * */
void _match_rom(DS18B20_sensor *sensor);

/**
 * INTERNAL: read the internal ROM from the sensor
 * 
 * @param: pointer to sensor struct! (member for sensor array in device struct)
 *
 *  */
void _read_scratchpad(DS18B20_sensor *sensor);




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// DS18B20  SENSOR FUNCTIONS

/**
 * Performs the rom search algorithm. Saves all found sensor into sensor array in device struct
 * @param: pointer to device struct
 * 
 * @return: returns the count of found devices
 * 
 * */
uint8_t ds18b20_search_sensors(DS18B20_dev *dev);

/**
 * sets the reading resolution for given sensor
 * RESOLTION IS FIXED IN DEFINE STATEMENT IN HEADER FILE
 * 
 * @param: pointer to sensor struct! (member for sensor array in device struct)
 * 
 * */
void ds18b20_set_resolution(DS18B20_sensor *sensor);

/**
 * reads temperature of given sensor, tremperature is saved into sensor struct
 * this function waits for the temperature conversion of the sensor 
 * takes 94 - 750ms! depending on resolution
 * 
 * @param: pointer to sensor struct! (member for sensor array in device struct)
 * 
 * */
void ds18b20_get_temp( DS18B20_sensor *sensor);

/**
 * reads temperature of given sensor, tremperature is saved into sensor struct
 * this function does not wait for the temperature conversion of the sensor 
 * if conversionis not finished yet, data may be corrupted
 * 
 * @param: pointer to sensor struct! (member for sensor array in device struct)
 * 
 * */
void ds18b20_get_temp_manual(DS18B20_sensor *sensor);

/**
 * starts a temperature conversion on given sensor
 * after conversion is finsihed, data can be read from the sensor (ds18b20_get_temp_manual function)
 * takes 94 - 750ms! depending on resolution
 * @param: pointer to sensor struct! (member for sensor array in device struct)
 * 
 * */
void ds18b20_start_temp_conv(DS18B20_sensor *sensor);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// MISC FUNCTION

/**
 * INTERNAL: Converts the raw data into a readable format
 * 
 * @param: raw read data
 * 
 * @return: int16_t conversed temeprature
 * 
 * */
int16_t _conv_temp(uint8_t *raw);

/**
 * INTERNAL: delay for conversion
 * is calculated on compile time!
 * 
 * */
void _wait_conv_delay();








