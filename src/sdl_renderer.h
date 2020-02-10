#ifndef _SDL_RENDERER_H_
#define _SDL_RENDERER_H_

#include <string>

namespace sdlrenderer
{
    
class SDLRenderer
{
public:
    enum AudioFormat {
        AUDIO_FORMAT_U16 = 0,
        AUDIO_FORMAT_S16,
        AUDIO_FORMAT_U32,
        AUDIO_FORMAT_S32,
    };

    enum VideoFormat {
        VIDEO_FORMAT_YV12 = 0,
        VIDEO_FORMAT_IYUV,
        VIDEO_FORMAT_YUY2,
        VIDEO_FORMAT_UYVY,
        VIDEO_FORMAT_YVYU,
        VIDEO_FORMAT_NV12,
        VIDEO_FORMAT_NV21,
    };

    class AudioCallback 
    {
    public:
        virtual ~AudioCallback() = default;
        virtual int OnReadAudioData(unsigned char* buffer, int size) = 0; 
    };
    
    typedef long SDLVideoRenderer;
    
public:
    static bool Init();
    static void Exit();

    static SDLRenderer* CreateSDLRenderer();
    virtual ~SDLRenderer() = default;

    virtual bool CreateAudioPlayer(int samplerate, int channels, AudioFormat format, int samples, AudioCallback* callback) = 0;
    virtual void SetAudioPlayPaused(bool paused) = 0;
    virtual bool DestroyAudioPlayer() = 0;

    virtual SDLVideoRenderer CreateVideoPlayer(const std::string& title, int width, int height, VideoFormat format) = 0;
    virtual bool RenderVideoFrame(SDLVideoRenderer renderer, unsigned char* buffer, int width, int height) = 0;
    virtual void DestroyVideoPlayer(SDLVideoRenderer renderer) = 0;
};

}

#endif // _SDL_RENDERER_H_