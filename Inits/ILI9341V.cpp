 /* mbed UniGraphic library - Device specific class
 * Copyright (c) 2015 Giuliano Dianda
 * Released under the MIT License: http://mbed.org/license/mit
 */
 
#include "Protocols.h"
#include "ILI9341V.h"
#include <chrono>

using namespace std::chrono;

// The number of pixels (320-240) to shif start of page in TFT frame memory for 240x240 displays. 
// As driver ILI9341 is for 240x320 display
const int PageStartShift = 80; 
const unsigned int LSDSizeX = 240;
const unsigned int LSDSizeY = 240;

ILI9341V::ILI9341V(proto_t displayproto, int Hz, PinName mosi, PinName miso, PinName sclk, PinName CS, PinName reset, PinName DC, const char *name)
    : TFT(displayproto, Hz, mosi, miso, sclk, CS, reset, DC, LSDSizeX, LSDSizeY, name)
{
    hw_reset(); //TFT class forwards to Protocol class
    BusEnable(true); //TFT class forwards to Protocol class

    //ToDo need impement read over LCD 
   // identify(); // will collect tftID and set mipistd flag
    init(); // per display custom init cmd sequence, implemented here
   // auto_gram_read_format();// try to get read gram pixel format, could be 16bit or 18bit, RGB or BGR. Will set flags accordingly
    set_orientation(0); //TFT class does for MIPI standard and some ILIxxx
    FastWindow(true); // most but not all controllers support this, even if datasheet tells they should. 
    cls();
    locate(0,0); 
    page_start_shift = row_start_shift = 0;
}


//reset and init the lcd controller
void ILI9341V::init()
{
    wr_cmd8(0x11);  // Sleep OUT
    ThisThread::sleep_for(150ms);
    wr_cmd8(0x36);  // Memory Access Control. Orientation 
    wr_data8(0x08);  
    
    wr_cmd8(0x3A);  // COLMOD: Pixel Format Set   
    wr_data8(0x55); // 0x55 - 16 bit for RGB and MPI

    wr_cmd8(0x21);  // Display Inversion ON
    
    // wr_cmd8(0xb2);  // Frame Rate Control (In Idle Mode/8 colors) (
    // wr_data8(0x05);
    // wr_data8(0x05);
    // wr_data8(0x00);
    // wr_data8(0x33);
    // wr_data8(0x33);
    
    // wr_cmd8(0xb7); // Entry Mode Set (B7h) Low voltage detection control, etc...
    // wr_data8(0x35); 

    //wr_cmd8(0xbb); // Backlight Control 4 
    //wr_data8(0x37);
   
    wr_cmd8(0xc0); // Power Control 1 . Set the GVDD level, which is a    reference level for the VCOM level and the grayscale voltage level.
    wr_data8(0x23); // was 2c  in original DataSheet  and has issue with inversion of picture and font. The changing it to 0x23  resolve the issue that is really surprised!!.   

    // wr_cmd8(0xc2);  // !!!Absent in datasheet
    // wr_data8(0x01);    

    // wr_cmd8(0xc3); // !!!Absent in datasheet
    // wr_data8(0x0f);

    // wr_cmd8(0xc4); // !!!Absent in datasheet
    // wr_data8(0x20);

    // wr_cmd8(0xc6); // !!!Absent in datasheet
    // wr_data8(0x10);
   
    wr_cmd8(0xd0); // NV Memory Write (D0h)
    wr_data8(0xa4);
    wr_data8(0xa1);
 
    wr_cmd8(0xe8); //Driver timing control A
    wr_data8(0x83);
   
    wr_cmd8(0xe9); //Driver timing control B
    wr_data8(0x09);
    wr_data8(0x09);
    wr_data8(0x08);
   
    wr_cmd8(0xe0); //Driver timing control C
    wr_data8(0xd0);
    wr_data8(0x05);
    wr_data8(0x09);
    wr_data8(0x09);
    wr_data8(0x08);
    wr_data8(0x14);
    wr_data8(0x28);
    wr_data8(0x33);
    wr_data8(0x3f);
    wr_data8(0x07);
    wr_data8(0x13);
    wr_data8(0x14);
    wr_data8(0x28);
    wr_data8(0x30);
   
    wr_cmd8(0xe1); //  Negative Gamma Correction
    wr_data8(0xd0);
    wr_data8(0x05); 
    wr_data8(0x09);
    wr_data8(0x09);
    wr_data8(0x08);
    wr_data8(0x03);
    wr_data8(0x24);
    wr_data8(0x32);
    wr_data8(0x3b);
    wr_data8(0x38);
    wr_data8(0x14);
    wr_data8(0x13);
    wr_data8(0x28);
    wr_data8(0x2f);
  
    wr_cmd8(0x29);  // display on
}

