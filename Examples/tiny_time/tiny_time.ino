/*
 * ATTiny clock
 * 
 * Uses ATTiny85, DS3231 and SH1106
 * 
 * ATTiny85 decodes date and time values produced by DS3231 and displays them on screen
 * 
 * ATTiny pin 7 (SCL) -> SH1106and D3231 SCL pins
 * ATTiny pin 5 (SDA) -> SH1105 and D3231 SDA pins
 * 
 * 330 Ohms pullups on both SDA and SCL buses
 * 10 nF decoupling capacitor tied to ATTiny VCC pin 8
 * 
 * For the sake of saving program memory, this code does not address RTC configuration.
 * RTC is supposed to be set up earlier and configures for 24h display. This example has not been tested with 
 * 12h display.
 * 
 * Reading from DS3231 involves no library. Ad hoc code is located in rtc_pilot.h and bcd_conv.h.
 * 
 * Dates are displayed in UK format. Years count start at 2000.
 * Sample code not to be run in TARDIS environment.
 * 
 */
 
#include<TinyWireM.h>
#include <SH1106mini.h>
#include "rtc_pilot.h"
#include "bcd_conv.h"

#define SCREEN_I2C_ADDR 0x3c

SH1106mini display(SCREEN_I2C_ADDR);

// Stores time when devices started
// Later on, it is updated after each sleep
// It's better to store this piece of information in human readable format as ATTiny hardware does not support floating point arithmetic
RTCParsedData WakeUp;

bool ToBeDone = true;

// By default osciallator is enabled at start and fires an interrupt, which must be confounded with alarm1 signal and must be skipped
volatile bool first_loop = true;

void setup() {
  display.init();           // YinyWireM started here
  display.setContrast(255);
  display.rotate();
  display.clear();
}

void loop() {
  display.clear();
  
  WakeUp = rtc_now();
  
  convert_dec(&WakeUp.t);
  convert_dec(&WakeUp.dt);

  display_date(1);
  display_time(10);

  delay(2000);
}

void display_date(int line_number){
  shchar_t* s = date_to_char(&WakeUp.dt);
  display.printArrayAt(s, DATE_STR_LEN, line_number, pos_t::CENTRE, angle_t::R90);
  delete[] s;
}

void display_time(int line_number){
  shchar_t* s = time_to_char(&WakeUp.t);
  display.printArrayAt(s, TIME_STR_LEN,line_number, pos_t::CENTRE, angle_t::R90);
  delete[] s;
}
