#include <avr/io.h>
#include <util/delay.h>

#include "bme280.h"
#include "bmeREG.h"
#include <i2c.h>





BME280_CalibParam calib_param;


uint8_t bme280_init(BME280_dev *dev){

    i2c_init();

    uint8_t retrycnt = 5;
    uint8_t *reg;

    while (retrycnt--){
        bme280_read(REG_ID, 1,  reg);
        if ( *reg == VALID_ID_1 || *reg == VALID_ID_2) {
            break;
        }
        if (retrycnt == 0) return 0;
    }
    
    dev->id = *reg;

    bme280_reset();
    _delay_ms(2);
    bme280_get_params(dev);

    // Write standard config as seen in bmeREG.h
    bme280_write(REG_CONFIG, DEFAULT_CONFIG);
    bme280_write(REG_CTRL_HUM, DEFAULT_CTRL_HUM);
    bme280_write(REG_CTRL_MEAS, DEFAULT_CTRL_MEAS |  CMD_MODE_SLEEP);

    //get actual config and save it in the device struct
    bme280_get_config(dev);
    return 1;
}


void bme280_read(uint8_t reg_addr, uint8_t size, uint8_t * buffer){
    
    i2c_start();
    i2c_write(BME_WRITE_ADDR);
    i2c_write(reg_addr);

    i2c_start();
    i2c_write(BME_READ_ADDR);

    while(size--){
        if (size == 0)  { *buffer = i2c_read_noack(); }
        else            { *buffer  = i2c_read_ack(); }
        buffer++;
    }
    i2c_stop();

}


void bme280_write(uint8_t reg_addr, uint8_t  buffer){
    i2c_start();
    i2c_write(BME_WRITE_ADDR);
    i2c_write(reg_addr);
    i2c_write(buffer);
    i2c_stop();
}




/*
    * Communication functions
*/

void bme280_get_data_manual(BME280_dev * dev){
    uint8_t buffer[8];

    int32_t uncomp_press, uncomp_temp;
    uint32_t uncomp_hum;

    bme280_read(REG_PRESS_MSB, 8, buffer);

    uncomp_press = (int32_t) ((((uint32_t) (buffer[0])) << 12) | (((uint32_t) (buffer[1])) << 4) | ((uint32_t) buffer[2] >> 4));
    uncomp_temp = (int32_t) ((((int32_t) (buffer[3])) << 12) | (((int32_t) (buffer[4])) << 4) | (((int32_t) (buffer[5])) >> 4));
    uncomp_hum = (uint32_t) buffer[6] << 8 | (uint32_t) buffer[7];
    
    dev->temperature = bme280_compensate_temp(dev, uncomp_temp);
    dev->pressure = bme280_compensate_press(dev, uncomp_press);
    dev->humidity = bme280_compensate_hum(dev, uncomp_hum);  

}


void bme280_get_data(BME280_dev * dev){
    uint8_t buffer[8];

    bme280_force_meas(dev);
    _delay_ms(MEAS_TIME);

    int32_t uncomp_press, uncomp_temp;
    uint32_t uncomp_hum;

    bme280_read(REG_PRESS_MSB, 8, buffer);

    uncomp_press = (int32_t) ((((uint32_t) (buffer[0])) << 12) | (((uint32_t) (buffer[1])) << 4) | ((uint32_t) buffer[2] >> 4));
    uncomp_temp = (int32_t) ((((int32_t) (buffer[3])) << 12) | (((int32_t) (buffer[4])) << 4) | (((int32_t) (buffer[5])) >> 4));
    uncomp_hum = (uint32_t) buffer[6] << 8 | (uint32_t) buffer[7];
    
    dev->temperature = bme280_compensate_temp(dev, uncomp_temp);
    dev->pressure = bme280_compensate_press(dev, uncomp_press);
    dev->humidity = bme280_compensate_hum(dev, uncomp_hum);   

}



void bme280_get_params(BME280_dev *dev){
    uint8_t reg[CALDATA_LENGTH_TP ];
    int16_t dig_h4_lsb;
    int16_t dig_h4_msb;
    int16_t dig_h5_lsb;
    int16_t dig_h5_msb;

    bme280_read(REG_CALDATA,CALDATA_LENGTH_TP , reg);

    dev->calib_param.dig_t1 = (uint16_t) (((uint16_t) reg[1] << 8) | ((uint16_t) reg[0]));
    dev->calib_param.dig_t2 = (int16_t)   (((int16_t) reg[3] << 8) | ((int16_t)  reg[2]));
    dev->calib_param.dig_t3 = (int16_t)   (((int16_t) reg[5] << 8) | ((int16_t)  reg[4]));
    dev->calib_param.dig_p1 = (uint16_t) (((uint16_t) reg[7] << 8) | ((uint16_t) reg[6]));
    dev->calib_param.dig_p2 = (int16_t)   (((int16_t) reg[9] << 8) | ((int16_t)  reg[8]));
    dev->calib_param.dig_p3 = (int16_t)  (((int16_t) reg[11] << 8) | ((int16_t) reg[10]));
    dev->calib_param.dig_p4 = (int16_t)  (((int16_t) reg[13] << 8) | ((int16_t) reg[12]));
    dev->calib_param.dig_p5 = (int16_t)  (((int16_t) reg[15] << 8) | ((int16_t) reg[14]));
    dev->calib_param.dig_p6 = (int16_t)  (((int16_t) reg[17] << 8) | ((int16_t) reg[16]));
    dev->calib_param.dig_p7 = (int16_t)  (((int16_t) reg[19] << 8) | ((int16_t) reg[18]));
    dev->calib_param.dig_p8 = (int16_t)  (((int16_t) reg[21] << 8) | ((int16_t) reg[20]));
    dev->calib_param.dig_p9 = (int16_t)  (((int16_t) reg[23] << 8) | ((int16_t) reg[22]));
    
    dev->calib_param.dig_h1 = reg[25];

    bme280_read(REG_CALDATA_H, CALDATA_LENGTH_H , reg);

    dev->calib_param.dig_h2 = (int16_t)(((uint16_t) reg[1] << 8) | ((uint16_t) reg[0]));
    dev->calib_param.dig_h3 = reg[2];

    dig_h4_msb = (int16_t)(int8_t)reg[3] * 16;
    dig_h4_lsb = (int16_t)(reg[4] & 0x0F);
    dev->calib_param.dig_h4 = dig_h4_msb | dig_h4_lsb;

    dig_h5_msb = (int16_t)(int8_t)reg[5] * 16;
    dig_h5_lsb = (int16_t)(reg[4] >> 4);
    dev->calib_param.dig_h5 = dig_h5_msb | dig_h5_lsb;

    dev->calib_param.dig_h6 = (int8_t)reg[6];  
}

