#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 300

void blink_red(void)
{
    PORTA |= _BV(PORTA0);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA0);
    _delay_ms(BLINK_DELAY_MS);
}

void blink_green(void)
{
    PORTA |= _BV(PORTA2);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA2);
    _delay_ms(BLINK_DELAY_MS);
}

void blink_blue(void)
{
    PORTA |= _BV(PORTA4);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA4);
    _delay_ms(BLINK_DELAY_MS);
}
void main (void)
{
    /* Set port A pin 7 for output*/
    DDRB |= _BV(DDB7);
    /* Turn off yellow LED */
    PORTB &= ~_BV(PORTB7);
    /* Set port A pins for output */
    DDRA |= _BV(DDA0);
    DDRA |= _BV(DDA2);
    DDRA |= _BV(DDA4);

    while (1) {
        /* Play with colors */
        blink_red();
        blink_green();
        blink_blue();
    }
}