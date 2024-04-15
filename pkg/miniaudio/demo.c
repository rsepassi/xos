#include <stdlib.h>
#include <stdio.h>

#include "miniaudio.h"

int playback_main(char* input_file)
{
    ma_result result;
    ma_engine engine;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.");
        return -1;
    }

    ma_engine_play_sound(&engine, input_file, NULL);

    printf("Press Enter to quit...");
    getchar();

    ma_engine_uninit(&engine);

    return 0;
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_encoder* pEncoder = (ma_encoder*)pDevice->pUserData;
    if (pEncoder == NULL) {
      printf("error: encoder missing");
      exit(1);
    }

    ma_encoder_write_pcm_frames(pEncoder, pInput, frameCount, NULL);

    (void)pOutput;
}

int capture_main(char* output_file)
{
    ma_result result;
    ma_encoder_config encoderConfig;
    ma_encoder encoder;
    ma_device_config deviceConfig;
    ma_device device;

    encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_f32, 2, 44100);

    if (ma_encoder_init_file(output_file, &encoderConfig, &encoder) != MA_SUCCESS) {
        printf("Failed to initialize output file.\n");
        return -1;
    }

    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format   = encoder.config.format;
    deviceConfig.capture.channels = encoder.config.channels;
    deviceConfig.sampleRate       = encoder.config.sampleRate;
    deviceConfig.dataCallback     = data_callback;
    deviceConfig.pUserData        = &encoder;

    result = ma_device_init(NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize capture device.\n");
        return -2;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&device);
        printf("Failed to start device.\n");
        return -3;
    }

    printf("Press Enter to stop recording...\n");
    getchar();
    
    ma_device_uninit(&device);
    ma_encoder_uninit(&encoder);

    return 0;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    printf("usage: demo input.wav output.wav");
    return -1;
  }

  if (playback_main(argv[1])) {
    printf("playback failed");
    return -1;
  }

  if (capture_main(argv[2])) {
    printf("capture failed");
    return -1;
  }

  if (playback_main(argv[2])) {
    printf("playback failed");
    return -1;
  }
}