// color TFT can rotate in hw (swap raw<->columns) for landscape/portrate views
// WE tune the page and row start as TFT has a 320x240 GM even 
// size of screen is 240x240 only  
void ILI9341V::set_orientation(int o)
{
    orientation = o;
    wr_cmd8(0x36);
    switch (orientation) {
        case 0:// default, portrait view 0°
            wr_data8(0x40); //for some other ILIxxxx
            page_start_shift = row_start_shift = 0; 
            break;
        case 1:// landscape view +90°
            wr_data8(0x21);//for some other ILIxxxx
            page_start_shift = row_start_shift = 0;
            break;
        case 2:// portrait view +180°
            wr_data8(0x91);//for some other ILIxxxx
            page_start_shift = 0;
            row_start_shift = PageStartShift; 
            break;
        case 3:// landscape view -90°
            wr_data8(0xF0);//for some other ILIxxxx
            page_start_shift = PageStartShift;
            row_start_shift = 0;
            break;
    }
    set_width(screensize_X);
    set_height(screensize_Y);

}


// TFT have both column and raw autoincrement inside a window, with internal counters
void ILI9341V::window(int x, int y, int w, int h)
{
    fastwindowready=false; // end raw/column going to be set to lower value than bottom-right corner
    wr_cmd8(0x2A);
    wr_data16(x+row_start_shift);   //start column
    wr_data16(x+w-1+row_start_shift);//end column

    wr_cmd8(0x2B);
    wr_data16(y+page_start_shift);   //start page
    wr_data16(y+h-1+page_start_shift);//end page
    
    wr_cmd8(0x2C);  //write mem, just send pixels color next
}


void ILI9341V::move(int x, int y, int w, int h,unsigned char *bitmap, unsigned short shift)
{

    int  j;
    unsigned char  padd;
    unsigned short *bitmap_ptr = (unsigned short *)bitmap;    


    padd = w%2; // the lines are padded to multiple of 4 bytes in a bitmap
    if(x<0) x=0;
    else if(x>=oriented_width) return;
    if(y<0) y=0;
    else if(y>=oriented_height) return;
    int cropX = (x+w)-oriented_width;
    if(cropX<0) cropX=0;
    int cropY = (y+h)-oriented_height;
    if(cropY<0) cropY=0;
    window(x, y, w-cropX, h-cropY);
    bitmap_ptr += ((h - 1)* (w + padd)); // begin of last line in array (first line of image)(standard bmp scan direction is left->right bottom->top)
    shift = shift%w;
    for (j = 0; j < h-cropY-1; j++) {         //Lines
        window_pushpixelbuf(bitmap_ptr+shift, w-cropX);
        bitmap_ptr -= w+padd;
    }
    //if(auto_up) copy_to_lcd();
}


