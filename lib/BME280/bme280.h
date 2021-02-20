/*  I2C - BME280 Temperature & Humidity Sensor Driver

Simple driver to connect to a BME280 sensor via I2C
supports reading out temp and preasure data

all data is saved in the BME280_config struct, which can be added as global variable to the main file

BME280_init() - initialuces the sensor and sets defautl values
                further config options will be added in the future
                returns a pointer to the global BME280_config struct

BME280_get(ptr to int32 (temp), ptr to uint32 (press))
                function forces a single read, reads the measured data, converts it
                and writes it into the 2 given varaible pointers

        covnert to ascii (var, char_buffer, base)
        ltoa(t, temp, 10);
        ultoa(p, press, 10);

Conncetion INFO
    CSB connected to VCC
    SDO is connected to ground 
        on ground I2C adress is 0xED
*/
#ifndef BME280
#define BME280


#define BME_READ_ADDR 0xED
#define BME_WRITE_ADDR 0xEC


#define MEAS_TIME 10


/*
    * Structure that holds the Calibration Parameters of the BME280 chip
    * see Datasheet or BOSH BME API
*/
typedef struct BME280_CalibParam{
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;

    uint8_t dig_h1;
    int16_t dig_h2;
    uint8_t dig_h3;
    int16_t dig_h4;
    int16_t dig_h5;
    int8_t  dig_h6;

    int32_t t_fine;
} BME280_CalibParam;


/*
    * Device Structure
    * holds information about chip (Id, config registers)
    * also stores the latest temperature, pressure and humidity values
*/
typedef struct BME280_dev {
    uint8_t id;
    // buffer for status registers
    uint8_t reg_status;
    uint8_t reg_ctrl_meas;
    uint8_t reg_ctrl_hum;
    uint8_t reg_config;    

    int32_t temperature;
    uint32_t pressure;
    uint32_t humidity;

    BME280_CalibParam calib_param;      


}BME280_dev;



/*
    * Initialices the empty devices Struct
    * Applies a default config to the sensor
*/
uint8_t bme280_init(BME280_dev * dev);

/*
    * low level read function
*/
void bme280_read(uint8_t reg_addr, uint8_t size, uint8_t * buffer);

/*
    * low level write function
*/
void bme280_write(uint8_t reg_addr, uint8_t  buffer);


// DATA GET FUNCTIONS
/*
    * Forces a Measurement and saves the calculated values into the device struct
    * NOTE: A delay of MEAS_TIME ( = 10) is applied for the measurement
*/
void bme280_get_data(BME280_dev * dev);

/*
    * same as bme280_get_data, but the measurement is not forced!
    * that means, the bme280_force_meas function has to be called beforehand
    * the sensor needs about 10ms for the measurement
*/
void bme280_get_data_manual(BME280_dev * dev);

/*
    * reads out all the calibration parameters and saves it into the device->calib struct
*/
void bme280_get_params(BME280_dev * );

/*
    * gets the current configration registers
*/
void bme280_get_config(BME280_dev * );


// MISC FUCTIONS
/*
    * soft reset
*/
void bme280_reset();

/*
    * Forces a single measurement
*/
void bme280_force_meas(BME280_dev * dev);


/*
    * Compenstation functions
    * takes the raw read data from the sensor!
    * saves result into device struct
    * for further info see BOSH BME API
    *   https://github.com/BoschSensortec/BMP280_driver
*/
int32_t bme280_compensate_temp(BME280_dev * dev, int32_t uncomp_temp);
uint32_t bme280_compensate_press(BME280_dev * dev, int32_t uncomp_pres );
uint32_t bme280_compensate_hum(BME280_dev * dev, uint32_t uncomp_hum);


















#endif






