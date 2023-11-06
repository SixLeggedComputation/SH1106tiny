/*
 * Conversion routines for DS3231
 */

#ifndef BCD_CONV_H
#define BCD_CONV_H

#include <Arduino.h>
#include "rtc_pilot.h"

// conversion routine for translating back from sexagesimal time to bcd encoding
// not used in this example
byte dec_to_bcd(byte n);

// changes fields of a ParsedTime struct and replace their BCD value with sexagesimal counterpart
void convert_dec(ParsedTime* t);

// changes fields of a ParsedDate struct and replace their BCD value with decimal counterpart
// notice that dow field is not converted, as it is not used here.
void convert_dec(ParsedDate* d);

#endif
