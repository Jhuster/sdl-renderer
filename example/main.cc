#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <string>
#include "sdl_renderer.h"
#include "utils/logging.h"

using namespace sdlrenderer;

static constexpr int DEFAULT_AUDIO_SAMPLERATE = 16000;
static constexpr int DEFAULT_AUDIO_FORMAT = SDLRenderer::AUDIO_FORMAT_S16;
static constexpr int DEFAULT_AUDIO_CHANNELS = 1;
static constexpr int DEFAULT_AUDIO_SAMPLES = 1024;

static constexpr int DEFAULT_VIDEO_WIDTH = 416;
static constexpr int DEFAULT_VIDEO_HEIGHT = 240;

static volatile bool gExit = false;

static void ExitCallback(int signum)
{
    gExit = true;
    std::cout << "ExitCallback: " << signum << std::endl;
}

class AudioFileReader : public SDLRenderer::AudioCallback
{
public:
    explicit AudioFileReader(const std::string& filepath)
    {
        _file = fopen(filepath.c_str(), "rb");
        if (_file == nullptr) {
            LOGE("[AudioFileReader][%s] failed to open audio file: %s", __FUNCTION__, filepath.c_str());
            return;
        }
    }
    
    ~AudioFileReader() override 
    {
        fclose(_file);
    }
    
    bool IsFileOpened() const 
    {
        return _file != nullptr;
    }
    
    bool IsFileEOF() const 
    {
        return _file == nullptr || feof(_file);
    }
    
    int OnReadAudioData(unsigned char* buffer, int size) override
    {
        if (!IsFileOpened()) {
            LOGE("[AudioFileReader][%s] audio file not opened", __FUNCTION__);
            return -1;
        }
        if (IsFileEOF()) {
            fseek(_file, 0, SEEK_SET);
        }
        return fread(buffer, size, 1, _file);
    }

private:
    FILE * _file;
};

class VideoFileReader 
{
public:
    explicit VideoFileReader(const std::string& filepath)
    {
        _file = fopen(filepath.c_str(), "rb");
        if (_file == nullptr) {
            LOGE("[VideoFileReader][%s] failed to open video file: %s", __FUNCTION__, filepath.c_str());
            return;
        }
    }

    bool IsFileOpened() const
    {
        return _file != nullptr;
    }

    bool IsFileEOF() const
    {
        return _file == nullptr || feof(_file);
    }

    bool ReadFrame(void *buffer, int size)
    {
        if (!IsFileOpened()) {
            LOGE("[VideoFileReader][%s] video file not opened", __FUNCTION__);
            return -1;
        }
        
        if (IsFileEOF()) {
            fseek(_file, 0, SEEK_SET);
        }
        
        if (fread(buffer, size, 1, _file) != 1) {
            LOGE("[VideoFileReader][%s] failed to read data", __FUNCTION__);
            return false;
        }
        
        return true;
    }
    
    ~VideoFileReader()
    {
        fclose(_file);
    }

private:
    FILE * _file;
};

struct Parameter
{
    std::string audioFilepath;
    std::string videoFilepath;
    int audioSamplerate{DEFAULT_AUDIO_SAMPLERATE};
    int audioFormat{DEFAULT_AUDIO_FORMAT};
    int audioChannels{DEFAULT_AUDIO_CHANNELS};
    int audioSamples{DEFAULT_AUDIO_SAMPLES};
    int videoWidth{DEFAULT_VIDEO_WIDTH};
    int videoHeight{DEFAULT_VIDEO_HEIGHT};
};

static void usage()
{
    Parameter params;
    
    std::cout << "Usage: ./example.out [options]" << std::endl;
    std::cout << "  -a " << "audio pcm file path [required]"<< std::endl;
    std::cout << "  -v " << "video yuv file path [required]"<< std::endl;
    std::cout << "  -s " << "audio samplerate, [default]: " << params.audioSamplerate << std::endl;
    std::cout << "  -m " << "audio format, [default]: " << params.audioFormat << std::endl;
    std::cout << "  -c " << "audio channels, [default]: " << params.audioChannels << std::endl;
    std::cout << "  -n " << "audio sample size, [default]: " << params.audioSamples << std::endl;
    std::cout << "  -w " << "video width, [default]: " << params.videoWidth << std::endl;
    std::cout << "  -h " << "video height, [default]: " << params.videoHeight << std::endl;
    std::cout << std::endl;
}

static int parse(int argc, char* argv[], Parameter& parameter) 
{
    std::cout << "parse options: " << std::endl;

    std::string format = "a:v:s:m:c:n:w:h:";

    int c;
    while ((c = getopt(argc, argv, format.c_str())) != -1) {
        switch (c) {
            case 'a':
                parameter.audioFilepath = optarg;
                break;
            case 'v':
                parameter.videoFilepath = optarg;
                break;
            case 's':
                parameter.audioSamplerate = atoi(optarg);
                break;
            case 'm':
                parameter.audioFormat = atoi(optarg);
                break;
            case 'c':
                parameter.audioChannels = atoi(optarg);
                break;
            case 'n':
                parameter.audioSamples = atoi(optarg);
                break;
            case 'w':
                parameter.videoWidth = atoi(optarg);
                break;
            case 'h':
                parameter.videoHeight = atoi(optarg);
                break;
            default:
                usage();
                return -1;
        }
        std::cout << "  [opt]: -" << (char) c << " : " << optarg << std::endl;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, ExitCallback);
    
    SDLRenderer::Init();
    
    auto renderer = SDLRenderer::CreateSDLRenderer();
    if (renderer == nullptr) {
        return -1;
    }
    
    Parameter parameter;
    if (parse(argc, argv, parameter) < 0) {
        return -1;
    }
    
    AudioFileReader audioReader(parameter.audioFilepath);
    if (audioReader.IsFileOpened()) {
        if (!renderer->CreateAudioPlayer(parameter.audioSamplerate, parameter.audioChannels, (SDLRenderer::AudioFormat) parameter.audioFormat,
                                         parameter.audioSamples, &audioReader)) {
            return -1;
        }
        renderer->SetAudioPlayPaused(false);
    }
    
    VideoFileReader videoReader(parameter.videoFilepath);
    if (videoReader.IsFileOpened()) {
        auto videoPlayer = renderer->CreateVideoPlayer("Example", parameter.videoWidth, parameter.videoHeight);
        if (videoPlayer == 0) {
            return -1;
        }
        int size = parameter.videoWidth * parameter.videoHeight * 3 / 2;
        auto* buffer = new unsigned char[size];
        while (!gExit) {
            if (!videoReader.ReadFrame(buffer, size)) {
                continue;
            }
            if (!renderer->RenderVideoFrame(videoPlayer, buffer, parameter.videoWidth, parameter.videoHeight, SDLRenderer::VIDEO_FORMAT_IYUV)) {
                break;
            }
            usleep(30 * 1000);
        }
        renderer->DestroyVideoPlayer(videoPlayer);
    }

    while (!gExit);

    renderer->DestroyAudioPlayer();
    
    SDLRenderer::Exit();
    
    return 0;
}