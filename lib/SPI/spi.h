/*  Simple SPI Driver - Kevin

SS must be handled by application!


*/

/*  Simple SPI Driver - Kevin

SS must be handled by application!


*/

#ifndef SPI
#define SPI
///////////////////////////////////////////////////////////////////////////////
// IO Pin Config
#define MISO_DDR DDRB
#define MOSI_DDR DDRB
#define SCK_DDR DDRB

#define MOSI_PORT PORTB
#define MISO_PORT PORTB
#define SCK_PORT PORTB

#define MOSI_PIN PB3
#define MISO_PIN PB4
#define SCK_PIN PB5

#define SS_PIN      2
#define SS_DDR     DDRB
#define SS_PORT    PORTB

/*
    * Enum for SPI Speed selection
    * 0 = fosc/4
    * 1 = fosc/16
    * 2 = fosc/64
    * 3 = fosc/128
*/
typedef enum  { SPI_SPEED_0 = 0, SPI_SPEED_1, SPI_SPEED_2, SPI_SPEED_3} eSpiPrescaler;


/* 
    * inits the master and ports
    * wants a init parameter fromt he eSpiPrescaler enum
uint8_t spi_init(eSPI_Prescaler);
*/
uint8_t spi_init(eSpiPrescaler speed);



/*
    * begin and end functions -> set the standard SS Pin high or low (define in spi.h)
*/
void spi_beginTrx(void);

void spi_endTrx(void);

/*
    * trades a single byte on the SPI bus. Return the recevied byte
*/
uint8_t spi_trade_byte(uint8_t data);

/*
    * trades a 16bit word on the SPI bus, Returns the received data
*/
uint16_t spi_trade_word(uint16_t data);


typedef enum { CPOL_LEADING = 0, CPOL_FALLIN} eSpiClockPolarity;
typedef enum { CPHA_LEADING = 0, CPHA_FALLIN} eSpiClockPhase;
/*
    * set clock polarity and sampling phase 
    * according to datasheet
*/
void spi_set_clock(eSpiClockPolarity, eSpiClockPhase);

void spi_close(void);

#endif 

