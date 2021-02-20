#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h> 



#include "nrf24.h"
#include "nrf24_reg.h"
#include <spi.h>

#define DEBUG



#ifdef DEBUG
#include <logger.h>
#include <stdio.h>
#endif




uint8_t nrf_init(NRF24_dev *dev){
    spi_init(SPI_SPEED_1);

    
    // *SET DEFAULT CONFIG
    

    _nrf_set_add_length();

    nrf_set_channel (DEFAULT_CHANNEL);
    nrf_set_datarate(DEFAULT_DATARATE);
    nrf_set_rf_power(DEFAULT_RF_PWR);
    nrf_set_crc(DEFAULT_CRC);
    nrf_set_lna_gain(DEFAULT_LNA);



    
    // *Init pins
    CE_DDR |= _BV(CE_PIN);
    CE_LOW;
    

    // optional - set clock polarity and sample timing for SPI
    //SPCR |= _BV(CPHA);
    //SPCR |= _BV(CPOL);

    nrf_activate_features();
    nrf_flush_rx();
    nrf_flush_tx();

    nrf_power_mode(dev, STANDBY);

    nrf_store_registers(dev); 

    return 1;
}



uint8_t nrf_read_register(uint8_t regaddr, uint8_t * data){

    spi_beginTrx();
    uint8_t status =  spi_trade_byte(R_REGISTER | regaddr) ; 
    *data = spi_trade_byte(NOP);
    spi_endTrx();
    return status;

}

uint8_t nrf_write_register(uint8_t regaddr, uint8_t data){
    spi_beginTrx();
    uint8_t status =  spi_trade_byte(W_REGISTER | regaddr) ; 
    spi_trade_byte(data);
    spi_endTrx();
    return status;

}

uint8_t nrf_read_multi_register(uint8_t regaddr, uint8_t *data, uint8_t length){

    spi_beginTrx();
    uint8_t status =  spi_trade_byte(R_REGISTER | regaddr) ; 
    while (length--){
        *data = spi_trade_byte(NOP);
        data++;
    }
    
    spi_endTrx();
    return status;

}

uint8_t nrf_write_multi_register(uint8_t regaddr, uint8_t *data,  uint8_t length){
    
    spi_beginTrx();
    uint8_t status =  spi_trade_byte(W_REGISTER | regaddr) ; 
    while (length--){
        spi_trade_byte(*data);
        data++;
    }    
    spi_endTrx();
    return status;

}


eERROR nrf_read_payload(NRF24_dev *dev, uint8_t * buffer, uint8_t len){


    spi_beginTrx();

    uint8_t status = spi_trade_byte(R_RX_PAYLOAD);
    //uint8_t r;
    while (len--){

        *buffer = spi_trade_byte(NOP);
        buffer++;

    }

    spi_endTrx();
    

    return status;

}


eERROR nrf_write_payload(NRF24_dev *dev, uint8_t * buffer, uint8_t len){

    spi_beginTrx();

    uint8_t blanks = dev->tx_payload_length - len;
    spi_trade_byte(W_TX_PAYLOAD);
    //uint8_t r;
    while (len--){
        spi_trade_byte(*buffer);
        buffer++;
    }

    while(blanks--){
        spi_trade_byte(0);
    }

    spi_endTrx();

    spi_beginTrx();
    
    uint8_t status = spi_trade_byte(NOP);
    spi_endTrx();

    if ( status & _BV(MAXIMUM_RT_IRQ) ){        
        nrf_reset_irq(status, MAXIMUM_RT_IRQ);
        return MAXIMUM_RT_IRQ;
    }

    return NO_ERROR;
}



