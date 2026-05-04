#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// LCD pins
#define RS PB0
#define EN PB1

void LCD_pulse() {
    PORTB |= (1<<EN);
    _delay_us(1);
    PORTB &= ~(1<<EN);
    _delay_ms(2);
}

void LCD_send4(unsigned char data) {
    PORTB &= 0x0F;              // clear upper 4 bits
    PORTB |= (data & 0xF0);     // send upper nibble
    LCD_pulse();
}

void LCD_cmd(unsigned char cmd) {
    PORTB &= ~(1<<RS);
    LCD_send4(cmd);
    LCD_send4(cmd << 4);
}

void LCD_data(unsigned char data) {
    PORTB |= (1<<RS);
    LCD_send4(data);
    LCD_send4(data << 4);
}

void LCD_init() {
    DDRB = 0xFF;
    _delay_ms(20);

    LCD_send4(0x30);
    LCD_send4(0x30);
    LCD_send4(0x20); // 4-bit mode

    LCD_cmd(0x28); // 4-bit, 2 line
    LCD_cmd(0x0C); // display ON
    LCD_cmd(0x01); // clear
    LCD_cmd(0x06);
}

void LCD_print(char *str) {
    while (*str) LCD_data(*str++);
}

// ADC
void ADC_init() {
    ADMUX = (1<<REFS0);
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);
}

uint16_t ADC_read() {
    ADCSRA |= (1<<ADSC);
    while (ADCSRA & (1<<ADSC));
    return ADC;
}

int main(void) {
    DDRC = 0xFF; // LEDs
    LCD_init();
    ADC_init();

    char buffer[16];

    while (1) {
        uint16_t value = ADC_read();
        uint8_t led = value / 128;

        PORTC = (1 << led);

        LCD_cmd(0x01);

        sprintf(buffer, "LED %d", led + 1);
        LCD_print(buffer);

        _delay_ms(200);
    }
}