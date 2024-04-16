#include <stdlib.h>
#include <stdio.h>

#include "miniaudio.h"

ma_result onOpen(ma_vfs* pVFS, const char* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile) {
  char* mode = (openMode & MA_OPEN_MODE_WRITE) ? "wb" : "rb";
  FILE* f = fopen(pFilePath, mode);
  *pFile = f;
  return MA_SUCCESS;
}

ma_result onClose(ma_vfs* pVFS, ma_vfs_file file) {
  fclose(file);
  return MA_SUCCESS;
}

ma_result onRead(ma_vfs* pVFS, ma_vfs_file file, void* pDst, size_t sizeInBytes, size_t* pBytesRead) {
  *pBytesRead = fread(pDst, 1, sizeInBytes, file);
  return MA_SUCCESS;
}

ma_result onInfo(ma_vfs* pVFS, ma_vfs_file file, ma_file_info* pInfo) {
  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);
  pInfo->sizeInBytes = fsize;
  return MA_SUCCESS;
}

int playback_main(char* input_file)
{
    ma_result result;
    ma_engine engine;

    ma_vfs_callbacks vfs = {
        .onOpen = onOpen,
        .onClose = onClose,
        .onRead = onRead,
        .onInfo = onInfo,
    };

    ma_resource_manager_config config = ma_resource_manager_config_init();
    config.pVFS = &vfs;

    ma_resource_manager resourceManager;
    result = ma_resource_manager_init(&config, &resourceManager);
    if (result != MA_SUCCESS) {
      return result;
    }

    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.pResourceManager = &resourceManager;

    result = ma_engine_init(&engineConfig, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.");
        return -1;
    }

    ma_sound sound;

    result = ma_sound_init_from_file(&engine, input_file, 0, NULL, NULL, &sound);
    if (result != MA_SUCCESS) {
          return result;
    }

    ma_sound_start(&sound);

    fprintf(stderr, "Press Enter to quit...");
    getchar();

    ma_sound_stop(&sound);
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

    fprintf(stderr, "Press Enter to stop recording...\n");
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
