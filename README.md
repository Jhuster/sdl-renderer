# SDL-renderer

A pcm & yuv raw data render library written with C++ based on SDL2.  The Mac platform is supported now and the other platforms will be supported later.

## Dependence

Please download & install [SDL2](http://www.libsdl.org/) first. 

You should install the `SDL2.framework` into `/Library/Frameworks` on Mac OS platform. 

## Build 

```shell
$ ./build.sh
```
The output files will be export to `output` dir.

## Usage

### 1. Audio Player

```c++
// first define a class inherit from `SDLRenderer::AudioCallback`
class AudioSource : public SDLRenderer::AudioCallback
{
public:
    // when the audio driver need audio data, the callback will be triggered
    int OnReadAudioData(unsigned char* buffer, int size) override
    {
        // you should copy the audio pcm data to the `buffer` with `size` avalable space
        // and return the actual avalable data size
    }
}

int main(int argc, char* argv[])
{
    SDLRenderer::Init();
  
    // create sdl renderer
    auto renderer = SDLRenderer::CreateSDLRenderer();
  
    // create audio player
    AudioSource source;
    renderer->CreateAudioPlayer(samplerate, channels, format, samples, &source)
  
    // start audio play, the callback will be triggered later.
    renderer->SetAudioPlayPaused(false);
  
    // wait for audio playing
    while(!gExit);
  
    // destroy the audio player
    renderer->DestroyAudioPlayer();
      
    SDLRenderer::Exit();
  
    return 0;
}
```

### 2. Video Player

```c++
int main(int argc, char* argv[])
{
    SDLRenderer::Init
      
    // create sdl renderer
    auto renderer = SDLRenderer::CreateSDLRenderer();
  
    // create video player
    auto videoPlayer = renderer->CreateVideoPlayer("Window Title", videoWidth, videoHeight, SDLRenderer::VIDEO_FORMAT_IYUV);
    if (videoPlayer == 0) {
        return -1;
    }
  
    // create a YUV file reader
    VideoSource source;
  
    int size = videoWidth * videoHeight * 3 / 2;
    auto* buffer = new unsigned char[size];
  
    while (!gExit) {
        // read one yuv frame
        if (!source.ReadFrame(buffer, size)) {
            continue;
        }
        // render the yuv frame
        if (!renderer->RenderVideoFrame(videoPlayer, buffer, videoWidth, videoHeight)) {
            break;
        }
        usleep(30 * 1000);
    }
  
    // destroy the video player
    renderer->DestroyVideoPlayer(videoPlayer);
  
    SDLRenderer::Exit();
  
    return 0;
}
```

## Test

```shell
Usage: ./example.out [options]
  -a audio pcm file path [required]
  -v video yuv file path [required]
  -s audio samplerate, [default]: 16000
  -m audio format, [default]: 1
  -c audio channels, [default]: 1
  -n audio sample size, [default]: 1024
  -w video width, [default]: 416
  -h video height, [default]: 240
```

```shell
$ cd output
$ ./example.out -a data/speech_16k_16bit.pcm -s 16000 -m 1 -c 1 -n 1024 -v data/horses_416x240_30.yuv -w 416 -h 240
```


## Contracts

Email：[lujun.hust@gmail.com](mailto:lujun.hust@gmail.com)
