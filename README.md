# The video player based on UniGrapic 
The video plaer based on UniGrapich library https://github.com/vketch/UniGraphic

The player plays video on the ILI9341V LCD only. But it is easy to improve/extend the player to work with other LCDs the UniGraphic contains.

The player allows you to ask which FPS you want to play with. In case there are no capabilities to play with asked FPS the player skips frames periodically to keep the timeline properly.  

The player expects raw video on the file system that has to be fast enought to achive FPS you ask. 

In tried it with SD card connected over SDIO interface.
The SDIO driver is not supported by mbed from the box. 
I use the NUCLEO-F412ZG with solution based on https://github.com/JojoS62/COMPONENT_SDIO 

## Example of program for the NUCLEO-F412ZG
```
#include "mbed.h"
#include "SDIOBlockDevice.h"
#include "FATFileSystem.h"
#include "ILI9341V.h"
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
    VideoPlayer video_player(&myLCD);

    File file(&fs, "raw_video.raw");
    video_player.play(file, 21);

    while (true) {
        ThisThread::sleep_for(500ms);
        led1 = !led1;    
    }
}

```

