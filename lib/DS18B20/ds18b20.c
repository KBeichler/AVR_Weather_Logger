#include "ds18b20.h"
#include <avr/io.h>
#include <util/delay.h>





// Global variables for search function
static uint8_t lastDeviceF;
static uint8_t lastDisc;
static uint8_t lastFDisc;
static uint8_t rom_buffer[8];
// search algorithm function declaration
static uint8_t _search_rom(void);




// init function
uint8_t ds18b20_init(DS18B20_dev *dev){
    
    //setup the IO port for OneWire
    PORT_SET_INPUT;
    PORT_LOW;

    dev->resolution = RESOLUTION;

    //search all devices. saved into rom_codes array
    //returns number of found devies
    dev->sensor_count = ds18b20_search_sensors(dev);


    //set measurement resolution to all sensors
    for ( int i = 0; i < dev->sensor_count ; i++){
        ds18b20_set_resolution(&dev->sensor[i]);
    }
    return 1;
}


// Onewire pulse functions

int8_t _ow_reset(){

    //pull port low for > 450us
    PORT_SET_OUTPUT;
    PORT_LOW;
    _delay_us(500);
    PORT_HIGH;
    PORT_SET_INPUT;
    _delay_us(REST_TIME);

    //read port for 5x sampling time to be sure that there are devices on the bus
    uint8_t c = 0;
    for (int i = 0; i < 500 / SAMPLE_TIME; i++){
        c += !(PIND & _BV(2));
        _delay_us(SAMPLE_TIME);
    }

    //if succesfull (slaved pulled bus low for > 50us) return 1
    if ( c > 5) {
        return 1;
    }   
    
    return 0;
    

}

void _ow_write_bit(uint8_t bit){

    
    PORT_SET_OUTPUT;
    PORT_LOW;

    if (bit){
        _delay_us(5);
        PORT_HIGH;
    }

    else{
        _delay_us(5);
    }

    _delay_us(TIME_W);

    PORT_HIGH;
    _delay_us(REST_TIME);
    PORT_SET_INPUT;

}

int8_t _ow_read_bit(){

    uint8_t c = 0;
    PORT_SET_OUTPUT;
    PORT_LOW;
    _delay_us(5);
    PORT_HIGH;
    PORT_SET_INPUT;
    _delay_us(15);

    //double invert c, so its either 0 or 1 
    c = !!(PIND & _BV(2));
    

    _delay_us(50);

    return c;

}




// byte functions
void ds18b20_write_byte(uint8_t cmd){


    PORT_SET_OUTPUT;

    for (int i = 0; i < 8; i++){
        //write corrosponding bit
        _ow_write_bit( cmd & _BV(i) );

    }

}

uint8_t ds18b20_read_byte(){
    uint8_t r = 0 ;
    for (int i = 0; i < 8 ; i++){

        r |= (_ow_read_bit() << i);

    }    

    return r;
}


void _match_rom(DS18B20_sensor *sensor){
    //sends a MATCH_ROM command followed by the 64bit rom code 
    
    _ow_reset();    
    ds18b20_write_byte(MATCH_ROM);
    for (int i = 0; i < 8; i++){

        ds18b20_write_byte(sensor->rom_code[i]);

    }

}



// Public functions
void ds18b20_set_resolution( DS18B20_sensor *sensor){
        uint8_t res = RESOLUTION;
        _match_rom(sensor);

        ds18b20_write_byte(WRT_SCRPAD);

        
        sensor->registers[2] = 0x1F | (res << 5);
        ds18b20_write_byte(sensor->registers[0]);
        ds18b20_write_byte(sensor->registers[1]);
        ds18b20_write_byte(sensor->registers[2]);
        
        

}


void ds18b20_get_temp( uint8_t idx, DS18B20_dev *dev){
    //get temparature reading from single slave

    ds18b20_start_temp_conv(&dev->sensor[idx]);
    _wait_conv_delay();

    uint8_t byte[2];
    _match_rom(&dev->sensor[idx]);
    ds18b20_write_byte(RD_SCRPAD);

    for (int i = 0; i < 2; i++){
        byte[i] = ds18b20_read_byte();        
    }

    dev->sensor[idx].temperature = _conv_temp(byte);

}

