/*      k - NRF24 Lib
    *
    * Simple lib for the NRF24L01 Module connected via SPI
    * developed on an AVR ATMEGA328p (should work on other AVR controllers)
    * 
    * 
    * To use the module, first initialise a NRF24_dev struct and pass a pointer to it to the nrf_init() function.
    * The module is configured with default values found in the nrf24_reg.h file
    * 
    * Reading and writing channels have to be opened with the corrosponding functions (tx and rx address are set here)
    *                       nrf_open_reading_pipe  - nrf_open_writing_pipe
    * 
    * lastly, the power mode has to be set! 3 modes are available 
    *                               POWER_OFF (chip disabled - register reading possible)
    *                               STANDBY (chip enabled)
    *                               ACTIVE (sending and receiving)
    * 
    * WIRING
    * the CE Pin and it corrosponding functions is declared in this file
    * the CSN pin is controlled by the SPI driver
    * 
    * MISO MOSI and SCK see Datasheet
    * 
    * THIS EXAMPLE
    * CE Pin  DDRD7
    * CSN Pin DDRB0                
*/

#include <avr/io.h>


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// CE - PIN SETTINGS
#define CE_PIN       7      //1
#define CE_DDR      DDRD    //DDRB
#define CE_PORT     PORTD   //PORTB



#define CE_HIGH     CE_PORT |= _BV(CE_PIN)
#define CE_LOW      CE_PORT &= ~(_BV(CE_PIN))
#define CE_STATE    CE_PORT & (_BV(CE_PIN))


// How many bytes should be used for the device tx and rx addresses
#define ADDR_LENGTH 5




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ENUMS

// Enum for Error Messages - !TODO
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// DEVICE STRUCT DEFINITION

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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// INIT FUNCTION
/**
* Initialises the Sensor with default values
*
* @param: pointer to device struct 
* 
* @return: enum Error
* 
*/
uint8_t nrf_init(NRF24_dev *dev);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// COMMUNICATION FUNCTIONS

/**
 * write a value to a single register
 * 
 * @param regaddr: Address of the register (nrf24_reg.h)
 * @param data: data which should be written
 * 
 * @return: returns the status register of the sensor
 * */
uint8_t nrf_write_register(uint8_t regaddr, uint8_t  data);

/**
 * read a value from a single register
 * 
 * @param regaddr: Address of the register (nrf24_reg.h)
 * @param data: pointer to a uint8_t which hold the data
 * 
 * @return: returns the status register of the sensor
 * */
uint8_t nrf_read_register(uint8_t regaddr, uint8_t * data);

/**
 * write multiple values to a register (used to set the 3-5 byte address)
 * 
 * @param reagaddr: Address of the register (nrf24_reg.h)
 * @param data: pointer to a uint8_t array
 * @param len: how many bytes should be written
 * 
 * @return: returns the status register of the sensor
 * */
uint8_t nrf_read_multi_register(uint8_t regaddr, uint8_t *data, uint8_t len);

/**
 * read multiple values from a register (used to set the 3-5 byte address)
 * 
 * @param reagaddr: Address of the register (nrf24_reg.h)
 * @param data: pointer to a uint8_t array
 * @param len: how many bytes should be read
 * 
 * @return: returns the status register of the sensor
 * */
uint8_t nrf_write_multi_register(uint8_t regaddr, uint8_t *data,  uint8_t len);

/**
 * read data from the RX FIFO
 * 
 * @param dev: pointer to the device struct
 * @param data: pointer to a uint8_t array which will hold the payload
 * @param len: how many bytes should be read
 * 
 * @return: enum Error
 * */
eERROR nrf_read_payload(NRF24_dev *dev, uint8_t *data, uint8_t len);

/**
 * write a payload into the TX FIFO (this functions excepts a ACK from the receiver)
 * 
 * @param dev: pointer to the device struct
 * @param data: pointer to a uint8_t array which hold the payload
 * @param len: how many bytes should be written
 * 
 * @return: enum Error
 * */
eERROR nrf_write_payload(NRF24_dev *dev, uint8_t *data, uint8_t len);

/**
 * write a payload into the TX FIFO (this functions excepts NO ACK from the receiver)
 * 
 * @param dev: pointer to the device struct
 * @param data: pointer to a uint8_t array which hold the payload
 * @param len: how many bytes should be written
 * 
 * @return: enum Error
 * */
eERROR nrf_write_payload_noack(NRF24_dev *dev, uint8_t *data, uint8_t len);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// STATE FUNCTIONS
/*
 * Set power mode or the module (PWR_UP Register and CE Pint)
 * 
 * @param dev: pointer to the device struct
 * @param mode: mode out of the POWER_MODE enum (POWER_DOWN; STANDY; ACTIVE)
 * 
 * 
 * @return: void
 * */
void nrf_power_mode(NRF24_dev *dev, ePOWER_MODE mode);

