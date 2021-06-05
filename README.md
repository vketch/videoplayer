# The video player based on UniGrapic 
The video plaer based on library https://os.mbed.com/teams/GraphicsDisplay/code/UniGraphic/ to play raw(B5G6R5) video. 

The UniGrapich Library is extended to support 240x240 LCD on ILI9341V driver - 
https://www.panelook.com/NMLCD-220Q33-2-2-inch-320x320-160nits-39pins-16-bit-RGB-I-F-With-RTP-TN-LCD-detail_83363.html

The player plays video on the above LCD only. But it is easy to improve/extend the player to work with other LCDs the UniGraphic contains.

The playerallows you to ask which FPS you want to play with. In case there are no capabilities to play with such FPS the player skips frames periodically to keep the timeline properly.  

The UniGrapich Library is extended to support "Fast SPI" that utilises HAL directly as an SPI interface the mbed-os provides is to slow to play video with FPS the video looks smoothly. Even the interface is called SPI_DMA  but actually it does not support DMA yet. Btw it is much faster the mbed SPI. 

The player expects raw video on the file system that has to be fast enought to achive FPS you ask. 

In tried it with SD card connected over SDIO interface.
The SDIO driver is not supported by mbed from the box. 
I use the NUCLEO-F412ZG with solution based on https://github.com/JojoS62/COMPONENT_SDIO 

# Example of program for  NUCLEO-F412ZG
```
#include "mbed.h"
#include "SDIOBlockDevice.h"
#include "FATFileSystem.h"
#include "VideoPlayer.h"

DigitalOut led1(LED1);

// File system on SD card connected over SDIO
static SDIOBlockDevice bd;
BlockDevice *BlockDevice::get_default_instance() { return &bd; }
BlockDevice *get_other_blockdevice() { return &bd; }
BlockDevice *blockDevice = BlockDevice::get_default_instance();
FATFileSystem fs("sd", blockDevice);

//LCD
DigitalOut selectSPI(D4, true); // Select SPI iterface on LCD
ILI9341V myLCD(SPI_DMA_, 10000000, D11, D12, D13, D10, D8, D7, "myLCD");

int main()
{    
    myLCD.raw_video("/fs/raw_video.raw", 20 );
    while (true) {
        ThisThread::sleep_for(500ms);
        led1 = !led1;    
    }
}

```

