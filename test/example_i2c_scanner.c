
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include <logger.h>
#include <i2c.h>

#define STATUSLED 5

int main(void)
{
    // helper variables
    uint8_t status;
    char message[64];

    // Statusled on PortB5
    DDRB |= _BV(STATUSLED);
    PORTB &= ~(_BV(STATUSLED));


    //init logger to redirect stdio to uart
    logger_init(9600);
    printf("\n********** K - I2C Scanner **********\n**********  PROGRAMM START **********\n");
        

    _delay_ms(100);
    while (1)
    {

        printf("\n********** Sart a new Scan now **********\n\n");
        PORTB ^= 1 << 5;

        // loop over all possible i2c addresses
        for (uint8_t i = 1; i <= 127; i++){

                uint8_t read_addr, write_addr;
                read_addr = ((i << 1) | 1);
                write_addr = (i<< 1);

                //start i2c and check for response on WRITE address
                i2c_init();
                i2c_start();
                i2c_write( read_addr );
                status = i2c_status();

                if (status == 0x40){
                    printf("Answer for READ on Address 0x%X \n", read_addr);
                }                
                i2c_stop();            
                i2c_close();

                //start i2c and check for response on READ address
                i2c_init();
                i2c_start();
                i2c_write( write_addr );
                status = i2c_status();

                if (status == 0x18){
                    printf("Answer for WRITE on Address 0x%X \n", write_addr);                    
                }                
                i2c_stop();
                i2c_close();
                
        }
        printf("\n**********   Scan finished   **********\n");
        PORTB ^= 1 << 5;    

        _delay_ms(10000);
       

    }

    return 0;
}



