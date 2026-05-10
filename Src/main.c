#define F_CPU 11059200UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#include "adc.h"
#include "timer.h"
#include "features.h"
#include "templates.h"
#include "lcd.h"
#include "uart.h"

/* ================================================================
 *  CONSTANTS
 * ================================================================ */
#define FRAME_SIZE 200      /* samples per frame (25 ms at 8kHz)  */
#define NUM_FRAMES 80       /* frames per utterance (1 second)     */
#define ENERGY_THRESHOLD 20 /*dah value mabda2y neb2a nl3b fyh bra7etna */

/* ================================================================
 *  DOUBLE BUFFER — shared between ISR and main()
 *
 *  buf[0] and buf[1] take turns:
 *    ISR  fills one buffer
 *    main processes the other
 *  They never touch the same buffer at the same time.
 * !!!!!IMPPPP(((ana 3amlt el 7anyaka dyh 3ashan fl 1 sec hykoon fyh 8000 byte w dah akbr mn el ram asln ya ema neshtry external eeprom)))
 * ================================================================ */

volatile uint8_t buf[2][FRAME_SIZE];
volatile uint8_t isr_buf = 0;   /* which buffer ISR is filling now  */
volatile uint8_t isr_idx = 0;   /* position inside that buffer      */
volatile uint8_t frame_rdy = 0; /* 1 = ISR finished a frame         */
volatile uint8_t recording = 0; /* 1 = actively capturing audio     */
char WORD[10];
uint8_t counter = 0;

/* ================================================================
 *  ISR — fires 8000 times per second (set up by Timer1_init)
 *
 *  Each call:
 *    1. reads one ADC sample
 *    2. stores it in the active buffer
 *    3. when 200 samples are collected:
 *       - sets frame_rdy so main() knows to process
 *       - swaps to the other buffer (isr_buf ^= 1)
 * ================================================================ */
ISR(TIMER1_COMPA_vect)
{
    if (!recording)
        return;

    buf[isr_buf][isr_idx++] = ADC_read(); // bamli wa7d mn el buffers l7d ma yekoon fyh 200 sample

    if (isr_idx >= FRAME_SIZE)
    {
        isr_idx = 0;
        frame_rdy = 1; /* signal main() that a frame is ready  */
        isr_buf ^= 1;  /* swap buffer: 0→1 or 1→0              */
    }
}

/*  CLASSIFY — Euclidean distance against stored template */
// void collect_word_samples(const char *word_name)
// {
//     float total_rms = 0, total_zcr = 0, total_env = 0;

//     // Initialize PB0 with internal pull-up
//     DDRB &= ~(1 << PB0);
//     PORTB |= (1 << PB0);

//     printf("\n--- TRAINING WORD: %s ---\n", word_name);

//     for (int s = 0; s < 10; s++)
//     {
//         printf("Sample %d/10: Press button, THEN speak.\n", s + 1);

//         // Wait for Button Press (Low)
//         while (PINB & (1 << PB0))
//             ;
//         _delay_ms(50); // Debounce

//         recording = 1; // Start ISR sampling

//         float sum_rms = 0, sum_zcr = 0, sum_env = 0;

//         // Collect 40 frames (1 second total)
//         for (uint8_t fn = 0; fn < 40; fn++)
//         {
//             while (!frame_rdy)
//                 ;
//             frame_rdy = 0;

//             uint8_t done = isr_buf ^ 1;
//             Features f = extract_frame_features((const uint8_t *)buf[done]);

//             sum_rms += f.rms;
//             sum_zcr += f.zcr;
//             sum_env += f.envelope;
//         }

//         recording = 0; // Stop ISR

//         // Average for this 1-second sample
//         total_rms += (sum_rms / 40);
//         total_zcr += (sum_zcr / 40);
//         total_env += (sum_env / 40);

//         printf("  Captured! Release button.\n");
//         while (!(PINB & (1 << PB0)))
//             ; // Wait for release
//         _delay_ms(200);
//     }

//     // Final Average Template for templates.h
//     printf("\n>>> FINAL TEMPLATE FOR %s <<<\n", word_name);
//     printf("{%f, %f, %f}\n\n",
//            (double)(total_rms / 20),
//            (double)(total_zcr / 20),
//            (double)(total_env / 20));
// }

