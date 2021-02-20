/*  ADC lib - kevin



*/
//0 - 7 -> prescaler 2-128*/
#ifndef ADC_INCLUDE
#define ADC_INCLUDE
#define ADC_PRESCALER 7 
#define VREF_EXT        0
#define VREF_INT        3


/* read ADC from one pin */
uint16_t adc_read(uint8_t );

/* sets voltage reference. 
possible values for ref
    0 - AREF extern
    1 - extern with capactiro an aref pin
    3 - internal 1,1v
values for prescaler
    0 - 7 -> prescaler 2-128*/
void adc_init(uint8_t ref);


/* disables Digital input puffer for ADC pin
0 - 5 */
void adc_init_pin(uint8_t);




void adc_enable();
void adc_disable();


#endif