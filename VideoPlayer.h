/* mbed  Video Player 
 */
#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include "mbed.h"
#include "FATFileSystem.h"
#include "ILI9341V.h"

#define DEFAULT_FPS          20

class VideoPlayer : private NonCopyable<VideoPlayer> {
public:

    VideoPlayer(ILI9341V *lcd);

    bool play(File& video_file, uint32_t fps);

    void stop();

    ~VideoPlayer();

private:
    //video
    ILI9341V *_lcd;
    bool _stop_play;
};

#endif
