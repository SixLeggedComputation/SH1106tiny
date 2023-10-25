#pragma once

/**
 * Fixed version of SH1106mini.h originally by samlewis02.
 * */

#include <Arduino.h>
#include <TinyWireM.h>

#define CHARGEPUMP 0x8D
#define COLUMNADDR 0x21
#define COMSCANDEC 0xC8
#define COMSCANINC 0xC0
#define DISPLAYALLON 0xA5
#define DISPLAYALLON_RESUME 0xA4
#define DISPLAYOFF 0xAE
#define DISPLAYON 0xAF
#define EXTERNALVCC 0x1
#define INVERTDISPLAY 0xA7
#define MEMORYMODE 0x20
#define NORMALDISPLAY 0xA6
#define PAGEADDR 0x22
#define PAGESTARTADDRESS 0xB0
#define SEGREMAP 0xA1
#define SETCOMPINS 0xDA
#define SETCONTRAST 0x81
#define SETDISPLAYCLOCKDIV 0xD5
#define SETDISPLAYOFFSET 0xD3
#define SETHIGHCOLUMN 0x10
#define SETLOWCOLUMN 0x00
#define SETMULTIPLEX 0xA8
#define SETPRECHARGE 0xD9
#define SETSEGMENTREMAP 0xA1
#define SETSTARTLINE 0x40
#define SETVCOMDETECT 0xDB
#define SWITCHCAPVCC 0x2
#define READMODIFYWRITE 0xe0
#define END 0xee
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
enum { 
	LEFT, 
	CENTRE, 
	RIGHT 
	};
	
enum angle_t{
	R0,
	R90,
	R180,
	R270};	
	
typedef unsigned char shchar_t;	

// Replacemant character
#define UNPRINTABLE 0x3F

class SH1106mini {

private:
    // I2C
    int myI2cAddress;
    
    // Stores the character bitmap to be displayed
   	shchar_t DisplayBuffer[8];
   	
   	// Exchanges Display buffer left and right pixels
   	void MirrorHorizontal();
   	
   	// Loads 1 character from font table, that is located in prog memory, into DisplayBuffer
   	void LoadCharFromFont(const shchar_t * bitmap);
   	
	// Exchanges bitmap top and bottom
	void MirrorVertical();
	
	// Checks if printed string characters match font bitmaps.
	// if so c is returned. Otherwise it is replaced with UNPRINTABLE
	shchar_t PrintableChar(const shchar_t c);
	
	/* rotates 1 character 90 degrees clockwise 
	 * 
	 * Takes its data from DisplayBuffer and produces a new array.
	 * Output depends on font definition */
	const shchar_t* rotateBuffer();
	
	/* Picks corresponding bitmap from font definition, stores it into DisplayBuffer, 
	 * then apply transformations, if any */
	void MakeBitmap(const shchar_t c, angle_t angle);
	
    /* Performs 90 degree counter-clockwise rotation.
	 * Transfers rotated bitmap into DisplayBuffer, then trashes local variables */
	void UpdateBufferWithRotated();

 public:
   // For I2C display: create the display object 
   SH1106mini(int i2cAddress);

   // Initialize the display
   void init();

   // Cycle through the initialization
   void resetDisplay(void);

   // Connect again to the display through I2C
   void reconnect(void);

   // Turn the display on
   void displayOn(void);

   // Turn the display offs
   void displayOff(void);

   // Clear the display buffer
   void clear(void);
   
   // rotates screen 
   // not to be condounded with character rotation
   void rotate(void);

   // Set display contrast
   void setContrast(char contrast);

   // Turn the display upside down
   void flipScreenVertically();

   // Send a command to the display (low level function)
   void sendCommand(shchar_t com);

   // Send all the init commands
   void sendInitCommands(void);

   // Write a  8 x 8 bitmap direct to display
   void drawBitmap(int x, int y, uint8_t *bitmap);
   
   // Write a string to display using alignment and characters rotation (not to be confounded with screen rotation)
   // String s must contain no character code less than 0x20. These codes will be replaced with '?'
   void printAt (int y, String s, int alignment, angle_t angle = R0);

   };
