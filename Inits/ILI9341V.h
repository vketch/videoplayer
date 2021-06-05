
#ifndef MBED_ILI9341V_H
#define MBED_ILI9341V_H



#include "mbed.h"
#include "TFT.h"
#include <chrono>
#include <ratio>

/** Class for ILI9341V (squared screen 240x240) tft display controller
* to be copypasted and adapted for other controllers
*/
class ILI9341V : public TFT
{
 
 public:
   
    /** Create an SPI display interface
    * @param displayproto SPI_8 or SPI_16
    * @param Hz SPI speed in Hz
    * @param mosi SPI pin
    * @param miso SPI pin
    * @param sclk SPI pin
    * @param CS pin connected to CS of display
    * @param reset pin connected to RESET of display
    * @param DC pin connected to data/command of display
    * @param name The name used by the parent class to access the interface
    */ 
    ILI9341V(proto_t displayproto, int Hz, PinName mosi, PinName miso, PinName sclk, PinName CS, PinName reset, PinName DC, const char* name);
  
    /** Set the window, which controls where items are written to the screen.
    * When something hits the window width, it wraps back to the left side
    * and down a row. If the initial write is outside the window, it will
    * be captured into the window when it crosses a boundary.
    * @param x is the left edge in pixels.
    * @param y is the top edge in pixels.
    * @param w is the window width in pixels.
    * @param h is the window height in pixels.
    */
    virtual void window(int x, int y, int w, int h);
    

    /** Set the orientation of the screen
    *  x,y: 0,0 is always top left 
    *
    * @param o direction to use the screen (0-3)
    * 0 = default 0° portrait view
    * 1 = +90° landscape view
    * 2 = +180° portrait view
    * 3 = -90° landscape view
    *
    */  
    virtual void set_orientation(int o);

    void move(int x, int y, int w, int h, unsigned char *bitmap, unsigned short shift=0);
    int BMP_16(int x, int y, const char *Name_BMP);   

protected:
    
    
    /** Init command sequence  
    */
    void init();
    void init_old();


private:

    int row_start_shift = 0;  
    int page_start_shift = 0;  

};
#endif