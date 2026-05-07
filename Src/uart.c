#include "../inc/uart.h"

void UART_init(long USART_BAUDRATE)
{
    // Load lower 8-bits of the baud rate
    UBRRL = BAUD_PRESCALE;
    // Load upper 8-bits
    UBRRH = (BAUD_PRESCALE >> 8);
    // Turn on transmission and reception
    UCSRB |= (1 << RXEN) | (1 << TXEN);
    // Use 8-bit char size
    UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);

    // 3ashan lw 3ozna nesha8l el printf
    static FILE uart_stream = FDEV_SETUP_STREAM(UART_putChar, UART_getChar, _FDEV_SETUP_RW);
    stdout = &uart_stream;
    stdin = &uart_stream;
}

int UART_getChar(FILE *stream)
{
    // Wait for data to be received
    while ((UCSRA & (1 << RXC)) == 0)
        ;
    return (UDR);
}

int UART_putChar(char c, FILE *stream)
{
    // Wait for empty transmit buffer
    while (!(UCSRA & (1 << UDRE)))
        ;
    UDR = c;
    return 0;
}

void UART_send_string(char *str)
{
    while (*str)
    {
        UART_putChar(*str++, NULL);
    }
}

void UART_get_string(char *buffer, uint8_t max_len)
{
    uint8_t i = 0;
    char c;
    while (i < max_len - 1)
    {
        c = UART_getChar(NULL);
        UART_putChar(c, NULL);
        if (c == '\r' || c == '\n')
            break;
        buffer[i++] = c;
    }
    buffer[i] = '\0'; // Null-terminate string
}

// Initialize the stream for standard I/O redirection
FILE uart_str = FDEV_SETUP_STREAM(UART_putChar, UART_getChar, _FDEV_SETUP_RW);