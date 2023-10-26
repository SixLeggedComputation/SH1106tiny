# SH1106mini

This is SH1106mini library, that was originally posted by samlewis02, 
with some bugs fixed and a few features added.

This library handles I2C communication between a ATTiny85 master and SH1106 based 128x64 pixel OLED display. 
There is no screen buffer, but a 1 character buffer instead, so as to fit binaries into 
ATTiny85 memory.

This documentation is based on samlewis02's original one, with some new entries added to it.

This library should exist in your Arduino/libraries folder.

## Dependencies
TinyWireM

## Disclaimer

This version of the library is being tested at the moment. Some bugs may have gone undetected.

## Usage

The Spirit level example shows how to use the graphics capability of the library.

## Features

Font is stored in prog memory as an array of 8 x 8 bitmaps, which encodes characters in range \u0020 up to \u007f.
Other characters are therefore unprintable and rendered as a '?' sign. Feel free to change it for an other.

An 8 x 8 bitmap (or font) can be drawn at any valid location within the display area. Note that if the bitmap does not fall on a 
vertical 8-bit boundary then two 8-bit writes are made. This may overwrite already written locations. 
When writing strings to the display there will be corruption if lines are too close.

In this version, methods have been added, which allow character flipping or rotation before it is rendered independantly from screen rotation.

## Changes

Here is a list of changes from original version.
 
Original version made use of char type. ATTiny implements signed char by 
default. This is an issue with font definition as it involves 0xFF.
Character type was changed from char to unsigned char. 
 
In function SH1106mini::drawBitmap():
 - removed yield() calls, which are meaningless for single-tasked ATTiny
 - removed pgm_read_byte_near call from this line 
 newval[n].Bytes[0] = bitmap[n] : bitmap was a function argument. It pointing
 to prog memory was not guaranteed. Original pgm_read_byte_near call was causing this library 
 to load random values into bitmap.
 
Dropped original method SH1106mini::flipChar (  const char * bitmap).
It was not atomic. There was a mess with indices. 
pgm_read_byte was actually picking values from several bitmaps at a time. 
This bug yielded unreadable outputs. It is replaced with MirrorVertical().

Now characters are loaded from font array by LoadCharFromFont, which does just that. 
Transformations can be applied to the bitmap at later stage using 
MirrorVertical(), MirrorHorizontal(), rotateBuffer() alone or 
in combination.
 
 in function void SH1106mini::clear(void)
 - removed yield() call
 
