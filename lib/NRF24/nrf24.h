/*      k - NRF24 Lib

CE Pin  DDRD7
CSN Pin DDRB0
*/
#include <avr/io.h>


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// CE - PIN SETTINGS
#define CE_PIN       7
#define CE_DDR      DDRD
#define CE_PORT     PORTD
#define CE_HIGH     CE_PORT |= _BV(CE_PIN)
#define CE_LOW      CE_PORT &= ~(_BV(CE_PIN))
#define CE_STATE    CE_PORT & (_BV(CE_PIN))



#define ADDR_LENGTH 5


typedef enum eERROR{
    DATA_READY_RX_IRQ = 6,
    DATA_SEND_TX_IRQ = 5,
    MAXIMUM_RT_IRQ = 4,
    NO_ERROR = 0
}eERROR;





typedef enum ePOWER_MODE{
    POWER_DOWN = 0,
    STANDBY,
    ACTIVE
} ePOWER_MODE;

typedef enum eRADIO_MODE{
    TX = 0,
    RX = 1
}eRADIO_MODE;



typedef struct 
{   
    uint8_t registers[32];

    uint8_t channel;
    uint8_t tx_pipe[5];
    uint8_t rx_pipe[6][5];
    
    uint8_t payload_mode;
    uint8_t tx_payload_length;
    uint8_t rx_payload_length[6];      

    ePOWER_MODE pwr_mode ;
    eRADIO_MODE radio_mode ;   

} NRF24_dev;



uint8_t nrf_init(NRF24_dev *dev);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// COMMUNICATION FUNCTIONS

uint8_t nrf_write_register(uint8_t regaddr, uint8_t  data);
uint8_t nrf_read_register(uint8_t regaddr, uint8_t * data);

uint8_t nrf_read_multi_register(uint8_t regaddr, uint8_t * data, uint8_t );
uint8_t nrf_write_multi_register(uint8_t regaddr, uint8_t *data,  uint8_t );


eERROR nrf_read_payload(NRF24_dev *dev, uint8_t * buffer, uint8_t len);

eERROR nrf_write_payload(NRF24_dev *dev, uint8_t * buffer, uint8_t len);

eERROR nrf_write_payload_noack(NRF24_dev *dev, uint8_t * buffer, uint8_t len);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// STATE FUNCTIONS

void nrf_power_mode(NRF24_dev *dev, ePOWER_MODE mode);

void nrf_radio_mode(NRF24_dev *dev, eRADIO_MODE mode);

void nrf_open_writing_channel(NRF24_dev *dev, uint8_t *, uint8_t );

void nrf_open_reading_channel(NRF24_dev *dev, uint8_t *, uint8_t , uint8_t );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// MISC FUNCTIONS
uint8_t nrf_flush_tx(void);
uint8_t nrf_flush_rx(void);

uint8_t nrf_activate_features(void);

uint8_t nrf_available(NRF24_dev *dev);

void nrf_reset_irq(uint8_t status, eERROR irq);

void nrf_store_registers(NRF24_dev *dev); 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// CONFIG FUNCTIONS

void nrf_set_channel(uint8_t channel);
void nrf_set_datarate(uint8_t datarate);
void nrf_set_rf_power(uint8_t rf_power);
void nrf_set_lna_gain(uint8_t lna_gain);
void nrf_set_crc(uint8_t crc);
void _nrf_set_payload_length( uint8_t idx ,uint8_t length);

void _nrf_set_add_length(void);
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS

void nrf_print_debug(NRF24_dev *dev);
void nrf_print_all(NRF24_dev *dev);


