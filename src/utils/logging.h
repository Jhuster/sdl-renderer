#ifndef _SDL_RENDERER_LOGGING_H_
#define _SDL_RENDERER_LOGGING_H_

#include <cstdarg>

namespace sdlrenderer
{

class Logging
{
public:
    enum LogLevel {
        LOG_ERROR = 0,
        LOG_WARNING,
        LOG_INFO,
        LOG_DEBUG,
        LOG_VERBOSE
    };

    typedef void (*Callback)(void* ptr, int level, const char* fmt, va_list vl);

    static Logging& Instance()
    {
        return _instance;
    }

    void SetLogCallback(Callback callback)
    {
        _callback = callback;
    }

    void Logout(void* ptr, int level, const char* fmt, ...);

private:
    Logging()
    {
        _callback = DefaultLoggingCallback;
    }

    ~Logging() = default;

private:
    static void DefaultLoggingCallback(void* ptr, int level, const char* fmt, va_list vl);
    static const char* GetDataStr();

private:
    Callback _callback;
    static Logging _instance;
};

#define LOGE(format, ...) sdlrenderer::Logging::Instance().Logout(nullptr, Logging::LOG_ERROR, format, ##__VA_ARGS__)
#define LOGW(format, ...) sdlrenderer::Logging::Instance().Logout(nullptr, Logging::LOG_WARNING, format, ##__VA_ARGS__)
#define LOGI(format, ...) sdlrenderer::Logging::Instance().Logout(nullptr, Logging::LOG_INFO, format, ##__VA_ARGS__)
#define LOGD(format, ...) sdlrenderer::Logging::Instance().Logout(nullptr, Logging::LOG_DEBUG, format, ##__VA_ARGS__)
#define LOGV(format, ...) sdlrenderer::Logging::Instance().Logout(nullptr, Logging::LOG_VERBOSE, format, ##__VA_ARGS__)

}

#endif // _SDL_RENDERER_LOGGING_H_