// show bmp file in 16bit RGB 565 format 
int ILI9341V::BMP_16(int x, int y, const char *Name_BMP)
{
    #define BMP_HEADER_SIZE     54
    #define OffsetPixelWidth    18
    #define OffsetPixelHeigh    22
    #define OffsetFileSize      34
    #define OffsetPixData       10
    #define OffsetBPP           28

    unsigned char BMP_Header[BMP_HEADER_SIZE];
    unsigned short BPP_t;
    int PixelWidth,PixelHeigh,start_data;
    unsigned int    i,off;
    int padd, j;


    FILE *Image = fopen(Name_BMP, "rb");  // open the bmp file
    if (!Image) {
        printf("Can open the %s file", Name_BMP);
        return -1;
    }
    fread(BMP_Header,1,BMP_HEADER_SIZE,Image);      // get the BMP Header

    if (BMP_Header[0] != 0x42 || BMP_Header[1] != 0x4D) {  // check magic byte
        fclose(Image);
        printf("%s is no the BMP file", Name_BMP);
        return -2; 
    }

    
    BPP_t = BMP_Header[OffsetBPP] + (BMP_Header[OffsetBPP + 1] << 8); 
    // check is it a 16 bit BMP
    if (BPP_t != 0x0010) {
        fclose(Image);
        printf("%s is no the 16 bit BMP file", Name_BMP);
        return -3; 
    }

    // check the size
    PixelHeigh = BMP_Header[OffsetPixelHeigh] + (BMP_Header[OffsetPixelHeigh + 1] << 8) + (BMP_Header[OffsetPixelHeigh + 2] << 16) + (BMP_Header[OffsetPixelHeigh + 3] << 24);
    PixelWidth = BMP_Header[OffsetPixelWidth] + (BMP_Header[OffsetPixelWidth + 1] << 8) + (BMP_Header[OffsetPixelWidth + 2] << 16) + (BMP_Header[OffsetPixelWidth + 3] << 24);

    //!ToDo the negative  value is about of orintation let ignore it for the MVP
    if(PixelHeigh < 0)
        PixelHeigh = (0-PixelHeigh);

    if (PixelHeigh > oriented_height + y || PixelWidth > oriented_width + x) {
        fclose(Image);
        printf("The BMP file is to big. PixelHeigh = %d,  PixelWidth=%d", PixelHeigh, PixelWidth);
        return -4;     
    }

    start_data = BMP_Header[OffsetPixData] + (BMP_Header[OffsetPixData + 1] << 8) + (BMP_Header[OffsetPixData + 2] << 16) + (BMP_Header[OffsetPixData + 3] << 24);

 
    size_t LineSize = PixelWidth * sizeof(unsigned short);
    // the bmp lines are padded to multiple of 4 bytes
    padd = 0;
    while( LineSize%4 != 0)
        padd += 2;

    size_t LinesInFrame = PixelHeigh;      // number of lines we read for one frame
    
    const size_t FrameSize = (LineSize + padd) * LinesInFrame;
    unsigned char *frame = (unsigned char *)malloc(FrameSize); // we need a buffer for the frame
    if (frame == NULL) {
        printf("Can't allocate %d bytes of memory for the buf", FrameSize);
        return -5;         // error no memory
    }
    // Timer t;
    // t.start();
    // long long time1=0, time2=0;
    window(x, y, PixelWidth, PixelHeigh);
    for (j = 0; j < PixelHeigh; j += LinesInFrame) {
      // t.reset();
      fread(frame, 1, FrameSize, Image); // read from storage
      for (int i = 0; i < FrameSize; i += 2) // sqap bytes as we pass bitmap as unsinged short
        std::swap(frame[i], frame[i + 1]);
      // time1 +=
      // std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count();
      // t.reset();
      for (int line = 0; line < LinesInFrame; line++) // write to LCD line by line
        window_pushpixelbuf( (unsigned short *)(frame + (LineSize + padd) * line), PixelWidth);
      // time2 +=
      // std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count();
      // window_pushpixelbuf((unsigned short *)(buf), PixelWidth*line_num;
    }
    free (frame);
    fclose(Image);

    // printf("t1= %lld t2=%lld", time1, time2);    
    if(auto_up) copy_to_lcd();
    return 0;
}


// show raw video 240x240 16bit RGB 565
int ILI9341V::raw_video(const char *file_name, uint32_t fps )
{
    FILE *Video = fopen(file_name, "rb");  // open the bmp file
    if (!Video) {
        printf("Can open the %s file", file_name);
        return -1;
    }

    int Width = LSDSizeX, Heigh = LSDSizeY;

    size_t LineSize = Width * sizeof(unsigned short);
    size_t LinesInFrame = Heigh;      // number of lines we read for one frame
    
    const size_t FrameSize = (LineSize) * LinesInFrame;
    unsigned char *frame = (unsigned char *)malloc(FrameSize); // we need a buffer for the frame
    if (frame == NULL) {
        printf("Can't allocate %d bytes of memory for the buf", FrameSize);
        return -5;         // error no memory
    }

    size_t NumberBytes;
    window(0, 0, Width, Heigh);


    milliseconds frame_ms{1000/fps};
    milliseconds delta_ms{0}; //time we are back or forward from expected 
            
    Timer time;            
    time.start();
    uint32_t frame_number=0;
    uint32_t skip_frames=0;    
    while(1){    
        time.reset();
        
        if ( delta_ms > frame_ms ) // if we play to fast, we  play the same  frame
        {
            ThisThread::sleep_for(frame_ms);
            delta_ms -= frame_ms;
            debug("play the same frame %d\n", frame_number);
            continue;
        }
        else {
            NumberBytes = fread(frame, 1, FrameSize, Video); // read from storage        
            if(NumberBytes != FrameSize) // end of video
            break;
            frame_number++;
            if ( delta_ms > -frame_ms ) // we play not to slow, we disply the frame to LCD
            {
                for (int i = 0; i < FrameSize; i += 2) // sqap bytes as we pass bitmap as unsinged short
                    std::swap(frame[i], frame[i + 1]);
                for (int line = 0; line < LinesInFrame; line++) // write to LCD line by line
                    window_pushpixelbuf( (unsigned short *)(frame + (LineSize) * line), Width);                
            }
            else{
                skip_frames++;
            }
            delta_ms += frame_ms - duration_cast<milliseconds>(time.elapsed_time());
        }        
    }

    free (frame);
    fclose(Video);
    debug("total frame%d, skip frame %d\n", frame_number, skip_frames);
    // printf("t1= %lld t2=%lld", time1, time2);    
    if(auto_up) 
        copy_to_lcd();
    return 0;
}

