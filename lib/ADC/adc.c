/*

REGISTER
ADMUX - multiplexer


*/


#include <avr/io.h>

#include <util/delay.h>


#include <adc.h>

void adc_init(eADC_REF ref){
    ADMUX |= (ref << 6);
    ADCSRA |= ADC_PRESCALER;
    //enable ADC
    ADCSRA |= (1<<ADEN);
}

void adc_enable(){
    ADCSRA |= (1<<ADEN);
}
void adc_disable(){
    ADCSRA &= ~(1<<ADEN);
}

void adc_init_pin(uint8_t pin){
    //disable digital input buffer for that pin
    DIDR0 |= (1 << pin);
}

uint16_t adc_read(uint8_t port){


    //clear port selection in ADMUX
    ADMUX &= 0xF0;
    //set correct port
    ADMUX |= port;
    //set ADC start bit to start converion
    ADCSRA |= (1<<ADSC);

    //wait till ADC interupt flag ist set-> conversion finished and data register updated
    while (!(ADCSRA & (1<<ADIF))){};
    return ADC;

}



