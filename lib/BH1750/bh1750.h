#include <avr/io.h>


#define BH1750_WRITE_ADDR       0x46
#define BH1750_READ_ADDR        0x47



////////////////////////////////////////////////////////
// I2C COMMANDS

#define BH1750_POWER_DOWN          0x00
#define BH1750_POWER_ON            0x01
#define BH1750_RESET               0x07

//HIGH RES measrung time = 120ms
#define BH1750_CONT_H_RES          0x10
#define BH1750_ONE_TIME_H_RES      0x20
// LOW RES mearuing time = 16ms
#define BH1750_ONE_TIME_L_RES      0x23
////////////////////////////////////////////////////////


uint8_t bh1750_init(void);

void bh1750_power (uint8_t);

void bh1750_toggle_oneread_h(void);
uint16_t bh1750_oneread_h_no_delay(void);

// a single read in high res mode, takes about 125ms
uint16_t bh1750_oneread_h(void);

// a single read in low res mode, takes about 20ms
uint16_t bh1750_oneread_l(void);

//reset the sensor
void bh1750_reset(void);

//sets the sensor in continous high res mode
void bh1750_set_cont_h();
// reads the last measurement in continous mode
uint16_t bh1750_contread_h(void);