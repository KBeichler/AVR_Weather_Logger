#include <avr/io.h>
#include <i2c.h>


static void _wait_for_trx(void);



uint8_t i2c_init(void){

    static uint8_t i2c_init_finished = 0;
    if (i2c_init_finished != 0) { return 1; }

    /* SCL Frequency = F_CPU / (16 + 2* TWBR * Prescaler[TWSR low 2 bits]) */
    /* 16M / 16 + 2 * 72 * 1 ~= 100kHz */

    TWSR &= ~(_BV(TWPS0) | _BV(TWPS1)) ; // 00 = prescaler 1

    TWBR = I2C_BIT_RATE;
    TWCR |=  _BV(TWEN); //TWEN activates the I2C Interface


    return ++i2c_init_finished;
}

void i2c_close(void){
    TWCR &=  ~(_BV(TWEN)); //TWEN activates the I2C Interfac

}

static void _wait_for_trx(void){
    while (!(TWCR &(_BV(TWINT))));
}


void i2c_start(void){
    // TWSTA sets start condition
    //TWINT activates the transacton
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) ;
    _wait_for_trx();
}

void i2c_stop(void){
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN) ;
    //wait for STOP to finish
    while ( !(TWCR & (1 << TWSTO)));
    
}

void i2c_write(uint8_t data){
    TWDR = data;
    TWCR = _BV(TWINT) | _BV(TWEN) ;
    _wait_for_trx();
}

uint8_t i2c_read_ack(void){
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) ;
    _wait_for_trx();
    return TWDR;
}

uint8_t i2c_read_noack(void){
    TWCR = (_BV(TWINT) | _BV(TWEN) );
    _wait_for_trx();
    return TWDR;
}

uint8_t i2c_status(void){
    _wait_for_trx();
    return (TWSR & 0xF8);
}

    