static uint8_t classify(Features avg)
{

    printf("Features -> RMS : %f , ZCR : %f , ENV: %f \n", avg.rms, avg.zcr, avg.envelope);
    float min_dist = 1e9f;
    uint8_t best_word = 0;

    for (uint8_t w = 0; w < NUM_WORDS; w++)
    {
        float diff_rms = avg.rms - word_templates[w][0];
        float diff_zcr = avg.zcr - word_templates[w][1];
        float diff_env = avg.envelope - word_templates[w][2];

        float dist = (diff_rms * diff_rms) + (diff_zcr * diff_zcr) + (diff_env * diff_env);

        if (dist < min_dist)
        {
            min_dist = dist;
            best_word = w;
        }

        printf("Word = %d and distance = %f\n", w, dist);
    }

    return best_word;
}

void Leds_init()
{

    DDRA |= (1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7);
    DDRC |= (1 << PC3);
}

void outputLeds(uint8_t word)
{
    switch (word)
    {
    case 0: // on
        printf("led 1 on");
        PORTA |= (1 << PA1);
        PORTA &= ~((1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 1: // off
        printf("led 2 on");
        PORTA |= (1 << PA2);
        PORTA &= ~((1 << PA1) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 2: // start
        printf("led 3 on");
        PORTA |= (1 << PA3);
        PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 3: // stop
        printf("led 4 on");
        PORTA |= (1 << PA4);
        PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
        // case 4: // left
        //     printf("led 5 on");
        //     PORTA |= (1 << PA5);
        //     PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA6) | (1 << PA7));
        //     PORTC &= ~(1 << PC3);
        //     break;
        // case 5: // right
        //     printf("led 6 on");
        //     PORTA |= (1 << PA6);
        //     PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA7));
        //     PORTC &= ~(1 << PC3);
        //     break;
        // case 6: // up
        //     printf("led 7 on");
        //     PORTA |= (1 << PA7);
        //     PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6));
        //     PORTC &= ~(1 << PC3);
        //     break;
        // case 7: // down
        //     printf("led 8 on");
        //     PORTC |= (1 << PC3);
        //     PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        //     break;

    default:
        break;
    }
}

void outputLCD(uint8_t word)
{
    LCD_Clear();
    switch (word)
    {
    case 0: // on

        strcpy(WORD, "CAIRO");

        break;
    case 1: // off

        strcpy(WORD, "ASYUT");

        break;
    case 2: // start

        strcpy(WORD, "GHARBIA");

        break;
    case 3: // stop

        strcpy(WORD, "KENA");

        break;
        // case 4: // left

        //     strcpy(WORD, "LEFT");

        //     break;
        // case 5: // right

        //     strcpy(WORD, "RIGHT");

        //     break;
        // case 6: // up

        //     strcpy(WORD, "UP");
        //     // printf(" i read up\n");
        //     break;
        // case 7: // down

        //     strcpy(WORD, "DOWN");
        //     // printf(" i read down\n");
        //     break;

    default:
        return;
    }
    LCD_String_xy(0, 5, WORD);
    printf("\n I Heared %s\n", WORD);
}

int main(void)
{
    ADC_init();
    Timer1_init();
    Leds_init();
    LCD_Init();
    LCD_String("Testing...");
    UART_init(9600);
    sei();
    UART_menu();

    // collect_word_samples("GHARBIA");
    DDRB &= ~(1 << PB0); // Set PB0 as input
    PORTB |= (1 << PB0); // Enable pull-up resistor
    while (1)
    {
        // Wait for Button Press (PB0) to start listening
        printf("press button to record\n");
        while (PINB & (1 << PB0))
            ;
        _delay_ms(50); // Debounce

        LCD_Clear();
        LCD_String("Listening...");

        // Reset buffers and indices
        isr_buf = 0;
        isr_idx = 0;
        frame_rdy = 0;

        float sum_rms = 0.0f;
        float sum_zcr = 0.0f;
        float sum_env = 0.0f;

        recording = 1; // ISR starts collecting samples

        // Collect 40 frames (1 second)
        for (uint8_t fn = 0; fn < NUM_FRAMES; fn++)
        {
            while (!frame_rdy)
                ;
            frame_rdy = 0;

            uint8_t done = isr_buf ^ 1;
            Features f = extract_frame_features((const uint8_t *)buf[done]);

            sum_rms += f.rms;
            sum_zcr += f.zcr;
            sum_env += f.envelope;
        }

        recording = 0; // Stop ISR

        //  Final Classification
        Features avg;
        avg.rms = sum_rms / NUM_FRAMES;
        avg.zcr = sum_zcr / NUM_FRAMES;
        avg.envelope = sum_env / NUM_FRAMES;

        uint8_t word = classify(avg);
        outputLeds(word);
        outputLCD(word);

        // Wait for button release before allowing next command
        printf("captured ! release button\n");
        while (!(PINB & (1 << PB0)))
            ;
        _delay_ms(200);
    }

    return 0;
}