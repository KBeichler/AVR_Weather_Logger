// BME280 - I2C Register Maps, commands, etc.



#define VALID_ID_1      0x58
#define VALID_ID_2      0x60


#define REG_ID          0xD0
#define REG_RESET       0xE0
#define REG_CTRL_HUM    0xF2
#define REG_STATUS      0xF3
#define REG_CTRL_MEAS   0xF4
#define REG_CONFIG      0xF5
#define REG_PRESS_MSB   0xF7
#define REG_PRESS_LSB   0xF8
#define REG_PRESS_XSB   0xF9
#define REG_TEMP_MSB    0xFA
#define REG_TEMP_LSB    0xFB
#define REG_TEMP_XSB    0xFC
#define REG_HUM_MSB     0xFD
#define REG_HUM_LSB     0xFE

#define REG_CALDATA     0x88
#define REG_CALDATA_H   0xE1
#define CALDATA_LENGTH_TP  26
#define CALDATA_LENGTH_H    7



// COMMANDS

#define CMD_MODE_FORCE  0x01
#define CMD_MODE_SLEEP  0x00
#define CMD_MODE_NORMAL 0x02
#define CMD_RESET       0xB6

#define DEFAULT_CONFIG      0x00
#define DEFAULT_CTRL_MEAS   0x24
#define DEFAULT_CTRL_HUM    0x01