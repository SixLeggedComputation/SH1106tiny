/*
* Types and methods definition for communication with DS3231
*/

#ifndef RTC_PILOT_H
#define RTC_PILOT_H

#include <Arduino.h>

// DS3231 I2C address from datasheet
#define RTC_I2C_ADDR 0x68

// separates hours minutes and seconds in display
#define DATE_FIELD_SEPARATOR 0x2d
// separates day month and years in display
#define TIME_FIELD_SEPARATOR 0x3a
// date format: dd-mm-20yy
#define DATE_STR_LEN 10
// time format: hh/mm/ss
#define TIME_STR_LEN 8

// hours, minutes, seconds
struct ParsedTime{
  byte h,m,s;
};

// day, month, year, day of week
struct ParsedDate{
  byte d,m,y, dow;
};

// a structure for handling parsed data together
struct RTCParsedData{
  ParsedTime t;
  ParsedDate dt;
};

// retrieve current date and time information from DS3231
// returned values are BCD codes, which must be translated before displaying them
RTCParsedData rtc_now();

// builds date string for display
unsigned char* date_to_char(const ParsedDate* aDate);

// builds time strinf for display
unsigned char* time_to_char(const ParsedTime* aTime);

#endif
