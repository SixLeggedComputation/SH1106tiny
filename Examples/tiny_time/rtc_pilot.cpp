#ifndef RTC_PILOT_CPP
#define RTC_PILOT_CPP

#include "rtc_pilot.h"
#include "bcd_conv.h"
#include<TinyWireM.h>

// masque à appliquer pour allumer un bit du registre A1M
#define A1_SET_MASK 128

// Index des bits du registre A1

// secondes
#define A1R_SEC 0
// minutes
#define A1R_MIN 1
// heures
#define A1R_H   2
// DY/DT
#define A1R_D   3

// DS3231 registers

// date register
#define REG_DATE 0

// A1 alarm (not used yet)
#define REG_A1 7

#define REG_CTRL_LOW 0x0e
#define REG_CTRL_HIGH 0x0f

// Stores BCD codes to be sent to alarm 1 (not used yet)
byte a1_bytes[4] = {0,0,0,0};

bool request(int how_many){
  if(how_many > 0){
    TinyWireM.requestFrom(RTC_I2C_ADDR, how_many);
    return true;
  }  
  else{
    return false;
  }
}

void send_reg_addr(byte reg_addr, bool close_after_sending){
  TinyWireM.beginTransmission(RTC_I2C_ADDR);
  TinyWireM.write(reg_addr);

  if (close_after_sending) {
    TinyWireM.endTransmission();  
  }
}

void fill_char_buffer(uint8_t value, unsigned char* db, uint8_t start_index){
  db[start_index] = (value<10) ? 0x30 : (0x30 + value/10);
  db[start_index+1] = 0x30 + ((value < 10) ? value : (value % 10));
}

unsigned char* date_to_char(const ParsedDate* aDate){
  unsigned char* conversion = new unsigned char[DATE_STR_LEN];
  fill_char_buffer(aDate->d, conversion, 0);
  conversion[2] = DATE_FIELD_SEPARATOR;
  fill_char_buffer(aDate->m, conversion, 3);
  conversion[5] = DATE_FIELD_SEPARATOR;
  conversion[6] = 0x32;
  conversion[7] = 0x30;
  fill_char_buffer(aDate->y, conversion, 8);
  return conversion;
}

RTCParsedData rtc_now(){
  send_reg_addr(REG_DATE, true);
  request(7);
  RTCParsedData reading, result;
  reading.t.s = TinyWireM.read();
  reading.t.m = TinyWireM.read();
  reading.t.h = TinyWireM.read();
  reading.dt.dow = TinyWireM.read();
  reading.dt.d = TinyWireM.read();
  reading.dt.m = TinyWireM.read();
  reading.dt.y = TinyWireM.read();
  result.t = reading.t;
  result.dt = reading.dt;
  return result;
}

unsigned char* time_to_char(const ParsedTime* aTime){
  unsigned char* conversion = new unsigned char[TIME_STR_LEN];
  fill_char_buffer(aTime->h, conversion, 0);
  conversion[2] = TIME_FIELD_SEPARATOR;
  fill_char_buffer(aTime->m, conversion, 3);
  conversion[5] = TIME_FIELD_SEPARATOR;
  fill_char_buffer(aTime->s, conversion, 6);
  return conversion;
}

#endif
