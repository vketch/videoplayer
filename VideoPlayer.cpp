
#include "VideoPlayer.h" 


using namespace std;
using namespace std::chrono;

VideoPlayer::VideoPlayer(ILI9341V *lcd):
    _lcd(lcd)
{    
    _stop_play = true;
}

VideoPlayer::~VideoPlayer(){};


void VideoPlayer::stop(){
    _stop_play = true;
}

bool VideoPlayer::play(File& video_file, uint32_t fps){
    debug("start play video\n");
    _stop_play = false;

    int Width = _lcd->sizeX();
    int Heigh = _lcd->sizeX();

    size_t LineSize = Width * sizeof(unsigned short);
    size_t LinesInFrame = Heigh;      // number of lines we read for one frame
    
    const size_t FrameSize = (LineSize) * LinesInFrame;
    unsigned char *frame = (unsigned char *)malloc(FrameSize); // we need a buffer for the frame
    if (frame == NULL) {
        error("Can't allocate %d bytes of memory for the buf", FrameSize);
        return false;
    }

    size_t NumberBytes;
    _lcd->window(0, 0, Width, Heigh);


    milliseconds frame_ms{1000/fps};
    milliseconds delta_ms{0}; //time we are back or forward from expected 
            
    Timer time, total_time;
    time.start();
    total_time.start();
    uint32_t total_frames=0;    
    uint32_t skip_frames=0;
    uint32_t delay_frames=0;    
    while(!_stop_play){    
        time.reset();
        
        if ( delta_ms > frame_ms ) // if we play to fast, we  play the same  frame
        {
            ThisThread::sleep_for(frame_ms);
            delta_ms -= frame_ms;
            delay_frames++;
            //debug("play the same frame %d\n", frame_number);
        }
        else {
            NumberBytes = video_file.read(frame, FrameSize);
            if(NumberBytes != FrameSize) // end of video
                break;
            total_frames++;

            if ( delta_ms > -frame_ms ) // we play not to slow, we disply the frame to LCD
            {
                for (int i = 0; i < FrameSize; i += 2) // sqap bytes as we pass bitmap as unsinged short
                    std::swap(frame[i], frame[i + 1]); // !ToDo configure LCD for proper byte order 
                for (int line = 0; line < LinesInFrame; line++) // write to LCD line by line
                    _lcd->window_pushpixelbuf( (unsigned short *)(frame + (LineSize) * line), Width);                
            }
            else{
                //debug("skip frame %d\n", frame_number);
                skip_frames++;
            }
            delta_ms += frame_ms - duration_cast<milliseconds>(time.elapsed_time());            
        }        
    }
    total_time.stop();
    
    debug("video takes=%llu, total_frame=%d, skip frame %d, delay_frame_number = %d\n", 
        duration_cast<seconds>(total_time.elapsed_time()).count(), total_frames, skip_frames, delay_frames);
    
    free (frame);

    // printf("t1= %lld t2=%lld", time1, time2);    
    if(_lcd->auto_up) 
        _lcd->copy_to_lcd();

    return true;
}
