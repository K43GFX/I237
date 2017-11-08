#include <stdio.h>
#define __ASSERT_USE_STDERR
#include <assert.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

#define BLINK_DELAY_MS 300

static inline void init_leds(void)
{
    /* Set port A pin 7 for output*/
    DDRB |= _BV(DDB7);
    /* Turn off yellow LED */
    PORTB &= ~_BV(PORTB7);
    /* Set port A pins for output */
    DDRA |= _BV(DDA0);
    DDRA |= _BV(DDA2);
    DDRA |= _BV(DDA4);
}


/* Init error console as stderr in UART1 and print user code info */
static inline void init_errcon(void)
{
    simple_uart1_init();
    stderr = &simple_uart1_out;
    fprintf(stderr, "Version: %s built on: %s %s\n",
            FW_VERSION, __DATE__, __TIME__);
    fprintf(stderr, "avr-libc version: %s avr-gcc version: %s\n",
            __AVR_LIBC_VERSION_STRING__, __VERSION__);
}


static inline void blink_leds(void)
{
    /* Not putting into cycle, otherwise wouldn't proceed with main() */
    PORTA |= _BV(PORTA0);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA0);
    _delay_ms(BLINK_DELAY_MS);
    PORTA |= _BV(PORTA2);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA2);
    _delay_ms(BLINK_DELAY_MS);
    PORTA |= _BV(PORTA4);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA4);
    _delay_ms(BLINK_DELAY_MS);
}


void main(void)
{
    init_leds();
    init_errcon();
    /* Test assert - REMOVE IN FUTURE LABS */
    char *array;
    uint32_t i = 1;
    extern int __heap_start, *__brkval;
    int v;
    array = malloc( i * sizeof(char));
    assert(array);
    /* End test assert */

    while (1) {
        blink_leds();
        /* Test assert - REMOVE IN FUTURE LABS */
        /*
         * Increase memory allocated for array by 100 chars
         * until we have eaten it all and print space between stack and heap.
         * That is how assert works in run-time.
         */
        array = realloc( array, (i++ * 1000) * sizeof(char));
        fprintf(stderr, "%d\n",
                (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
        assert(array);
        /* End test assert */
    }
}
