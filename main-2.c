#define F_CPU 11059200UL

#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

// ======================================================
// ADC FUNCTIONS
// ======================================================

void ADC_Init()
{
    ADMUX = (1 << REFS0); // AVCC reference, ADC0

    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Prescaler = 64
}

uint16_t ADC_Read(uint8_t channel)
{
    channel &= 0x07;

    ADMUX = (ADMUX & 0xF8) | channel;

    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC))
        ;

    return ADC;
}

// ======================================================
// MAIN
// ======================================================

int main(void)
{
    uint16_t adc_value;

    // PA1 -> PA7 outputs
    DDRA = 0xFE;

    // PC3 = LED8 output
    DDRC |= (1 << PC3);

    // LCD INIT
    LCD_Init();

    // ADC INIT
    ADC_Init();

    LCD_Clear();

    LCD_String_xy(0, 0, "Testing Pot");

    _delay_ms(1000);

    LCD_Clear();

    while (1)
    {
        // Read analog value from PA0
        adc_value = ADC_Read(0);

        // ======================================================
        // TURN OFF ALL LEDs
        // ======================================================

        PORTA = 0x00;
        PORTC &= ~(1 << PC3);

        // ======================================================
        // SIGNAL LEVEL DETECTION
        // ======================================================

        if (adc_value < 128)
        {
            PORTA = (1 << PA1);

            LCD_String_xy(0, 0, "LED 1 ON     ");
        }

        else if (adc_value < 256)
        {
            PORTA = (1 << PA2);

            LCD_String_xy(0, 0, "LED 2 ON     ");
        }

        else if (adc_value < 384)
        {
            PORTA = (1 << PA3);

            LCD_String_xy(0, 0, "LED 3 ON     ");
        }

        else if (adc_value < 512)
        {
            PORTA = (1 << PA4);

            LCD_String_xy(0, 0, "LED 4 ON     ");
        }

        else if (adc_value < 640)
        {
            PORTA = (1 << PA5);

            LCD_String_xy(0, 0, "LED 5 ON     ");
        }

        else if (adc_value < 768)
        {
            PORTA = (1 << PA6);

            LCD_String_xy(0, 0, "LED 6 ON     ");
        }

        else if (adc_value < 790)
        {
            PORTA = (1 << PA7);

            LCD_String_xy(0, 0, "LED 7 ON     ");
        }

        else
        {
            PORTC |= (1 << PC3);

            LCD_String_xy(0, 0, "LED 8 ON     ");
        }

        _delay_ms(20);
    }
}