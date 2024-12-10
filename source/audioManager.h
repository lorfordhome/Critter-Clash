
#include "miniaudio.h"
#pragma once

class AudioManager {
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

public:
    ~AudioManager() {
        Release();
    }
    void Release() {
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
    }
    void Initialise();
    void PlayMusic(const char* filePath);
    //void PlaySFX(const char* filePath);
    void StopMusic();
};