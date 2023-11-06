#ifndef BCD_CONV_CPP
#define BCD_CONV_CPP

#include <Arduino.h>
#include "bcd_conv.h"
#include "rtc_pilot.h"

// bcd->dec conversion mask to be used for all date and time fields, except month
// Consider computing a specific mask for RTC 12h display configuration
#define CUSTOM_MASK 0b11110000

// masque pour le décodage du mois : on annule le bit century
#define MONTH_MASK 0b01110000

byte dec_to_bcd(byte n){
 return (n/10)*16 + (n % 10);
}

// decoding_mask sert à annuler des bits de contrôles qui ne font pas partie de l'encodage BCD
byte bcd_to_dec(const byte * reading, byte decoding_mask){
  
  byte mult = ((*reading) & decoding_mask) >> 4;
  byte mod = ((*reading) & 0b00001111);
  return 10*mult+mod;
}

void convert_dec(ParsedTime* t){
  byte tmp = t->s;
  t->s = bcd_to_dec(&tmp, CUSTOM_MASK);

  tmp = t->m;
  t->m = bcd_to_dec(&tmp, CUSTOM_MASK);

  tmp = t->h;
  t->h = bcd_to_dec(&tmp, CUSTOM_MASK); 
}

void convert_dec(ParsedDate* DateBuffer){
  byte tmp = DateBuffer->d;
  DateBuffer->d = bcd_to_dec(&tmp, CUSTOM_MASK);

  tmp = DateBuffer->m;
  DateBuffer->m = bcd_to_dec(&tmp, MONTH_MASK);

  tmp = DateBuffer->y;
  DateBuffer->y = bcd_to_dec(&tmp, CUSTOM_MASK);
}

#endif
