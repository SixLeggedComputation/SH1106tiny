
#include "SH1106mini.h"
#include "SH1106fonts.h"

// constructor for I2C - we indicate i2cAddress
SH1106mini::SH1106mini(int i2cAddress) {
  myI2cAddress = i2cAddress;
}

void SH1106mini::init() {
  TinyWireM.begin();
  sendInitCommands();
  resetDisplay();
}

void SH1106mini::resetDisplay(void) {
  displayOff();
  clear();
  displayOn();
}

void SH1106mini::reconnect() {
   TinyWireM.begin();
}

void SH1106mini::rotate(void){
	sendCommand(0xA1);
}

void SH1106mini::displayOn(void) {
    sendCommand(0xaf);        //display on
}

void SH1106mini::displayOff(void) {
  sendCommand(0xae);          //display off
}

void SH1106mini::setContrast(char contrast) {
  sendCommand(0x81);
  sendCommand(contrast);
}

void SH1106mini::flipScreenVertically() {
  sendCommand(0xA0);              //SEGREMAP   //Rotate screen 180 deg
  sendCommand(SETCOMPINS);
  sendCommand(0x22);
  sendCommand(COMSCANINC);            //COMSCANDEC  Rotate screen 180 Deg
}

void SH1106mini::clear(void) {
    for (uint8_t page = 0; page<8; page++){
		sendCommand(PAGESTARTADDRESS | page);
		sendCommand(SETLOWCOLUMN | 2);
		sendCommand(SETHIGHCOLUMN | 0);
			
		// send as 16 bunches of 8 bytes data in one xmission
		for(uint8_t y=0; y<16; y++){
			TinyWireM.beginTransmission(myI2cAddress);
			TinyWireM.write(0x40);
			for (uint8_t w=0; w<8; w++) {
				TinyWireM.write(0x0);
			}
			TinyWireM.endTransmission();
		}
	}
}

void SH1106mini::drawBitmap(int x, int y, uint8_t *bitmap) {
	uint8_t  width = 8;
	
	if ( x < 0) x = 0;
	if (x > 119) x = 119;
	x += 2;
	if (y < 0) y = 0;
    if (y > 55) y = 55;	// constrain x,y to stop running over edge
	uint8_t page = y >> 3; // quicker divide by eight
	uint8_t posn = y & 7; // might be 0
	// Use union to pre-shift data
	union shifty
	{
	uint16_t intVar;
    uint8_t Bytes[2];
	};
	
	shifty newval[8]; // array of unions to hold shifted data
	
	for (uint16_t n = 0; n<8; n++){ // initialise array and shift by posn
		newval[n].intVar = 0; // don't assume it's clear
		newval[n].Bytes[0] = bitmap[n];
		newval[n].intVar = newval[n].intVar << posn;
	}
	// write first page (might be the only one, depending on posn)
	sendCommand(PAGESTARTADDRESS | page++); // increment in case we need another page
	sendCommand(SETLOWCOLUMN | ( x & 0xf));
	sendCommand(SETHIGHCOLUMN | ( (x>>4)& 0xf));

	// send 8 bytes of data in one go
	TinyWireM.beginTransmission(myI2cAddress);
	TinyWireM.write(0x40);
	for (uint8_t w=0; w<width; w++) {
		TinyWireM.write(newval[w].Bytes[0]);
	}
	TinyWireM.endTransmission();

	if (posn) { // true (non zero) so do another page
		sendCommand(PAGESTARTADDRESS | page);
		sendCommand(SETLOWCOLUMN | ( x & 0xf));
		sendCommand(SETHIGHCOLUMN | ( (x>>4)& 0xf));
		// send 8 bytes of data in one go
		TinyWireM.beginTransmission(myI2cAddress);
		TinyWireM.write(0x40);
		for (uint8_t w=0; w<width; w++) {
			TinyWireM.write(newval[w].Bytes[1]);
		}
		TinyWireM.endTransmission();
	}
}


void SH1106mini::MirrorHorizontal() {
	shchar_t* OperationBuffer = new shchar_t[8];
	
	for (uint8_t i = 0, j=7; i < 8; i++, j--) {
		OperationBuffer[i] = DisplayBuffer[j];
	}
	
	memcpy(DisplayBuffer, OperationBuffer,8);
	delete[] OperationBuffer;
}

