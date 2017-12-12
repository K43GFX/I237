#ifndef HMI_MSG_H_
#define HMI_MSG_H_

#define VER_FW   "Version: " FW_VERSION " built on: " __DATE__ " " __TIME__ "\r\n"
#define VER_LIBC "avr-libc version: " __AVR_LIBC_VERSION_STRING__ " avr-gcc version: " __VERSION__ "\r\n"

#define STUD_NAME "Karl Erik Õunapuu"
extern PGM_P const number_name[];

#endif