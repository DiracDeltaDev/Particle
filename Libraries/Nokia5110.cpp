/*
 file: Nokia5110.cpp
 brief: Nokia5110 functions
 */

#include "Nokia5110.h"

/* DIMENSIÓN */
#define LCDWIDTH 84
#define LCDHEIGHT 48

/* COMANDOS */
#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

uint8_t pcd8544_buffer[LCDWIDTH * LCDHEIGHT / 8];

Nokia5110::Nokia5110(int dc, int cs, int rst, int led) : Adafruit_GFX(LCDWIDTH, LCDHEIGHT) {
    _dc = dc;
    _rst = rst;
    _cs = cs;
    _led = led;
    _rotation = HORIZONTAL_1;
}

Nokia5110::~Nokia5110()
{
    
}


/* Draw Pixel *************************************************************/
void Nokia5110::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
        return;
    
    int16_t t;
    switch(_rotation){
        case 1:
            t = x;
            x = y;
            y =  LCDHEIGHT - 1 - t;
            break;
        case 2:
            x = LCDWIDTH - 1 - x;
            y = LCDHEIGHT - 1 - y;
            break;
        case 3:
            t = x;
            x = LCDWIDTH - 1 - y;
            y = t;
            break;
    }
    
    if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
        return;
    
    // x is which column
    if (color)
        pcd8544_buffer[x+ (y/8)*LCDWIDTH] |= (1 << (y%8));
    else
        pcd8544_buffer[x+ (y/8)*LCDWIDTH] &= ~ (1 << (y%8));
}
/***************************************************************************/

/* Begin ******************************************************************/
void Nokia5110::begin(uint8_t contrast, uint8_t bias){
    // configure control pins
    pinMode(_dc, OUTPUT);
    pinMode(_cs, OUTPUT);
    pinMode(_rst, OUTPUT);
    pinMode(_led, OUTPUT);
    
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    
    // reset the LCD to a known state
    digitalWrite(_rst, LOW);
    delay(500);
    digitalWrite(_rst, HIGH);
    
    // get into the EXTENDED mode!
    command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
    
    // LCD bias select (4 is optimal?)
    command(PCD8544_SETBIAS | bias);
    
    // set VOP
    if (contrast > 0x7f)    contrast = 0x7f;
    
    command( PCD8544_SETVOP | contrast); // Experimentally determined
    
    // normal mode
    command(PCD8544_FUNCTIONSET);
    
    // Set display to Normal
    command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
    
    display();
}
/***************************************************************************/

/* Command *****************************************************************/
void Nokia5110::command(uint8_t cmd){
    digitalWrite(_dc, LOW); /* Command mode */
    digitalWrite(_cs, LOW); /* Select lcd */
    SPI.transfer(cmd);
    digitalWrite(_cs, HIGH); /* Deselect lcd */
}

/* Contrast ******************************************************************/
void Nokia5110::setContrast(uint8_t val) {
    if (val > 0x7f) val = 0x7f;
    
    command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
    command( PCD8544_SETVOP | val);
    command(PCD8544_FUNCTIONSET);
}
/***************************************************************************/

/* BackLight ******************************************************************/
void Nokia5110::setBackLight(uint8_t on_off) {
    digitalWrite(_led, on_off);
}
/***************************************************************************/

/* Display *****************************************************************/
void Nokia5110::display(void) {
    uint8_t col, maxcol, p;
    
    for(p = 0; p < 6; p++) {
        
        command(PCD8544_SETYADDR | p);
        // start at the beginning of the row
        col = 0;
        maxcol = LCDWIDTH-1;
        
        command(PCD8544_SETXADDR | col);
        
        digitalWrite(_dc, HIGH); /* Command mode */
        digitalWrite(_cs, LOW); /* Select */
        for(; col <= maxcol; col++) {
            SPI.transfer(pcd8544_buffer[(LCDWIDTH*p)+col]);
        }
        digitalWrite(_cs, HIGH); /* Deselect */
        
    }
    
    command(PCD8544_SETYADDR );  // no idea why this is necessary but it is to finish the last byte?
    
}
/***************************************************************************/

/* Clear *******************************************************************/
void Nokia5110::clearDisplay(void) {
    memset(pcd8544_buffer, 0, LCDWIDTH*LCDHEIGHT/8);
    cursor_y = cursor_x = 0;
}
/***************************************************************************/

