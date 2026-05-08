#include "adc.h"

/*
 * ADC Initialization
 *
 * ADMUX:
 *   REFS0 = 1  → AVcc as reference voltage (5V)
 *   ADLAR = 1  → left-align result, so ADCH = 8-bit result
 *   channel 0  → PA0 is the microphone input pin
 *
 * ADCSRA:
 *   ADEN  = 1  → enable the ADC
 *   ADPS2 = 1,
 *   ADPS1 = 1  → prescaler = /64
 *               11059200 / 64 = 172800 Hz ADC clock (safe range: 50-200 kHz)
 */
void ADC_init(void)
{
    ADMUX = (1 << REFS0)   /* AVcc reference                  */
            | (1 << ADLAR) /* left-align → 8-bit via ADCH     */
            | 0;           /* channel 0 = PA0 (malhash lazma bs ana bat7anyk)                */

    ADCSRA = (1 << ADEN)    /* enable ADC                      */
             | (1 << ADPS2) /* prescaler bits: 110 = /64       */
             | (1 << ADPS1);
}

/*
 * Read one 8-bit ADC sample
 *   - starts a conversion
 *   - waits until hardware clears ADSC (conversion done)
 *   - returns ADCH (8 most significant bits)
 */

uint8_t ADC_read(void)
{
    ADCSRA |= (1 << ADSC); /* start conversion                */
    while (ADCSRA & (1 << ADSC))
        ;        /* wait — ADSC clears when done    */
    return ADCH; /* return 8-bit result             */
}
