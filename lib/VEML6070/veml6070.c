#include <veml6070.h>
#include <i2c.h>
#include <avr/io.h>
#include <util/delay.h>


#include <logger.h>

uint8_t reg = 0x02;

static uint8_t read_msb();
static uint8_t read_lsb();
static void write_byte(uint8_t data);


void veml6070_init(uint8_t sensitivity){

    i2c_init();
    if (sensitivity > 4){
        sensitivity = 3;
    }
    uint8_t ara = veml_reset_irq(); 
   

    reg |= (sensitivity << IT_BIT);

    write_byte(reg);

}

// power = 1 for power on
void veml6070_power(VEML_POWER power){
    veml_reset_irq();
    if (power != 0){
        reg |= (1 << SD_BIT);
    }
    else{
        reg &= ~(1 << SD_BIT);
    }

    write_byte(reg);

    i2c_stop();
}


uint16_t veml6070_read_uv(void){
    uint16_t data;
    uint8_t lsb, msb;
/*
    msb = read_msb();
    _delay_us(100);
    lsb = read_lsb();*/
    i2c_stop();

    i2c_start();
    i2c_write(READ_MSB_ADDR);
    
    msb = i2c_read_ack();
    i2c_stop();
    _delay_us(50);

    i2c_start();    
    i2c_write(READ_LSB_ADDR);
    lsb = i2c_read_ack();;

    i2c_stop();
 


    data = (msb << 8) | lsb;
    return data;
}










static uint8_t read_msb(){
    i2c_start();
    i2c_write(READ_MSB_ADDR);
    
    uint8_t msb = i2c_read_ack();

    i2c_stop();
 

    return msb;
}

static uint8_t read_lsb(){
    i2c_start();    
    i2c_write(READ_LSB_ADDR);
    uint8_t lsb = i2c_read_ack();;

    i2c_stop();
 
    return lsb;
}

static void write_byte(uint8_t data){
    i2c_start();

    i2c_write(WRITE_ADDR);

    i2c_write(data);

    i2c_stop();
    
    


}
uint8_t veml_reset_irq(){
    i2c_start();
    i2c_write(READ_ARA_ADDR  | 1);
    uint8_t tmp = i2c_read_ack();
    i2c_stop();

    return 0;
    
}