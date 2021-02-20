#include <avr/io.h>
#include <spi.h>



uint8_t spi_init(eSpiPrescaler speed)
{
    static uint8_t spi_init_finished = 0;
    if ( spi_init_finished != 0 ){
        return 1;
    }


    // Set MOSI and SCK output,
    MOSI_DDR |= _BV(MOSI_PIN) ;
    SCK_DDR |= _BV(SCK_PIN) ; 

    // activate MISO pullup
    MISO_DDR &= ~(_BV(MISO_PIN));
    MISO_PORT |= _BV(MISO_PIN) ; 

    // set SS pin as output
    SS_DDR |= _BV(SS_PIN);
    SS_PORT |= _BV(SS_PIN);
    
    // Enable SPI, Master, set clock rate fck/16 
    SPCR = _BV(SPE) | _BV(MSTR) | (speed << SPR0);

    return ++spi_init_finished;
}



void spi_beginTrx(void){
    SS_PORT &= ~(_BV(SS_PIN));
}

void spi_endTrx(void){
    SS_PORT |= _BV(SS_PIN);
}

uint8_t spi_trade_byte(uint8_t data)
{
    // write data in SPDR register to start transimsion
    SPDR = data;
    // Wait for transmission complete     
    while(!(SPSR & (_BV(SPIF)))) ;

    return SPDR;
}

uint16_t spi_trade_word(uint16_t data){
    uint16_t trade; 
    
    spi_trade_byte((uint8_t) (data >> 8));
    trade = SPDR;
    trade = (trade << 8);
    spi_trade_byte((uint8_t) (data & 0x00FF));
    trade += SPDR;

    return trade;
}

void spi_set_clock(eSpiClockPolarity pol, eSpiClockPhase phase){

    SPCR &= 0xF3;
    if (pol)    { SPCR |= _BV(CPOL);};
    if (phase)  { SPCR |= _BV(CPHA);};


}


void spi_close(){

    SPCR &= ~(_BV(SPE));

}