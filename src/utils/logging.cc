#include <cstdio>
#ifdef _WIN32
#include <time.h>
#endif // _WIN32
#include <string>
#include "logging.h"

namespace sdlrenderer
{

#define LOG_TAG "SDL-Renderer"

static const char LOG_LEVEL_ARRAY[] = {'E', 'W', 'I', 'D', 'V'};
#define PRINT(str) printf("%s\n", str);

Logging Logging::_instance = Logging();

void Logging::Logout(void* ptr, int level, const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    if (_callback) {
        _callback(ptr, level, fmt, vl);
    }
    va_end(vl);
}

const char* Logging::GetDataStr()
{
    char buffer[4096];
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 4096, "%Y-%m-%d %I:%M:%S", timeinfo);
    return std::string(buffer).c_str();
}

void Logging::DefaultLoggingCallback(void* ptr, int level, const char* fmt, va_list vl)
{
    static const int LOG_BUFFER_LEN = 4096;
    char buffer[LOG_BUFFER_LEN] = {0};
    int ret = snprintf(buffer,
                       LOG_BUFFER_LEN,
                       "%c/%s %s ",
                       LOG_LEVEL_ARRAY[level],
                       GetDataStr(),
                       LOG_TAG);
    vsnprintf(buffer + ret, LOG_BUFFER_LEN - ret, fmt, vl);
    buffer[LOG_BUFFER_LEN - 1] = '\0';
    PRINT(buffer);
}

}