void SH1106mini::MirrorVertical() {
	shchar_t* OperationBuffer = new shchar_t[8];
	
	for (int i = 0; i < 8; i++) {
		OperationBuffer[i] = 0; // clear to start
		
		for (int j = 0; j < 8; j++) {
		  OperationBuffer[i] |= (((DisplayBuffer[i] >> j) & 1) << (7-j));
		}
	}
	
	memcpy(DisplayBuffer, OperationBuffer,8);
	delete[] OperationBuffer;
}


void SH1106mini::LoadCharFromFont(const shchar_t * bitmap){
	for(uint8_t i = 0; i<8; i++){
		DisplayBuffer[i] = pgm_read_byte(bitmap + i);
	}
}

const shchar_t* SH1106mini::rotateBuffer(){
   shchar_t* rotated = new shchar_t[8];
   
   for(uint8_t i = 0; i<8; i++){
	   rotated[i]=0;
   }
  
   uint8_t OldBit;
  
   for (uint8_t y = 0; y < 8; y++){
	   for(uint8_t x = 0; x < 8; x++){
	 	  OldBit = (DisplayBuffer[y] >> x) & 1;
		  
	 	  if(OldBit){
			  rotated[x] |= (1 << y);
		  }
	   }
   }
  
   return rotated;	
}

shchar_t SH1106mini::PrintableChar(const shchar_t c){
    return (c >= 0x20 && c<=0x7f) ? c : UNPRINTABLE;	
}

void SH1106mini::MakeBitmap(const shchar_t c, angle_t angle){
	LoadCharFromFont(font8x8[PrintableChar(c) - 0x20]);
			
	switch(angle){
		case angle_t::R0:
			// R0 does nothing. this writing just avoids -Wswitch being sent by compiler
			break;
			
		case angle_t::R90:
			UpdateBufferWithRotated();
			break;
		    
		case angle_t::R180:
		    MirrorHorizontal();
		    break;
		    
		case angle_t::R270:
  		    UpdateBufferWithRotated();
		    MirrorHorizontal();
		    break;
	}
}


void SH1106mini::UpdateBufferWithRotated(){
    const shchar_t* rotated = rotateBuffer();
    memcpy(DisplayBuffer, rotated, 8);
    delete[] rotated;		
}

void SH1106mini::printAt (int y, String s, int alignment, angle_t angle){
	int x;
	// string length must be less or equal to screen width
	// otherwise abandon
	if (s.length()<= 16) { 
		switch (alignment) {
			case LEFT:
				x = 0;
				break;
			case CENTRE:
				x = (128 - (s.length() << 3)) >> 1; // x = (128 - string width) / 2
				break;
			case RIGHT:
				x = (128 - (s.length() << 3)); // x = (128 - string width)
				break;
			default:
				break;
		}

		for (unsigned int k = 0; k < s.length(); k++) {
			MakeBitmap(s.charAt(k), angle);
			drawBitmap(x, y, DisplayBuffer);
			x += 8;
		}
	}
}
	
void SH1106mini::sendCommand(shchar_t com) {
   TinyWireM.beginTransmission(myI2cAddress);      //begin transmitting
   TinyWireM.write(0x80);                          //command mode
   TinyWireM.write(com);
   TinyWireM.endTransmission();                    // stop transmitting
  
}

void SH1106mini::sendInitCommands(void) {
  sendCommand(DISPLAYOFF);
  sendCommand(NORMALDISPLAY);
  sendCommand(SETDISPLAYCLOCKDIV);
  sendCommand(0x80);
  sendCommand(SETMULTIPLEX);
  sendCommand(0x3F);
  sendCommand(SETDISPLAYOFFSET);
  sendCommand(0x00);
  sendCommand(SETSTARTLINE | 0x00);
  sendCommand(CHARGEPUMP);
  sendCommand(0x14);
  sendCommand(MEMORYMODE);
  sendCommand(0x00);
  sendCommand(SEGREMAP);
  sendCommand(COMSCANDEC);
  sendCommand(SETCOMPINS);
  sendCommand(0x12);
  sendCommand(SETCONTRAST);
  sendCommand(0xCF);
  sendCommand(SETPRECHARGE);
  sendCommand(0xF1);
  sendCommand(SETVCOMDETECT);
  sendCommand(0x40);
  sendCommand(DISPLAYALLON_RESUME);
  sendCommand(NORMALDISPLAY);
  sendCommand(0x2e);            // stop scroll
  sendCommand(DISPLAYON);
}
