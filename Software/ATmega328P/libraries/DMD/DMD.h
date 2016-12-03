#ifndef DMD_H_
#define DMD_H_

//Arduino toolchain header, version dependent
#include "Arduino.h"
#include <SPI.h>

// ATmega328p
#define PIN_DMD_A              19
#define PIN_DMD_B              18
#define PIN_DMD_LATCH          17
#define PIN_DMD_OE              3
#define PIN_BRIGHTNESS_ADJUST  A0

// Teensy
//#define PIN_DMD_LATCH           2
//#define PIN_DMD_A               3
//#define PIN_DMD_B               4
//#define PIN_DMD_OE              5
//#define PIN_BRIGHTNESS_ADJUST   A0

//Pixel/graphics writing modes (bGraphicsMode)
#define GRAPHICS_NORMAL    0
#define GRAPHICS_INVERSE   1

//display screen (and subscreen) sizing
#define DMD_PIXELS_ACROSS   32  //pixels across x axis
#define DMD_PIXELS_DOWN     16  //pixels down y axis
#define DMD_RAM_SIZE_BYTES  64  //32*16/8 = 64 bytes

//lookup table for DMD::writePixel to make the pixel indexing routine faster
static byte bPixelLookupTable[8] = {
   0x80,   //0, bit 7
   0x40,   //1, bit 6
   0x20,   //2. bit 5
   0x10,   //3, bit 4
   0x08,   //4, bit 3
   0x04,   //5, bit 2
   0x02,   //6, bit 1
   0x01    //7, bit 0
};

// Font Indices
#define FONT_LENGTH             0
#define FONT_FIXED_WIDTH        2
#define FONT_HEIGHT             3
#define FONT_FIRST_CHAR         4
#define FONT_CHAR_COUNT         5
#define FONT_WIDTH_TABLE        6

static byte brightnessLookupTable[256] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 29, 29, 30, 31, 31, 32, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 70, 71, 72, 73, 74, 75, 76, 77, 79, 80, 81, 82, 83, 85, 86, 87, 88, 90, 91, 92, 94, 95, 96, 98, 99, 100, 102, 103, 105, 106, 108, 109, 110, 112, 113, 115, 116, 118, 120, 121, 123, 124, 126, 128, 129, 131, 132, 134, 136, 138, 139, 141, 143, 145, 146, 148, 150, 152, 154, 155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181, 183, 185, 187, 189, 191, 193, 196, 198, 200, 202, 204, 207, 209, 211, 214, 216, 218, 220, 223, 225, 228, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252, 255};

//The main class of DMD library functions
class DMD{
  public:
    //Instantiate the DMD
    DMD(byte panelsWide, byte panelsHigh);

    //Set or clear a pixel at the x and y location (0,0 is the top left corner)
    void writePixel( unsigned int bX, unsigned int bY, byte bGraphicsMode, byte bPixel );

    //Draw a string
    void drawString( int bX, int bY, const char* bChars, byte length, byte bGraphicsMode);

    //Select a text font
    void selectFont(const uint8_t* font);

    //Draw a single character
    int drawChar(const int bX, const int bY, const unsigned char letter, byte bGraphicsMode);

    //Find the width of a character
    int charWidth(const unsigned char letter);

    //Draw a scrolling string
    void drawMarquee( const char* bChars, byte length, int left, int top);

    //Move the maquee accross by amount
    boolean stepMarquee( int amountX, int amountY);

    //Clear the screen in DMD RAM
    void clearScreen();

    //Draw or clear a line from x1,y1 to x2,y2
    void drawLine( int x1, int y1, int x2, int y2, byte bGraphicsMode );

    //Draw or clear a box(rectangle) with a single pixel border
    void drawBox( int x1, int y1, int x2, int y2, byte bGraphicsMode );

    //Draw or clear a filled box(rectangle) with a single pixel border
    void drawFilledBox( int x1, int y1, int x2, int y2, byte bGraphicsMode );

    //Scan the dot matrix LED panel display, from the RAM mirror out to the display hardware.
    //Call 4 times to scan the whole display which is made up of 4 interleaved rows within the 16 total rows.
    void scanDisplayBySPI();
    
    // set the brightness of the display (0~255) using a potentiometer
    void setBrightness();
    
    // set the brightness of the display by passing in a brightnessLevel (0-8)
    void setBrightness(int brightnessLevel);

  private:
    //Display information
    byte DisplaysWide;
    byte DisplaysHigh;
    byte DisplaysTotal;
    int DisplayBrightness;
    int DisplayBrightnessLevel;
    
    //Mirror of DMD pixels in RAM, ready to be clocked out
    byte *bDMDScreenRAM;
    
    //scanning pointer into bDMDScreenRAM
    volatile byte bDMDByte;
    
    //Pointer to current font
    const uint8_t* Font;
    
    //Marquee values
    char marqueeText[256];
    byte marqueeLength;
    int marqueeWidth;
    int marqueeHeight;
    int marqueeOffsetX;
    int marqueeOffsetY;
};

#endif /* DMD_H_ */