/*
 * Set radio mode or the module (TX or RX)
 * 
 * @param dev: pointer to the device struct
 * @param mode: mode out of the eRADIO_MODE enum (TX, Rx)
 * 
 * 
 * @return: void
 * */
void nrf_radio_mode(NRF24_dev *dev, eRADIO_MODE mode);

/*
 * Sets up a writing channel (sets TX address and payload length)
 * 
 * @param dev: pointer to the device struct
 * @param adress: pointer to array holding the address (3-5 bytes)
 * @param tx_pload_length: payload length for the TX FIFO. whe transmitting, tx fifo will be filed with 0 until pload_length is reached
 * 
 * @return: void
 * */
void nrf_open_writing_channel(NRF24_dev *dev, uint8_t *address, uint8_t tx_pload_length);

/*
 * Sets up a reading channel (sets RX address (uo to 5 different) and payload length)
 * ! for Auto ACK the RX pipe 0 needs to be the same address as TX pipe (this function overrides RX Pipe0 settings!)
 * 
 * @param dev: pointer to the device struct
 * @param adress: pointer to array holding the rx address (3-5 bytes)
 * @param tx_pload_length: payload length for the RX FIFO
 * @param idx: index of what reading pipe to use (up to 5)
 * 
 * @return: void
 * */


void nrf_open_reading_channel(NRF24_dev *dev, uint8_t *address, uint8_t rx_pload_length, uint8_t idx);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// MISC FUNCTIONS

/*
 * flushes TX FIFO buffer
 *  
 * @return: void
 * */
uint8_t nrf_flush_tx(void);
/*
 * flushes RX FIFO buffer
 * 
 * @return: void
 * */
uint8_t nrf_flush_rx(void);

/*
 * Command to send the ACTIVATE command  (Datasheet page 46)
 * activates following features: R_RX_PL_WID, W_ACK_PAYLOAD, W_TX_PAYLOAD_NOACK
 * @return: status register
 * */
uint8_t nrf_activate_features(void);

/*
 * Command check if the module is available (depening on the current MODE)
 * RX Mode: checks for data in RX fifo
 * TX Mode: checks if space in TX fifo is free
 * @param dev: pointer to the device struct
 * 
 * @return: 1 if available, 0 if not
 * */
uint8_t nrf_available(NRF24_dev *dev);

/*
 * Command to reset IRQ flag in device register
 * @param status: the last read status register (the register flag should be read there)
 * @param: the IRQ to reset, taken from eERROR enum (it is a bitmask of the status register)
 * 
 * @return: void
 * */
void nrf_reset_irq(uint8_t status, eERROR irq);

/*
 * Reads all registers from the sensor and stores it into the registers array in the device struct
 * @param dev: pointer to the device struct
 * 
 * @return: void
 * */
void nrf_store_registers(NRF24_dev *dev); 

/*
 * Checks if the TX or RX FIFO is empty
 * @param mode: TX or RX
 * 
 * @return: 0 if empty, 1 if data in fifo
 * */
uint8_t nrf_fifo_empty(eRADIO_MODE mode);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// CONFIG FUNCTIONS

/*
 * Function to set the RF channel
 * @param channel: 8bit channel address
 * 
 * @return: void
 * */
void nrf_set_channel(uint8_t channel);

/*
 * Function to set the datarate
 * @param datarate: 0 = 1Mbps, 1 = 2Mbps
 * 
 * @return: void
 * */
void nrf_set_datarate(uint8_t datarate);

/*
 * Function to set the rf_power
 * @param rf_power: 00 = -18dBm, 01 = -12dBm, 10 = -6dBm, 11 = 0dBm
 * 
 * @return: void
 * */
void nrf_set_rf_power(uint8_t rf_power);

/*
 * Function to enable/disable the Low_Noise_Amplifier
 * @param lna_gain: 0 = disable, 1 = enable
 * 
 * @return: void
 * */
void nrf_set_lna_gain(uint8_t lna_gain);

/*
 * Function to sete the CRC mode
 * @param lna_gain: 0 = 8bits, 1 = 16bits
 * 
 * @return: void
 * */
void nrf_set_crc(uint8_t crc);

/*
 * interal function to set the payload length for a pipe
 * @param idx:index number of rx/tx pipe
 * @param length: 0-32 
 * 
 * @return: void
 * */
void _nrf_set_payload_length( uint8_t idx ,uint8_t length);

/*
 * interal function to set the used address length for all pipe
 * @param void: use the ADDR_LENGTH define in nrf24.h file!
 * 
 * @return: void
 * */
void _nrf_set_add_length(void);
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS

/* DEBUG FUNCTION: prints all register to stdout
 * @param dev: pointer to the device struct
 *  
 * @return: void
 * */
void nrf_print_debug(NRF24_dev *dev);

/* DEBUG FUNCTION: prints sensor information to stdout
 * @param dev: pointer to the device struct
 *  
 * @return: void
 * */
void nrf_print_all(NRF24_dev *dev);


