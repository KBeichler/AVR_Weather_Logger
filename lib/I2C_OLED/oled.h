/*    Simple driver for 128*64 OLED Display with SSD1306 Controller - Kevin



USAGE:
    set I2C address  in header file
    startup sequence is in OLED_INIT_COMMANDS array in c file

    in programm - 
        OLED_init(); initialices thje display
        OLED_set_cursor(row,col) - sets the current writign pos in the display
        OLED_send_font(char *) - expects a string and writes it do display
        OLED_clear() - clears everything
        OLED_clear_line(line) - clears this line
*/

#ifndef OLED
#define OLED

#define OLED_READ_ADDR 0x79
#define OLED_WRITE_ADDR 0x78

// Command Definitions  Reference Manual from Page 28

////////////////////////////////////////////////////////
// Fundamentals 
#define OLED_SET_CONTRAST                0x81
// Takes a a second value from 0x00 to 0xFF -> standard 0x7F
#define OLED_DISPLAY_ON                 0xAF
#define OLED_DISPLAY_OFF                0xAE

#define OLED_DISPLAY_GRAM               0xA4
#define OLED_DISPLAY_DEBUG_ON           0xA5

#define OLED_DISPLAY_NORMAL             0xA6
#define OLED_DISPLAY_INVERSE            0xA7

////////////////////////////////////////////////////////
// SCrolling
#define OLED_SCROLLING_DEACTIVATE       0x2E



////////////////////////////////////////////////////////
// Addressing
//
// 00 - 0F Lower Column - lower nibble of addresing byte
// 10 - 1F Upper Column - upper nibble of addressing byte
#define OLED_LOWER_ADDR_NIBBLE          0x00
#define OLED_UPPER_ADDR_NIBBLE          0x10

// memory adressing wants second command as 0x0*
// x is     0 - Horizontal adressen
//          1 - Vertical Adresing
//          2 - Page adressing (reset)
#define  OLED_SET_MEMORY_ADRESSING      0X20  
#define  OLED_SET_MEMORY_ADRESSING_H    0X00    
#define  OLED_SET_MEMORY_ADRESSING_V    0X01    
#define  OLED_SET_MEMORY_ADRESSING_P    0X02      

//set column expects 2 folowing commands
// 0-127 start adress
// 0-127 end adress
#define  OLED_SET_COLUMN_ADDRESS        0x21


//set page expects 2 folowing commands
// 0-7 start adress
// 0-7 end adres
#define  OLED_SET_PAGE_ADRESS           0x22



// start page gets selected with
// 0xB0 - 0xB7
#define OLED_SET_CURRENT_PAGE           0xB0


// SET for the driection of the display
// X Direction Scanning 
#define OLED_SEGREMAP_NORMAL                           0xA0
#define OLED_SEGREMAP_INV                              0xA1 
// X Direction
#define OLED_COMSCAN_INC                               0xC0
#define OLED_COMSCAN_DEC                               0xC8



////////////////////////////////////////////////////////
// Hardware Config

#define OLED_SET_DISPLAY_START          0x40
//0x40 to 0x7F

#define OLED_SET_VERTICAL_OFFSET        0xD3
// second command from 0-63

#define OLED_SET_PINS                   0xDA
// possible  values 0x02, 0x12, 0x022, 0x32
// only 0x12 workd for me

////////////////////////////////////////////////////////
// Timing and so on

// secodn command set from 0b0000 to 0b1111 (default (0b1000)
#define OLED_DISPLAY_CLOCK_DIVIDER      0xD5


////////////////////////////////////////////////////////
// Charge Pump (for lower voltage!)

//second command 0x14 for on 0x10 for off
#define OLED_CHARGE_PUMP                0x8D
#define OLED_CHARGE_ON                  0x14
#define OLED_CHARGE_OFF                 0x10

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
// COMMAND VALUES
#define CMD_SINGLE_COM    0x00
#define CMD_MULTI_COM     0x80
#define CMD_SINGLE_DATA   0x40
#define CMD_MULTI_DATA    0xC0
#define CMD_DATA          0x40

#define CHAR_SIZE 7




////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
// external variable for font
extern const char font[256][6];


struct OLED_PARAMS {
    uint8_t page ;
    uint8_t col ;
    
    };


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

// Function declarations


void OLED_init();

//takes a array with commands and the size of the array and sends it to the display
void OLED_send_cmd(uint8_t *, uint8_t );

void OLED_send_data(uint8_t *  /*, uint8_t*/);

void OLED_send_stream(uint8_t* , uint8_t );

void OLED_clear();
void OLED_clear_line(uint8_t );



// HELPER FUNCTIONS
void OLED_set_cursor(uint8_t , uint8_t );


#endif