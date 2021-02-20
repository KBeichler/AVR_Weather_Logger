#ifndef UART
#define UART

#define MAXSIZE 128





uint8_t uart_init(uint32_t baudrate);

/*
    * Check if data is available in the uart rx buffer
*/
uint8_t uart_available(void);

/*
    * flush usart rx buffer
*/
void uart_flush(void);


/*
    * transmits a String -> Finish on NULL Terminator.
    * Stops on MAXSIZE (define in uart.h)
*/
void uart_writestring(char * data) ;

/*
    * Write string function which utilices pgm (string is saved in flash memory -> needs to be known at compile time)
    * USAGE uart_writestring_p(PSTR("hello world\n"));
*/
void uart_writestring_p(const char * data) ;

/*
    * Reads a string from thew uart
    * terminates on newline '\n
    * adds the newline to the buffer string and NULL terminates the string
*/
void uart_readstring(char * buffer) ;


void uart_close();











#endif 