void ds18b20_get_temp_manual(DS18B20_sensor *sensor){
    //get temparature reading from single slave

    uint8_t byte[2];
    _match_rom(sensor);
    ds18b20_write_byte(RD_SCRPAD);

    for (int i = 0; i < 2; i++){
        byte[i] = ds18b20_read_byte();        
    }

    sensor->temperature = _conv_temp(byte);

}

int16_t _conv_temp(uint8_t *raw){

    //converts read temperature into 2 variables
    // 23.45 -> uu.ll
    int16_t upper = 0;
    uint8_t lower = 0;

    uint16_t result;

    upper = (raw[1] & 0x7)<<4;
    upper |= (raw[0] & 0xF0) >> 4;


    //get bit 3-0 of LSB. check if one, if yes -> add 2^-i to lower digit
    for (int i = 0; i <= RESOLUTION ; i++){
        lower += (raw[0] & _BV(3 - i)) > 0 ? 100 >> (i+1) : 0;
    }

    while ( lower > 100){
        lower /= 10;
    }

    

    //if temperature is negative, negate upper byte and make it negative
    if (raw[1] & 0xF0){
        upper = 0x1F & (~upper);
        result = (upper*100) + lower;
        result |= _BV(15);
    }

    else{
        result = (upper*100) + lower;
    }

    return result;

}


void ds18b20_start_temp_conv(DS18B20_sensor *sensor){

    _match_rom(sensor);
    ds18b20_write_byte(CONVERT_T);

}

void _wait_conv_delay(){
    _delay_ms(CONV_DELAY);
}

void _read_scratchpad(DS18B20_sensor *sensor){
    uint8_t buffer[9];

    _match_rom(sensor);

    ds18b20_write_byte(RD_SCRPAD);

    for (uint8_t i = 0; i < 9; i++){
        buffer[i] = ds18b20_read_byte();
        
    }

    sensor->registers[0] = buffer[2];
    sensor->registers[1] = buffer[3];
    sensor->registers[2] = buffer[4];
}

/*  Search function. Returns nuber of found devices and saves ROM codes into rom_code array
    Implementation details see 
    https://www.maximintegrated.com/en/design/technical-documents/app-notes/1/187.html
*/

uint8_t ds18b20_search_sensors(DS18B20_dev *dev){
    lastDeviceF = 0;
    uint8_t c = 0;

    //primary search loop
    //loops until _searchROM does not find new devices
    //saves rom code into rom_codes array
    while( _search_rom() ){
    
        for (int i = 0; i < 8 ; i++){            
            dev->sensor[c].rom_code[i] = rom_buffer[i];;
        }

        _read_scratchpad(&dev->sensor[c]);

        c++;
    }
    
    return c;

}




static uint8_t _search_rom(){

    uint8_t cmp_bit, id_bit;
    uint8_t bit_pos, byte_pos;
    uint8_t last_zero;
    uint8_t bit_mask = 0;
    uint8_t srch_dir;

    if (!_ow_reset() || lastDeviceF){     
        return 0;
    }    

    id_bit = 1;
    bit_pos = 0;
    byte_pos = 0;
    last_zero = 0,

    ds18b20_write_byte(SRCH_ROM);   

    do{
        id_bit = _ow_read_bit();
        cmp_bit = _ow_read_bit();
        
        if ((id_bit == 1) && (cmp_bit == 1)){            
            break;
        }

        if (id_bit != cmp_bit){
            srch_dir = id_bit;
        }

        else{

            //get search direction
            if (bit_pos ==  lastDisc){
                srch_dir = 1;
            }

            else{

                if (bit_pos > lastDisc){
                    srch_dir = 0;
                }

                else{ //CHECK!!!
                    srch_dir = rom_buffer[byte_pos] & _BV(bit_mask);
                }

            }

            if (srch_dir == 0){                
                last_zero = bit_pos;

                if (last_zero < 9){
                    lastFDisc = last_zero;
                }
            }

        }
        
        if (srch_dir){
            rom_buffer[byte_pos] |= _BV(bit_mask);
            _ow_write_bit(1);
        }

        else{
            rom_buffer[byte_pos] &= ~(_BV(bit_mask));
            _ow_write_bit(0);
        }

        bit_pos++;
        bit_mask++;

        if (bit_mask > 7){
            byte_pos++;
            bit_mask = 0;
        }

        
    }while(byte_pos < 8);
    
    lastDisc = last_zero;

    if (lastDisc == 0){
        lastDeviceF = 1;
    }

    return 1;
    
}