/* 3yzin timer yeshta8l kol 8000 khz*/
#include "timer.h"

void Timer1_init(void)
{
    // without a prescaler -- > 1383 in counter register (ctc mode)
    TCCR1B |= (1 << CS10); // clk/1 --> no prescaler

    // ctc mode
    TCCR1B |= (1 << WGM12);
    TCCR1B &= ~(1 << WGM13);
    TCCR1A &= ~(1 << WGM11);
    TCCR1A &= ~(1 << WGM10);

    OCR1A = 1382;           // for the 8KHz
    TIMSK |= (1 << OCIE1A); // to enable the timer1 compare match to register A int.
}
