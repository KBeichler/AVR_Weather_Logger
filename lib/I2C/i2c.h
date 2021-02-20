#ifndef I2C
#define I2C

#ifndef I2C_SPEED
#define I2C_SPEED 100000  /* 100000 or 400000 */
#endif



#define I2C_BIT_RATE (F_CPU/(I2C_SPEED*2) - 8)




// Inits the I2C bus to 100kHz, fixed right now
uint8_t i2c_init(void);

void i2c_close(void);


void i2c_start(void);

void i2c_stop(void);

void i2c_write(uint8_t data);

uint8_t i2c_read_ack(void);

uint8_t i2c_read_noack(void);

// retruns the status code of the TWSR REgsiter -> codes see documentation from page 227
uint8_t i2c_status(void);





















#endif