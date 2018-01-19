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
#include <stdlib.h>
#include "../lib/matejx_avr_lib/mfrc522.h"
#include "../lib/andy_brown_memdebug/memdebug.h"

#define UART_BAUD 9600
#define UART_STATUS_MASK 0x00FF

#define BLINK_DELAY_MS  900
#define LED_RED         PORTA0 // Arduino Mega digital pin 22
#define LED_GREEN       PORTA2 // Arduino Mega digital pin 24


volatile uint32_t seconds;
static uint32_t prev_time;


static inline void init_rfid_reader(void)	
{
	/*	Init	RFID-RC522	*/
	MFRC522_init();
	PCD_Init();
}

// Create microrl object and pointer on it
microrl_t rl;
microrl_t *prl = &rl;

static inline void init_timer(void)
{
    seconds = 0; // Set counter to random number 0x19D5F539 in HEX. Set it to 0 if you want
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= _BV(WGM12); // Turn on CTC (Clear Timer on Compare)
    TCCR1B |= _BV(CS12); // fCPU/256
    OCR1A = 62549; // Note that it is actually two registers OCR5AH and OCR5AL
    TIMSK1 |= _BV(OCIE1A); // Output Compare A Match Interrupt Enable
}


static inline void init_leds(void)
{
    /* RGB LED board set up and off */
    DDRA |= _BV(LED_RED) | _BV(LED_GREEN);
    PORTA &= ~(_BV(LED_RED) | _BV(LED_GREEN));
}


static inline void heartbeat(void)
{
    /* Clone counter */
    uint32_t now;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        now = seconds;
    }
    char ascii_buf[11] = {0x00};

    if (prev_time != now) {
        ltoa(now, ascii_buf, 10);
        uart1_puts_p(PSTR("Uptime: "));
        uart1_puts(ascii_buf);
        uart1_puts_p(PSTR(" s.\r\n"));
    }

    //Toggle LED
    PORTA ^= _BV(LED_GREEN);
    prev_time = now;
}


void main(void)
{
    /* Initialize LEDs */
    init_leds();
    /* Initialize UART interfaces */
    uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    /* Print out student name and terminal prefix */
    uart0_puts_p(PSTR(STUD_NAME "\r\n"));
    uart0_puts_p(PSTR("Use backspace to delete entry and enter to confirm.\r\n"));
    uart0_puts_p(PSTR("Arrow keys and del doesn't work currently.\r\n"));
    /* Initialize heartbeat timer */
    init_timer();
    /* Start terminal */
    microrl_init(prl, uart0_puts);
    microrl_set_execute_callback(prl, cli_execute);

    init_rfid_reader();
    /* Allow interruptions */
    sei();

    while (1) {
        heartbeat();
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
    }
}

ISR(TIMER1_COMPA_vect)
{
    /* Increase heartbeat counter by 1 */
    seconds++;
}
