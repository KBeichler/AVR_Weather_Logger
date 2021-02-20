#include <avr/io.h>
#include <avr/pgmspace.h>


#include "uart.h"

// LOW LEVEL FUNCTIONS
/*
    * low level transmit function. Takes one byte of data as argument and shifts it out the UART register
*/
static void _transmitByte(uint8_t data);

/*
    * low level function to to receive a single byte. This function is blocking!
*/
static char _receiveByte(void);




uint8_t uart_init(uint32_t baudrate){
    static uint8_t uart_init_finished = 0;
    if (uart_init_finished != 0){
        return 2;
    }

    uint16_t ubrr = F_CPU/16/baudrate;
    
    UBRR0H = (uint8_t )(ubrr>>8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

    return ++uart_init_finished;
}



static void _transmitByte(uint8_t data){
    // wait for transmit buffer to be eumpty
    while ( !( UCSR0A & _BV(UDRE0)) )
        ;

    // load data into transmit buffer
    UDR0 = data;
}


static char _receiveByte(void){


    /* Wait for data to be received */
    while ( !(uart_available()) );

    return UDR0;
}



uint8_t uart_available(void){

    //check for bit RXC0 -> receiving complete
    if ((UCSR0A & (1 <<RXC0))) {
        return 1;
    }

    return 0;
}

void uart_flush(void){
  //read the receive buffer until ints empty
    
    do{
      unsigned char d = UDR0;
    }
    while ( UCSR0A & (1<<RXC0) );
}






void uart_writestring(char * data) {
    uint8_t i = 0;
    while (data[i]) {
    if (i >= MAXSIZE ) {  break;  }
    _transmitByte(data[i]);
    i++;

    }

}


void uart_writestring_p(const char * data) {

    uint8_t i = 0;
    while (pgm_read_byte(data)!= 0x00) {
        if (i >= MAXSIZE ) {   break;  }

    _transmitByte(pgm_read_byte(data));
    data++;
    i++;

    }

}




void uart_readstring(char * buffer) {
    char in;
    uint8_t i =  0;

    while (i < (MAXSIZE - 1)) {                   // prevent over-runs 
        in = _receiveByte();

        if ( in == '\r' || in == '\n' ) {  break;  }  // detect newline, break out of loop
        
        else {
            buffer[i] = in;                    // add in the letter 
            i++;
        }

    }
    buffer[i] = '\n';       //terminate with newline
    buffer[i+1] = 0;        // terminal NULL character 
    uart_flush();
}

void uart_close(){
    UCSR0B &= ~(_BV(TXEN0) | _BV(RXEN0));

}