void bme280_get_config(BME280_dev * dev){
    uint8_t reg[4];
    // read out control registers
    
    bme280_read(REG_CTRL_HUM, 4,  reg);
    dev->reg_ctrl_hum = reg[0];
    dev->reg_status = reg[1];
    dev->reg_ctrl_meas = reg[2];
    dev->reg_config = reg[3];
}


// MISC FUNCTIONS

//Start a Single, Forced measurement. wait MEAS_TIME to finish measurement (5ms)
void bme280_force_meas(BME280_dev * dev){
    bme280_write(REG_CTRL_MEAS, ( dev->reg_ctrl_meas | CMD_MODE_FORCE ));   

}

void bme280_reset(){
    bme280_write(REG_RESET, CMD_RESET);
}




/* 
Temeprature and Preasure conversion functions.
Taken from the datasheet and the BOSH Sensor API
https://github.com/BoschSensortec/BME280_driver

Takes the ADC data read from the sensor and returns an
    int32 for temp
    uint32 for preasure
*/
int32_t bme280_compensate_temp(BME280_dev * dev, int32_t uncomp_temp){
    //code from BOSH API
    int32_t var1, var2, T;    
    var1 = ((((uncomp_temp >> 3 ) - ((int32_t) dev->calib_param.dig_t1 << 1))) * ((int32_t) dev->calib_param.dig_t2)) >> 11;
    var2 =(((((uncomp_temp >> 4 ) - ((int32_t) dev->calib_param.dig_t1)) * ((uncomp_temp >> 4) - ((int32_t) dev->calib_param.dig_t1))) >> 12 ) * ((int32_t) dev->calib_param.dig_t3)) >> 14;
    dev->calib_param.t_fine = var1 + var2;
    T = (dev->calib_param.t_fine * 5 + 128) / 256;
    return T;

}


uint32_t bme280_compensate_press(BME280_dev * dev, int32_t uncomp_pres ){
    //code from BOSH API
    int32_t var1, var2;
    uint32_t  P;

    var1 = (((int32_t) dev->calib_param.t_fine) >> 1) - (int32_t) 64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t) dev->calib_param.dig_p6);
    var2 = var2 + ((var1 * ((int32_t) dev->calib_param.dig_p5)) << 1);
    var2 = (var2 >> 2) + (((int32_t) dev->calib_param.dig_p4) << 16);
    var1 = (((dev->calib_param.dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + 
                ((((int32_t) dev->calib_param.dig_p2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t) dev->calib_param.dig_p1)) >> 15);

    P = (uint32_t)(((int32_t)(1048576 - uncomp_pres) - (var2 >> 12)) * 3125);

    /* Avoid exception caused by division with zero */
    if (var1 != 0)
    {   
        /* Check for overflows against UINT32_MAX/2; if pres is left-shifted by 1 */
        if (P < 0x80000000)
        {
            P = (P << 1) / ((uint32_t) var1);
        }
        else
        {
            P = (P / (uint32_t) var1) * 2;
        }


        var1 = (((int32_t) dev->calib_param.dig_p9) * ((int32_t) (((P >> 3) * (P >>3)) >> 13))) >> 12;
        var2 = (((int32_t) (P >> 2)) * ((int32_t) dev->calib_param.dig_p8)) >> 13;
        P = (uint32_t) ((int32_t) P + ((var1 + var2 + dev->calib_param.dig_p7) >> 4));

    }
    else
    {
        P = 0;        
    }

    return P;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
uint32_t bme280_compensate_hum(BME280_dev * dev, uint32_t uncomp_hum){
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;
    uint32_t humidity_max = 102400;

    var1 = dev->calib_param.t_fine - ((int32_t)76800);
    var2 = (int32_t)(uncomp_hum * 16384);
    var3 = (int32_t)(((int32_t)dev->calib_param.dig_h4) * 1048576);
    var4 = ((int32_t)dev->calib_param.dig_h5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)dev->calib_param.dig_h6)) / 1024;
    var3 = (var1 * ((int32_t)dev->calib_param.dig_h3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)dev->calib_param.dig_h2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)dev->calib_param.dig_h1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);

    if (humidity > humidity_max)
    {
        humidity = humidity_max;
    }

    return humidity;
}