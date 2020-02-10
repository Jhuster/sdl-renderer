#ifndef _SDL_RENDERER_IMPL_H_
#define _SDL_RENDERER_IMPL_H_

#include "sdl_renderer.h"
#include <SDL2/SDL.h>

namespace sdlrenderer
{
    
class SDLRendererImpl : public SDLRenderer
{
public:
    SDLRendererImpl();
    ~SDLRendererImpl() override;

    class VideoRenderer 
    {
    public:
        SDL_Window* window{nullptr};
        SDL_Renderer* renderer{nullptr};
        SDL_Texture * texture{nullptr};
        ~VideoRenderer()
        {
            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
        }
    };

public:
    bool CreateAudioPlayer(int samplerate, int channels, AudioFormat format, int samples, AudioCallback* callback) override;
    void SetAudioPlayPaused(bool paused) override;
    bool DestroyAudioPlayer() override;

    SDLVideoRenderer CreateVideoPlayer(const std::string& title, int width, int height, VideoFormat format) override;
    bool RenderVideoFrame(SDLVideoRenderer renderer, unsigned char* buffer, int width, int height) override;
    void DestroyVideoPlayer(SDLVideoRenderer renderer) override;
    
private:
    static void OnReadAudioData(void *userdata, unsigned char *stream, int len);

private:
    AudioCallback * _callback;
    unsigned char * _audioBuffer;
};

}

#endif // _SDL_RENDERER_IMPL_H_