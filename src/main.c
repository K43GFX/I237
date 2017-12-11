#include <stdio.h>
#include <string.h> 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "../lib/andygock_avr-uart/uart.h"
#include "hmi_msg.h"
#include "../lib/helius_microrl/microrl.h"
#include "cli_microrl.h"

#define UART_BAUD 9600
#define UART_STATUS_MASK 0x00FF

#define BLINK_DELAY_MS  2000
#define LED_RED         PORTA0 // Arduino Mega digital pin 22
#define LED_GREEN       PORTA2 // Arduino Mega digital pin 24

#define COUNT_SECONDS

#define ASCII_PRINT

#ifdef ASCII_PRINT
#include <stdlib.h> // stdlib is needed to use ltoa() - Long to ASCII
#endif 

volatile uint32_t counter_1;
static uint32_t prev_time;

//  Create  microrl object  and pointer on  it
microrl_t rl;
microrl_t *prl = &rl;

static inline void init_counter_1(void)
{
    counter_1 = 0; // Set counter to random number 0x19D5F539 in HEX. Set it to 0 if you want
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= _BV(WGM12); // Turn on CTC (Clear Timer on Compare)
    TCCR1B |= _BV(CS12); // fCPU/256
    OCR1A = 62549; // Note that it is actually two registers OCR5AH and OCR5AL
    TIMSK1 |= _BV(OCIE1A); // Output Compare A Match Interrupt Enable
}


static inline void init_led(void)
{
    /* Set pin 0 of PORTA (ARDUINO mega pin 22) for output and set low */
    DDRA |= _BV(LED_RED);
    PORTA &= ~_BV(LED_RED);
}


static inline void heartbeat(void)
{
    uint32_t now;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        now = counter_1;
    }

    char ascii_buf[11] = {0x00};

    if (prev_time != now) {
        //Print uptime to uart1
        ltoa(now, ascii_buf, 10);
        uart1_puts_p(PSTR("Uptime: "));
        uart1_puts(ascii_buf);
        uart1_puts_p(PSTR(" s.\r\n"));
    }
    prev_time = now;
    PORTA ^= _BV(LED_GREEN);
}



static inline void simu_big_prog(void)
{
    /* Simulate big program with delay and toggle LED */
    PORTA ^= _BV(LED_RED);
    _delay_ms(BLINK_DELAY_MS);
}


void main(void)
{
    uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart0_puts_p(PSTR("Console started\r\n"));
    init_counter_1();
    microrl_init(prl, uart0_puts);
    microrl_set_execute_callback(prl, cli_execute);
    sei();

    while (1) {
        heartbeat();
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
    }

}

ISR(TIMER1_COMPA_vect)
{
  counter_1++;
}
