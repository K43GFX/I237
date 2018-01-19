#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/matejx_avr_lib/mfrc522.h"
#include <string.h>

typedef struct identity {
    char *client;
    char *uid;
    struct identity *next;
} identity_t;


identity_t * authorized_identities = NULL;


/* Read the data from RFID object */
void rfid_read(const char *const *argv)
{
    (void) argv;
    Uid uid;
    Uid *uid_ptr = &uid;
    char uid_size[2];
    char card_uid[10];
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

        for (byte i = 0; i < uid.size; i++) {
            itoa(uid.uidByte[1], card_uid, 10);
            uart0_puts(card_uid);
        }

        uart0_puts("\r\n");
    } else {
        uart0_puts("Unable to select card.\r\n");
    }
}

/* Add new RFID object to authorized list */
void rfid_new(const char *const *argv)
{
    uart0_puts("\r\n");
    identity_t *new_identity;
    new_identity = malloc(sizeof(identity_t));

    /*
    TODO: search for begin / try method?
    argv[1] was uid
    argv[2] was provided client
    */

    /* Lets see if our UID is smaller than 10 bytes*/
    if (!(strlen(argv[1]) > 10)) {
        /* Properly allocated? */
        if (new_identity == NULL) {
            uart0_puts("MEMFAIL\r\n");
            free(new_identity);
            return;
        }
    } else {
        uart0_puts("UID longer than 10 bytes is not allowed!\r\n");
        return;
    }

    /* Uid memory allocation failsafe */
    new_identity->uid = malloc(strlen(argv[1]) + 1);

    if (new_identity->uid == NULL) {
        uart0_puts("MEMFAIL\r\n");
        free(new_identity);
        return;
    }

    /* Client memory allocation failsafe */
    new_identity->client = malloc(strlen(argv[2]) + 1);

    if (new_identity->client == NULL) {
        uart0_puts("MEMFAIL\r\n");
        free(new_identity);
        return;
    }

    /* Copy stuff to memory */
    strcpy(new_identity->uid, argv[1]);
    strcpy(new_identity->client, argv[2]);

    /* Let's check if we have any cards added so far */
    if (authorized_identities != NULL) {
        new_identity->next = authorized_identities;
        identity_t *current = authorized_identities;

        while (current != NULL) {
            /* Checking if UID is unique */
            if (!strcmp(current->uid, new_identity->uid)) {
                uart0_puts("CARD_PRESENT_MSG \r\n");
                return;
            }

            current = current->next;
        }
    } else {
        new_identity->next = NULL;
    }

    /* Neat, since we passed everything let's authorize this one  */
    authorized_identities = new_identity;
    uart0_puts("Card added!\r\nCard UID: ");
    uart0_puts(new_identity->uid);
    //uart0_puts("\r\nUID Size: ");
    //uart0_puts(PSTR(strlen(new_identity->uid)));
    //uart0_puts(" bytes\r\nUser name: ");
    uart0_puts("\r\nUser name: ");
    uart0_puts(new_identity->client);
    uart0_puts("\r\n");
}

/* Remove RFID object from authorized list */
void rfid_remove(const char *const *argv)
{
    identity_t *current = authorized_identities;
    identity_t *prev = NULL;

    /* Checking if ANY cards added at all */
    if (authorized_identities == NULL) {
        uart0_puts("No cards added to the system.\r\n");
        return;
    } else {
        /* While some cards are still not compared */
        while (current != NULL) {
            /* Specific card discovered? */
            if (!strcmp(argv[1], current->uid)) {
                /* LETS REMOVE CARD */
                uart0_puts("Card successfully removed!\r\n");
                free(current);
                return;
            } else {
                /* Not that card */
                if (prev == NULL) {
                    authorized_identities = current->next;
                } else {
                    prev->next = current->next;
                }
            }

            prev = current;
            current = current->next;
        }

        /*
        GLITCH: success an not found messages are turned around :/ */
        uart0_puts("Specific card was not found.\n\r");
        return;
    }
}

/* Show all authorized cards in the system */
void rfid_show(void)
{
    uart0_puts("\r\n");

    /* Are any cards even added to the system? */
    if (authorized_identities == NULL) {
        uart0_puts("No authorized cards added to the system yet.\r\n");
    } else {
        identity_t *current = authorized_identities;
        int counter = 1;
        char counts[10];

        /* for every card */
        while (current != NULL) {
            itoa(counter, counts, 10);
            uart0_puts(counts);
            uart0_puts(" UID: ");
            uart0_puts(current->uid);
            uart0_puts(" ");
            uart0_puts("User: ");
            uart0_puts(current->client);
            uart0_puts("\r\n");
            current = current->next;
            counter++;
        }
    }
}
