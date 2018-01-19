#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/matejx_avr_lib/mfrc522.h"

/* Read the data from RFID object */
void cli_rfid_read(const char *const *argv) {
    (void) argv;
    Uid uid;
    Uid *uid_ptr = &uid;
    char uid_size[2];
    char card_uid[20];
    uart0_puts("\n");

    if (PICC_IsNewCardPresent()) {
        uart0_puts("Card selected!\r\n");
        PICC_ReadCardSerial(uid_ptr);
        itoa(uid.size, uid_size, 10);

        uart0_puts("UID size: ");
        uart0_puts(uid_size);
        uart0_puts("UID sak: ");

        uart0_puts(PICC_GetTypeName(PICC_GetType(uid.sak))); /* sak */
        uart0_puts("Card UID: ");

        for (byte i=0; i<uid.size; i++) {
            itoa(uid.uidByte[1], card_uid, 10);
            uart0_puts(card_uid);
        }

        uart0_puts("\r\n");
    } else {
        uart0_puts("Unable to select card.\r\n");
    }
}