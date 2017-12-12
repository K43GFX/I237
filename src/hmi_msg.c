#include <avr/pgmspace.h>
#include "hmi_msg.h"

static const char nmb0[] PROGMEM = "zero";
static const char nmb1[] PROGMEM = "one";
static const char nmb2[] PROGMEM = "two";
static const char nmb3[] PROGMEM = "three";
static const char nmb4[] PROGMEM = "four";
static const char nmb5[] PROGMEM = "five";
static const char nmb6[] PROGMEM = "six";
static const char nmb7[] PROGMEM = "seven";
static const char nmb8[] PROGMEM = "eight";
static const char nmb9[] PROGMEM = "nine";

PGM_P const number_name[] PROGMEM = {nmb0, nmb1, nmb2, nmb3, nmb4, nmb5, nmb6, nmb7, nmb8, nmb9};