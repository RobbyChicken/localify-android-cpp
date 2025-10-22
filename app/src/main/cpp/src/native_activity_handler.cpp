#include "native_activity_handler.h"
#include "app_config.h"
#include <android/log.h>
#include <jni.h>

#define LOG_TAG "LocalifyNativeActivity"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace localify {

NativeActivityHandler::NativeActivityHandler(ANativeActivity* activity)
    : activity(activity), window(nullptr), config(nullptr), assetManager(nullptr),
      initialized(false), focused(false), visible(false) {
    
    LOGI("Creating NativeActivityHandler");
    
    if (activity) {
        assetManager = activity->assetManager;
        setupAssetManager();
    }
}

NativeActivityHandler::~NativeActivityHandler() {
    LOGI("Destroying NativeActivityHandler");
}

void NativeActivityHandler::onCreate() {
    LOGI("NativeActivity onCreate");
    
    // Get configuration
    config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, assetManager);
    
    // Request necessary permissions
    requestPermissions();
    
    initialized = true;
}

void NativeActivityHandler::onStart() {
    LOGI("NativeActivity onStart");
    visible = true;
}

void NativeActivityHandler::onResume() {
    LOGI("NativeActivity onResume");
    focused = true;
}

void NativeActivityHandler::onPause() {
    LOGI("NativeActivity onPause");
    focused = false;
}

void NativeActivityHandler::onStop() {
    LOGI("NativeActivity onStop");
    visible = false;
}

void NativeActivityHandler::onDestroy() {
    LOGI("NativeActivity onDestroy");
    
    if (config) {
        AConfiguration_delete(config);
        config = nullptr;
    }
    
    initialized = false;
    focused = false;
    visible = false;
}

void NativeActivityHandler::onNativeWindowCreated(ANativeWindow* window) {
    LOGI("NativeWindow created");
    this->window = window;
    
    if (window) {
        int32_t width = ANativeWindow_getWidth(window);
        int32_t height = ANativeWindow_getHeight(window);
        int32_t format = ANativeWindow_getFormat(window);
        
        LOGI("Window dimensions: %dx%d, format: %d", width, height, format);
        
        // Set window format for optimal rendering
        ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
    }
}

void NativeActivityHandler::onNativeWindowDestroyed() {
    LOGI("NativeWindow destroyed");
    window = nullptr;
}

void NativeActivityHandler::onNativeWindowResized(ANativeWindow* window) {
    LOGI("NativeWindow resized");
    
    if (window) {
        int32_t width = ANativeWindow_getWidth(window);
        int32_t height = ANativeWindow_getHeight(window);
        LOGI("New window dimensions: %dx%d", width, height);
    }
}

void NativeActivityHandler::onNativeWindowRedrawNeeded(ANativeWindow* window) {
    LOGI("NativeWindow redraw needed");
    // This will trigger a render in the main loop
}

void NativeActivityHandler::onWindowFocusChanged(bool hasFocus) {
    LOGI("Window focus changed: %s", hasFocus ? "gained" : "lost");
    focused = hasFocus;
}

void NativeActivityHandler::onConfigurationChanged() {
    LOGI("Configuration changed");
    
    if (config && assetManager) {
        AConfiguration_fromAssetManager(config, assetManager);
        
        // Log configuration details
        int32_t orientation = AConfiguration_getOrientation(config);
        int32_t density = AConfiguration_getDensity(config);
        
        LOGI("New orientation: %d, density: %d", orientation, density);
    }
}

void NativeActivityHandler::onInputQueueCreated(AInputQueue* queue) {
    LOGI("Input queue created");
    // Input queue will be handled by the main application loop
}

void NativeActivityHandler::onInputQueueDestroyed(AInputQueue* queue) {
    LOGI("Input queue destroyed");
}

void NativeActivityHandler::onContentRectChanged(const ARect* rect) {
    if (rect) {
        LOGI("Content rect changed: (%d,%d) - (%d,%d)", 
             rect->left, rect->top, rect->right, rect->bottom);
    }
}

void NativeActivityHandler::onLowMemory() {
    LOGI("Low memory warning received");
    // Could implement memory cleanup here
}

void NativeActivityHandler::setupAssetManager() {
    if (!assetManager) {
        LOGE("AssetManager is null");
        return;
    }
    
    LOGI("AssetManager setup completed");
    
    // Test asset access
    AAssetDir* assetDir = AAssetManager_openDir(assetManager, "");
    if (assetDir) {
        const char* filename;
        while ((filename = AAssetDir_getNextFileName(assetDir)) != nullptr) {
            LOGI("Found asset: %s", filename);
        }
        AAssetDir_close(assetDir);
    }
}

void NativeActivityHandler::requestPermissions() {
    LOGI("Requesting permissions");
    
    // In a full implementation, you would use JNI to request permissions
    // For now, we'll just log what permissions we need
    
    LOGI("Required permissions:");
    LOGI("  - %s", AppConfig::Permissions::INTERNET);
    LOGI("  - %s", AppConfig::Permissions::ACCESS_NETWORK_STATE);
    LOGI("  - %s", AppConfig::Permissions::ACCESS_FINE_LOCATION);
    LOGI("  - %s", AppConfig::Permissions::ACCESS_COARSE_LOCATION);
    
    // In a real app, you'd call something like:
    // JNIEnv* env = activity->env;
    // jclass activityClass = env->GetObjectClass(activity->clazz);
    // jmethodID requestPermissions = env->GetMethodID(activityClass, "requestPermissions", "([Ljava/lang/String;I)V");
    // ... etc
}

} // namespace localify