eERROR nrf_write_payload_noack(NRF24_dev *dev, uint8_t * buffer, uint8_t len){

    spi_beginTrx();

    uint8_t blanks = dev->tx_payload_length - len;
    spi_trade_byte(W_TX_PAYLOAD_NO_ACK);
    //uint8_t r;
    while (len--){
        spi_trade_byte(*buffer);
        buffer++;
    }

    while(blanks--){
        spi_trade_byte(0);
    }

    spi_endTrx();

    return NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// STATE FUNCTIONS

void nrf_power_mode(NRF24_dev *dev, ePOWER_MODE mode){
    uint8_t reg;
    nrf_read_register(CONFIG, &reg);
    
    switch(mode){
        case(POWER_DOWN):
            reg &= ~(1 << PWR_UP);
            CE_LOW;
            break;

        case(ACTIVE):
            reg |= (1<<PWR_UP);
            CE_HIGH;
            break;

        default:
        case(STANDBY):
            reg |= (1<<PWR_UP);
            CE_LOW;
            break;  
        }

    nrf_write_register(CONFIG, reg ); 
    dev->pwr_mode = mode;
    
}

void nrf_radio_mode(NRF24_dev *dev, eRADIO_MODE mode){

    uint8_t reg;
    nrf_read_register(CONFIG, &reg);

    if (mode == RX){
        reg |= (1 << PRIM_RX) ;        
    }
    else if (mode ==TX){
        reg &= ~(1 << PRIM_RX);    
    }   
        
    nrf_write_register(CONFIG, reg);
    dev->radio_mode = mode;
    if (!(CE_STATE)){
        CE_HIGH;
    }
    
}

void nrf_open_reading_channel(NRF24_dev *dev, uint8_t *address, uint8_t idx, uint8_t rx_pload_length){
    dev->rx_payload_length[idx] = rx_pload_length;

    uint8_t reg;
    nrf_read_register(EN_RXADDR, &reg);
    nrf_write_register(EN_RXADDR, (reg | _BV(idx)));
    nrf_set_payload_length(idx, rx_pload_length);

    //nrf_write_multi_register( (RX_ADDR_P0+idx), address, ADDR_LENGTH + 2);
    spi_beginTrx();
    spi_trade_byte( (W_REGISTER | (RX_ADDR_P0 + idx) ));

    for ( uint8_t i = 0; i < (ADDR_LENGTH); i++){
        spi_trade_byte(address[i]);
        dev->rx_pipe[idx][i] = address[i];
    }
    spi_endTrx();

}

void nrf_open_writing_channel(NRF24_dev *dev, uint8_t *address, uint8_t tx_pload_length){
    dev->tx_payload_length = tx_pload_length;

    //nrf_write_multi_register( TX_ADDR , address, ADDR_LENGTH + 2);
    
    spi_beginTrx();
    spi_trade_byte( (W_REGISTER | (TX_ADDR) ));
    
    for (uint8_t i = 0; i < (ADDR_LENGTH); i++){
        spi_trade_byte(address[i]);
        dev->tx_pipe[i] = address[i];
    }
    spi_endTrx();
    nrf_open_reading_channel(dev, address, 0, tx_pload_length);


}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// CONFIG FUNCTIONS

void nrf_set_channel(uint8_t channel){   
    nrf_write_register(RF_CH, channel);
}

void nrf_set_datarate(uint8_t datarate){
    uint8_t reg;
    nrf_read_register(RF_SETUP, &reg);
    // Clear bit and set it
    reg = (reg & ~(_BV(RF_DR))) | (datarate << RF_DR);
    nrf_write_register(RF_SETUP, reg);
}

void nrf_set_rf_power(uint8_t rf_power){
    uint8_t reg;
    nrf_read_register(RF_SETUP, &reg);
    // Clear bit and set it
    reg = (reg & ~(_BV(RF_PWR) | _BV(RF_PWR + 1))) | (rf_power << RF_PWR);
    nrf_write_register(RF_SETUP, reg);
}

void nrf_set_lna_gain(uint8_t lna_gain){
    uint8_t reg;
    nrf_read_register(RF_SETUP, &reg);
    // Clear bit and set it
    reg = ((reg & ~(_BV(LNA_HCURR))) | (lna_gain << LNA_HCURR));
    nrf_write_register(RF_SETUP, reg);
}

void nrf_set_crc(uint8_t crc){
    uint8_t reg;
    nrf_read_register(CONFIG, &reg);
    // Clear bit and set it
    reg = ((reg & ~(_BV(CRCO))) | (crc << CRCO));
    nrf_write_register(CONFIG, reg);
}

void nrf_set_payload_length(uint8_t idx, uint8_t length){                    
    nrf_write_register( ((RX_PW_P0)+idx ) , length);         
}

void _nrf_set_add_length(void){
    nrf_write_register(SETUP_AW, (ADDR_LENGTH -2));
}




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// MISC FUNCTIONS

uint8_t nrf_flush_tx(){
    spi_beginTrx();
    uint8_t status = spi_trade_byte(FLUSH_TX);
    spi_endTrx();
    return status;
    
}



uint8_t nrf_flush_rx(){
    spi_beginTrx();
    uint8_t status = spi_trade_byte(FLUSH_RX);

    spi_endTrx();
    return status;
}



uint8_t nrf_activate_features(void){
    spi_beginTrx();
    uint8_t status = spi_trade_byte(ACTIVATE);
    spi_trade_byte(0x73);
    spi_endTrx();
    return status;
}



uint8_t nrf_available(NRF24_dev *dev){
    uint8_t reg;
    nrf_read_register(FIFO_STATUS, &reg);


    if (dev->radio_mode == RX){
        if ( !(reg & (1 <<RX_EMPTY) ) ){
            return 1;
        }


    }

    else if (dev->radio_mode == TX){
        if ( !(reg & (1 << TX_FULL) ) ){
            return 1;
        }

    }

    return 0;

}


void nrf_reset_irq(uint8_t status, eERROR irq){
    status |= _BV(irq);
    nrf_write_register(STATUS, status);
}

void nrf_store_registers(NRF24_dev *dev){

    for (uint8_t i = 0; i < REGMAP_END; i++){

        dev->registers[STATUS] = nrf_read_register(i, &dev->registers[i]);

    }

}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS

#ifdef DEBUG

void nrf_print_all(NRF24_dev *dev){
    nrf_store_registers(dev);
    printf_P(PSTR("DEBUG: SHOW ALL REGISTERS OF THE NRF24 ON STARTUP\n"));
    for (uint8_t i = 0; i < REGMAP_END; i++){
        printf_P(PSTR("************************\n")); 
        printf_P(PSTR("REGISTER 0x%02X \n"), i);  
        printf_P(PSTR("DATA: 0x%02X \n"), dev->registers[i]);


    }


}


void nrf_print_debug(NRF24_dev *dev){

    nrf_store_registers(dev);
    uint8_t address[ADDR_LENGTH + 1];
    printf_P(PSTR("\nNRF24: DEBUG OUTPUT REGISTERS\n"));
    printf_P(PSTR("STATUS = 0x%02X\n"),dev->registers[STATUS]);

    nrf_read_multi_register(RX_ADDR_P0, address, 5);
    address[5] = 0;
    printf_P(PSTR("RX_ADDR_PIPE0 = %s \n"), address);;   

    nrf_read_multi_register(RX_ADDR_P1, address, 5);
    address[5] = 0;
    printf_P(PSTR("RX_ADDR_PIPE1 = %s \n"), address);

    nrf_read_multi_register(TX_ADDR, address, 5);
    address[5] = 0;
    printf_P(PSTR("TX_ADDR_PIPE = %s \n"), address);

    printf_P(PSTR("RX_PW_PW = 0x%02X 0x%02X \n"),dev->registers[RX_PW_P0],dev->registers[RX_PW_P1]);
    printf_P(PSTR("CONFIG = 0x%02X\n"),dev->registers[CONFIG]);
    printf_P(PSTR("EN_AA = 0x%02X\n"), dev->registers[EN_AA]);
    printf_P(PSTR("EN_RX_ADDR = 0x%02X\n"), dev->registers[EN_RXADDR]);
    printf_P(PSTR("ADDR_WIDTH = 0x%02X\t\t01 - 3b | 10 - 4b | 11 - 5b\n"),  dev->registers[SETUP_AW]);
    printf_P(PSTR("RF_CHANNEL = 0x%02X\n"), dev->registers[RF_CH]);
    printf_P(PSTR("RF_SETUP = 0x%02X\n"), dev->registers[RF_SETUP]);
    printf_P(PSTR("FEATURE = 0x%02X\n"), dev->registers[FEATURE]);
    printf_P(PSTR("DYNPD = 0x%02X\n"), dev->registers[DYNPD]);

    printf_P(PSTR("DEBUG OUTPUT REGISTERS END\n\n"));

}

#endif


