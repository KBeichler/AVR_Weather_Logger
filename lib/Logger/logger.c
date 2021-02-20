
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>


static int uart_put_stream(char , FILE *);

static FILE uart_out = FDEV_SETUP_STREAM(uart_put_stream, NULL,_FDEV_SETUP_WRITE);

void logger_init(uint32_t baudrate){

    uart_init(baudrate);
    stdout = &uart_out;

}

static int uart_put_stream(char c, FILE *stream){

    while ( !( UCSR0A & _BV(UDRE0)) )
        ;

  UDR0 = c;
  return 0;

}

