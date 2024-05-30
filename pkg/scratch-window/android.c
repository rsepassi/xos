#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/log.h>

#include "android_native_app_glue.h"

int _xos_android_provide_native_window(void*, int32_t, int32_t);

#define LOG_TAG "NativeActivity"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

void doAndroidLog(const char* msg) {
	LOGI("%s", msg);
}

typedef struct {
	struct android_app* app;
} Ctx;

static int32_t ctxHandleInput(struct android_app* app, AInputEvent* event) {
  Ctx* ctx = (Ctx*)app->userData;
  return 0;
}

static void ctxTermDisplay(Ctx* ctx) {}

static void drawFrame() {}

static void fatal(const char* msg) {
  LOGE("%s", msg);
  abort();
}

static void ctxHandleCmd(struct android_app* app, int32_t cmd) {
  Ctx* ctx = (Ctx*)app->userData;
  switch (cmd) {
    case APP_CMD_INIT_WINDOW:
			LOGI("APP_CMD_INIT_WINDOW");
			LOGI("init display");
      // if (!ctxInitDisplay(ctx)) fatal("init display failed");
      ANativeWindow* window = ctx->app->window;
      if (_xos_android_provide_native_window(window, ANativeWindow_getWidth(window), ANativeWindow_getHeight(window)) != 0) fatal("zig init failed");
      break;
    case APP_CMD_START:
			LOGI("APP_CMD_START");
    case APP_CMD_RESUME:
			LOGI("APP_CMD_RESUME");
      break;
    case APP_CMD_GAINED_FOCUS:
			LOGI("APP_CMD_GAINED_FOCUS");
      break;
    case APP_CMD_TERM_WINDOW:
			LOGI("APP_CMD_TERM_WINDOW");
      ctxTermDisplay(ctx);
      break;
    case APP_CMD_INPUT_CHANGED:
    case APP_CMD_WINDOW_RESIZED:
    case APP_CMD_WINDOW_REDRAW_NEEDED:
    case APP_CMD_CONTENT_RECT_CHANGED:
    case APP_CMD_LOST_FOCUS:
    case APP_CMD_CONFIG_CHANGED:
    case APP_CMD_LOW_MEMORY:
    case APP_CMD_SAVE_STATE:
    case APP_CMD_PAUSE:
    case APP_CMD_STOP:
    case APP_CMD_DESTROY:
			LOGI("APP_CMD_ %d", cmd);
      break;
    default:
      break;
  }
}

void android_main(struct android_app* app) {
	LOGI("android_main");
	Ctx ctx = {0};
	app->userData = &ctx;
	app->onAppCmd = ctxHandleCmd;
	app->onInputEvent = ctxHandleInput;
	ctx.app = app;

	LOGI("android_main loop");
	while (!app->destroyRequested) {
    struct android_poll_source* source;
    int result = ALooper_pollOnce(-1, NULL, NULL, (void**)&source);
    assert(result != ALOOPER_POLL_ERROR);
    if (source != NULL) source->process(app, source);
  }

  ctxTermDisplay(&ctx);
}
