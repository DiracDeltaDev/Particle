#ifndef _NOKIA_5110_H
#define _NOKIA_5110_H

#include "application.h"
#include "Adafruit_GFX.h"

#define BLACK 1
#define WHITE 0

#if !defined(VERTICAL_1)
#define VERTICAL_1	1
#endif
#if !defined(HORIZONTAL_1)
#define HORIZONTAL_1	2
#endif
#if !defined(VERTICAL_2)
#define VERTICAL_2	3
#endif
#if !defined(HORIZONTAL_2)
#define HORIZONTAL_2	4
#endif

class Nokia5110 : public Adafruit_GFX {
public:
    // Hardware SPI.
    Nokia5110(int dc, int cs, int rst, int led);
    ~Nokia5110();
    
    void begin(uint8_t contrast = 60, uint8_t bias = 0x04);
    
    void command(uint8_t c);
    
    void setContrast(uint8_t val);
    void setBackLight(uint8_t on_off);
    void clearDisplay(void);
    void display();
    
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    uint8_t getPixel(int8_t x, int8_t y);
    
private:
    int _dc, _rst, _cs, _led;
    uint8_t _rotation;
};


#endif
