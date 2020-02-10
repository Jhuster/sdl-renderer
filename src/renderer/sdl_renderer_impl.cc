#include "sdl_renderer_impl.h"
#include "utils/logging.h"

namespace sdlrenderer
{

static constexpr int SDL2_AUDIO_FORMATS[4] = {
    AUDIO_U16SYS, AUDIO_S16SYS, AUDIO_S32SYS, AUDIO_F32SYS 
};

static constexpr int SDL2_VIDEO_FORMATS[8] = {
    SDL_PIXELFORMAT_YV12,
    SDL_PIXELFORMAT_IYUV,
    SDL_PIXELFORMAT_YUY2,
    SDL_PIXELFORMAT_UYVY,
    SDL_PIXELFORMAT_YVYU,
    SDL_PIXELFORMAT_NV12,
    SDL_PIXELFORMAT_NV21,
    SDL_PIXELFORMAT_EXTERNAL_OES
};

bool SDLRenderer::Init()
{
    LOGI("[SDLRenderer][%s]", __FUNCTION__);
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
        LOGE("[SDLRenderer][%s] Could not initialize SDL: %s", __FUNCTION__, SDL_GetError());
        return false;
    }
    return true;
}

void SDLRenderer::Exit()
{
    LOGI("[SDLRenderer][%s]", __FUNCTION__);
    SDL_Quit();
}
  
SDLRenderer* SDLRenderer::CreateSDLRenderer()
{
    return new SDLRendererImpl();
}

SDLRendererImpl::SDLRendererImpl() : _callback(nullptr), _audioBuffer(nullptr)
{
    
}

SDLRendererImpl::~SDLRendererImpl()
{
    delete [] _audioBuffer;
}

bool SDLRendererImpl::CreateAudioPlayer(int samplerate, int channels, AudioFormat format, int samples, AudioCallback *callback) 
{
    if (_callback != nullptr) {
        LOGE("[SDLRenderer][%s] already created", __FUNCTION__);
        return false;
    }
    
    _callback = callback;
    
    SDL_AudioSpec spec;
    spec.freq = samplerate;
    spec.format = SDL2_AUDIO_FORMATS[format];
    spec.channels = channels;
    spec.silence = 0;
    spec.samples = samples;
    spec.callback = SDLRendererImpl::OnReadAudioData;
    spec.userdata = this;

    if (SDL_OpenAudio(&spec, nullptr) < 0) {
        LOGE("[SDLRenderer][%s] failed: %s", __FUNCTION__, SDL_GetError());
        return false;
    }
    
    return true;
}

void SDLRendererImpl::SetAudioPlayPaused(bool paused)
{
    SDL_PauseAudio(paused ? 1 : 0);
}
    
bool SDLRendererImpl::DestroyAudioPlayer() 
{
    SDL_CloseAudio();
    _callback = nullptr;
    return false;
}

void SDLRendererImpl::OnReadAudioData(void *userdata, unsigned char *stream, int len) 
{
    auto renderer = (SDLRendererImpl *) userdata;
    
    if (renderer->_callback == nullptr || len <= 0) {
        return;
    }
    
    if (renderer->_audioBuffer == nullptr) {
        renderer->_audioBuffer = new unsigned char[len];
    }

    renderer->_callback->OnReadAudioData(renderer->_audioBuffer, len);

    SDL_memset(stream, 0, len);
    SDL_MixAudio(stream, renderer->_audioBuffer, len, SDL_MIX_MAXVOLUME);
}

SDLRenderer::SDLVideoRenderer SDLRendererImpl::CreateVideoPlayer(const std::string& title, int width, int height, VideoFormat format) 
{
    auto renderer = new VideoRenderer();
    
    renderer->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     width , height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (renderer->window == nullptr) {
        LOGE("[SDLRenderer][%s] failed to create window: %s", __FUNCTION__, SDL_GetError());
        delete renderer;
        return (SDLVideoRenderer) 0;
    }

    renderer->renderer = SDL_CreateRenderer(renderer->window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer->renderer == nullptr) {
        LOGE("[SDLRenderer][%s] failed to create renderer: %s", __FUNCTION__, SDL_GetError());
        delete renderer;
        return (SDLVideoRenderer) 0;
    }

    renderer->texture = SDL_CreateTexture(renderer->renderer, SDL2_VIDEO_FORMATS[format], SDL_TEXTUREACCESS_STREAMING,
                                          width, height);
    if (renderer->renderer == nullptr) {
        LOGE("[SDLRenderer][%s] failed to create renderer: %s", __FUNCTION__, SDL_GetError());
        delete renderer;
        return (SDLVideoRenderer) 0;
    }

    LOGI("[SDLRenderer][%s] created %d x %d, format = %d", __FUNCTION__, width, height, format);
    
    return (SDLVideoRenderer) renderer;
}

void SDLRendererImpl::DestroyVideoPlayer(SDLRenderer::SDLVideoRenderer renderer) 
{
    LOGI("[SDLRenderer][%s] destroyed", __FUNCTION__);
    auto r = (VideoRenderer *) renderer;
    delete r;
}

bool SDLRendererImpl::RenderVideoFrame(SDLVideoRenderer renderer, unsigned char *buffer, int width, int height) 
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;
        }
    }
    
    auto r = (VideoRenderer *) renderer;
    if (r == nullptr) {
        LOGE("[SDLRenderer][%s] invalid renderer !", __FUNCTION__);
        return false;
    }
    
    SDL_UpdateTexture(r->texture, nullptr, buffer, width);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;
    
    SDL_RenderClear(r->renderer);
    SDL_RenderCopy(r->renderer, r->texture, nullptr, &rect);
    SDL_RenderPresent(r->renderer);

    LOGV("[SDLRenderer][%s] %d x %d", __FUNCTION__, width, height);
    
    return true;
}

};