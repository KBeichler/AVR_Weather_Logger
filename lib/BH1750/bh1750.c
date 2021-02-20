#include <bh1750.h>

#include <avr/io.h>
#include <i2c.h>
#include <util/delay.h>






uint8_t bh1750_init(){
    i2c_init();
    bh1750_power(1);
    uint16_t test = bh1750_oneread_h();
    
    if (test > 0){
        return 1;
    }

    return 0;
}


void bh1750_power (uint8_t mode){
    i2c_start();
    i2c_write(BH1750_WRITE_ADDR);
    if (mode == 0){
        i2c_write(BH1750_POWER_DOWN);
    }
    else{
        i2c_write(BH1750_POWER_ON);
        
    }  

    i2c_stop();
}

void bh1750_toggle_oneread_h(void){
    bh1750_power(1);
    i2c_start();
    i2c_write(BH1750_WRITE_ADDR);
    i2c_write(BH1750_ONE_TIME_H_RES);
    i2c_stop();
}

uint16_t bh1750_oneread_h_no_delay(void){
    uint16_t d;
    i2c_start();
    i2c_write(BH1750_READ_ADDR);
    d = (i2c_read_ack() << 8);
    d += i2c_read_noack();
    i2c_stop();
    return d;


}

uint16_t bh1750_oneread_h(){

    uint16_t d;
    i2c_start();
    i2c_write(BH1750_WRITE_ADDR);
    i2c_write(BH1750_ONE_TIME_H_RES);
    i2c_stop();

    _delay_ms(125);

    i2c_start();
    i2c_write(BH1750_READ_ADDR);
    d = (i2c_read_ack() << 8);
    d += i2c_read_noack();
    i2c_stop();

    return d;

}

uint16_t bh1750_oneread_l(){
        uint16_t d;
    i2c_start();
    i2c_write(BH1750_WRITE_ADDR);
    i2c_write(BH1750_ONE_TIME_L_RES);
    i2c_stop();

    _delay_ms(20);

    i2c_start();
    i2c_write(BH1750_READ_ADDR);
    d = (i2c_read_ack() << 8);
    d += i2c_read_noack();
    i2c_stop();

    return d;
}

void bh1750_reset(){
    i2c_start();
    i2c_write(BH1750_WRITE_ADDR);
    i2c_write(BH1750_RESET);
    i2c_stop();

}

void bh1750_set_cont_h(){
    i2c_start();
    i2c_write(BH1750_WRITE_ADDR);
    i2c_write(BH1750_CONT_H_RES);
    i2c_stop();
}
uint16_t bh1750_contread_h(void){
    uint16_t d;
    i2c_start();
    i2c_write(BH1750_READ_ADDR);
    d = (i2c_read_ack() << 8);
    d += i2c_read_noack();
    i2c_stop();
    return d;

}