/* kNRF24   Register Map 
            Command Map
            Bit Masks
https://www.mouser.com/datasheet/2/297/nRF24L01_Product_Specification_v2_0-9199.pdf

*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// DEFAULt CONFIG
#define DEFAULT_USED_PIPES  2
#define DEFAULT_CHANNEL     0x0f
#define DEFAULT_DATARATE    0
#define DEFAULT_RF_PWR      3
#define DEFAULT_CRC         1
#define DEFAULT_LNA         1


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// SPI COMMANDS

// Read/Write 000AAAAA - 5 Bit Register adress - Reads up to 5 bytes
#define     R_REGISTER      0x00 
#define     W_REGISTER      0x20

// Reads/write 1 to 32 bytes to/from FIFO
#define     R_RX_PAYLOAD    0x61
#define     W_TX_PAYLOAD    0xA0

#define     FLUSH_TX        0xE1
#define     FLUSH_RX        0xE2
#define     REUSE_TX_PL     0xE3

//Followed by data 0x73 - Enables Fetures R_RX_PL_WID + W_ACK_PAYLOAD + W_TX_PAYLOAD_NOACK
#define     ACTIVATE        0x50

#define     R_RX_PL_WID     0x60
#define     W_ACK_PAYLOAD   0XA8
#define     W_TX_PAYLOAD_NO_ACK 0xB
#define     NOP             0xFF





///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// REGISTER MAP TABLE

    /* BIT MASKS
    MASK_RX_DR      6
    MASK_TX_DS      5
    MASK_MAX_RT     4
    EN_CRC          3
    CRCO            2
    PWR_UP          1
    PRIM_RW         0
    */
   /*
#define     CONFIG          0x00
#define     EN_AA           0x01    // Enhanced Shockburst
#define     EN_RXADDR       0x02
#define     SETUP_AW        0x03    // Setup address width
#define     SETUP_RETR      0x04    // Setup of Automatic Retransmission
#define     RF_CH           0x05
#define     RF_SETUP        0x06
#define     STATUS          0x07
#define     OBSERVE_TX      0x08
#define     CD              0x09    // Carrier detect page74
#define     RX_ADDR_P0      0x0A
#define     RX_ADDR_P1      0x0B
#define     RX_ADDR_P2      0x0C
#define     RX_ADDR_P3      0x0D
#define     RX_ADDR_P4      0x0E
#define     RX_ADDR_P5      0x0F
#define     TX_ADDR         0x10
#define     RX_PW_P0        0x11
#define     RX_PW_P1        0x12
#define     RX_PW_P2        0x13
#define     RX_PW_P3        0x14    
#define     RX_PW_P4        0x15
#define     RX_PW_P5        0x16
#define     FIFO_STATUS     0x17
#define     DYNPD           0x1C
#define     FEATURE         0x1D
*/

typedef enum eREGISTER_MAP {
    CONFIG          = 0x00,     
    EN_AA,      
    EN_RXADDR,  
    SETUP_AW,   
    SETUP_RETR, 
    RF_CH,      
    RF_SETUP,   
    STATUS,     
    OBSERVE_TX, 
    CD,         
    RX_ADDR_P0, 
    RX_ADDR_P1, 
    RX_ADDR_P2, 
    RX_ADDR_P3, 
    RX_ADDR_P4, 
    RX_ADDR_P5, 
    TX_ADDR,    
    RX_PW_P0,   
    RX_PW_P1,   
    RX_PW_P2,   
    RX_PW_P3,   
    RX_PW_P4,   
    RX_PW_P5,   
    FIFO_STATUS,
    DYNPD       = 0x1C,      
    FEATURE     = 0x1D,
    REGMAP_END   
        
}eREGISTER_MAP;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// BIT MASKS

#define     MASK_RX_DR      6
#define     MASK_TX_DS      5
#define     MASK_MAX_RT     4
#define     EN_CRC          3
#define     CRCO            2
#define     PWR_UP          1
#define     PRIM_RX         0

#define     ENAA_P5         5
#define     ENAA_P4         4
#define     ENAA_P3         3
#define     ENAA_P2         2   
#define     ENAA_P1         1
#define     ENAA_P0         0
#define     ERX_P5          5
#define     ERX_P4          4
#define     ERX_P3          3
#define     ERX_P2          2   
#define     ERX_P1          1
#define     ERX_P0          0
#define     AW              0   // 2 Bits
#define     ARD             4   // 4 Bits
#define     ARC             0   // 4 Bits

#define     PLL_LOCK        4
#define     RF_DR           3   
#define     RF_PWR          1   // 2 Bits
#define     LNA_HCURR       0
#define     RX_DR           6
#define     TX_DS           5
#define     MAX_RT          4
#define     RX_P_NO         1   // 3 Bits
#define     STATUS_TX_FULL  0
#define     PLOS_CNT        4   // 4 Bits
#define     ARC_CNT         0   // 4 Bits
#define     TX_REUSE        6
#define     TX_FULL         5
#define     TX_EMPTY        4
#define     RX_FULLL        1
#define     RX_EMPTY        0
#define     DPL_P5          5
#define     DPL_P4          4
#define     DPL_P3          3
#define     DPL_P2          2
#define     DPL_P1          1
#define     DPL_P0          0
#define     EN_DPL          2
#define     EN_ACK_PAY      1
#define     EN_DYN_ACK      0

