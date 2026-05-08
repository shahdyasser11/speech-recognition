#define F_CPU 11059200UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdint.h>

#include "adc.h"
#include "timer.h"
#include "features.h"
#include "templates.h"
#include "lcd.h"
#include "uart.h"

/* ================================================================
 *  CONSTANTS
 * ================================================================ */
#define FRAME_SIZE 200     /* samples per frame (25 ms at 8kHz)  */
#define NUM_FRAMES 40      /* frames per utterance (1 second)     */
#define ENERGY_THRESHOLD 8 /* dah value mabda2y neb2a nl3b fyh bra7etna */

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
char WORD[6];

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

static uint8_t classify(Features avg)
{

    Features norm;
    norm.rms = (avg.rms - feature_mean[0]) / feature_std[0];
    norm.zcr = (avg.zcr - feature_mean[1]) / feature_std[1];
    norm.envelope = (avg.envelope - feature_mean[2]) / feature_std[2];

    float min_dist = 1e9f;
    uint8_t best_word = 0;

    for (uint8_t w = 0; w < NUM_WORDS; w++)
    {
        float diff_rms = norm.rms - word_templates[w][0];
        float diff_zcr = norm.zcr - word_templates[w][1];
        float diff_env = norm.envelope - word_templates[w][2];

        float dist = sqrtf((diff_rms * diff_rms) + (diff_zcr * diff_zcr) + (diff_env * diff_env));

        if (dist < min_dist)
        {
            min_dist = dist;
            best_word = w;
        }
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
        PORTA |= (1 << PA1);
        PORTA &= ~((1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 1: // off
        PORTA |= (1 << PA2);
        PORTA &= ~((1 << PA1) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 2: // start
        PORTA |= (1 << PA3);
        PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 3: // stop
        PORTA |= (1 << PA4);
        PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 4: // left
        PORTA |= (1 << PA5);
        PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA6) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 5: // right
        PORTA |= (1 << PA6);
        PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA7));
        PORTC &= ~(1 << PC3);
        break;
    case 6: // up
        PORTA |= (1 << PA7);
        PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6));
        PORTC &= ~(1 << PC3);
        break;
    case 7: // down
        PORTC |= (1 << PC3);
        PORTA &= ~((1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
        break;

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
        strcpy(WORD, "ON");
        break;
    case 1: // off
        strcpy(WORD, "OFF");
        break;
    case 2: // start
        strcpy(WORD, "START");
        break;
    case 3: // stop
        strcpy(WORD, "STOP");
        break;
    case 4: // left
        strcpy(WORD, "LEFT");
        break;
    case 5: // right
        strcpy(WORD, "RIGHT");
        break;
    case 6: // up
        strcpy(WORD, "UP");
        break;
    case 7: // down
        strcpy(WORD, "DOWN");
        break;

    default:
        break;
    }
    LCD_String_xy(0, 5, WORD);
}

int main(void)
{
    ADC_init();
    Timer1_init();
    Leds_init();
    LCD_Init();
    UART_init(9600);
    sei();

    while (1)
    {

        uint8_t raw = ADC_read();
        int8_t dev = (int8_t)((int16_t)raw - 128);
        if (dev < 0)
            dev = -dev;
        if (dev < ENERGY_THRESHOLD)
            continue;

        isr_buf = 0;
        isr_idx = 0;
        frame_rdy = 0;

        /* accumulators — will sum features across all 40 frames --> dah 3ashan kol el 8000 byte */
        float sum_rms = 0.0f;
        float sum_zcr = 0.0f;
        float sum_env = 0.0f;

        recording = 1; /* ISR starts collecting samples now */

        for (uint8_t fn = 0; fn < NUM_FRAMES; fn++) // loops around all the frames
        {
            /* wait until ISR signals a frame is ready */
            while (!frame_rdy)
                ;
            frame_rdy = 0;

            /* ISR just swapped, so finished frame is in (isr_buf ^ 1) */
            uint8_t done = isr_buf ^ 1;

            /* extract features — returns a clean Features struct */
            Features f = extract_frame_features((const uint8_t *)buf[done]);

            /* accumulate */
            sum_rms += f.rms;
            sum_zcr += f.zcr;
            sum_env += f.envelope;
        }

        recording = 0; /* ISR stops collecting */

        Features avg;
        avg.rms = sum_rms / NUM_FRAMES;
        avg.zcr = sum_zcr / NUM_FRAMES;
        avg.envelope = sum_env / NUM_FRAMES;

        uint8_t word = classify(avg);
        outputLeds(word);
        outputLCD(word);
    }
    // to be implemented :
    // Bonus Idea: UART Command Interface

    // The document suggests a "Bonus Idea" to "Add UART command interface".

    // This would allow you to trigger the recording from your computer keyboard (e.g., press 'r' to start sampling)
    // rather than relying solely on the Energy Threshold trigger in your main.c.

    return 0;
}