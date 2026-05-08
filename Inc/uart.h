#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <avr/io.h>

// Macros for baud rate calculation based on 11.0592 MHz crystal
#define F_CPU 11059200UL
#define BAUD_RATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD_RATE * 16UL))) - 1)

// Function prototypes
void UART_init(long USART_BAUDRATE);
int UART_getChar(FILE *stream);
int UART_putChar(char c, FILE *stream);
void UART_send_string(char *str);
void UART_get_string(char *buffer, uint8_t max_len);
void UART_menu();
void UART_handle_command(char cmd);
int8_t UART_availiable();

// Global file stream declaration for printf/scanf support
extern FILE uart_str;

#endif