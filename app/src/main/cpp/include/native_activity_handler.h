#ifndef LOCALIFY_NATIVE_ACTIVITY_HANDLER_H
#define LOCALIFY_NATIVE_ACTIVITY_HANDLER_H

#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_window.h>

namespace localify {

// Native Activity Handler (replacing AndroidManifest.xml activity declaration)
class NativeActivityHandler {
private:
    ANativeActivity* activity;
    ANativeWindow* window;
    AConfiguration* config;
    AAssetManager* assetManager;
    
    bool initialized;
    bool focused;
    bool visible;
    
public:
    NativeActivityHandler(ANativeActivity* activity);
    ~NativeActivityHandler();
    
    // Activity lifecycle methods
    void onCreate();
    void onStart();
    void onResume();
    void onPause();
    void onStop();
    void onDestroy();
    
    // Window methods
    void onNativeWindowCreated(ANativeWindow* window);
    void onNativeWindowDestroyed();
    void onNativeWindowResized(ANativeWindow* window);
    void onNativeWindowRedrawNeeded(ANativeWindow* window);
    
    // Focus methods
    void onWindowFocusChanged(bool hasFocus);
    
    // Configuration methods
    void onConfigurationChanged();
    
    // Input methods
    void onInputQueueCreated(AInputQueue* queue);
    void onInputQueueDestroyed(AInputQueue* queue);
    
    // Content methods
    void onContentRectChanged(const ARect* rect);
    
    // Low memory
    void onLowMemory();
    
    // Getters
    ANativeActivity* getActivity() const { return activity; }
    ANativeWindow* getWindow() const { return window; }
    AAssetManager* getAssetManager() const { return assetManager; }
    bool isInitialized() const { return initialized; }
    bool isFocused() const { return focused; }
    bool isVisible() const { return visible; }
    
    // App metadata (replacing AndroidManifest.xml)
    struct AppMetadata {
        static constexpr const char* NATIVE_LIB_NAME = "localify";
        static constexpr bool DEBUGGABLE = true;
        static constexpr bool ALLOW_BACKUP = true;
        static constexpr int TARGET_SDK_VERSION = 34;
        static constexpr int MIN_SDK_VERSION = 24;
        
        // Intent filters
        static constexpr const char* MAIN_ACTION = "android.intent.action.MAIN";
        static constexpr const char* LAUNCHER_CATEGORY = "android.intent.category.LAUNCHER";
        static constexpr const char* VIEW_ACTION = "android.intent.action.VIEW";
        static constexpr const char* DEFAULT_CATEGORY = "android.intent.category.DEFAULT";
        static constexpr const char* BROWSABLE_CATEGORY = "android.intent.category.BROWSABLE";
        
        // OpenGL ES requirements
        static constexpr int GLES_VERSION_MAJOR = 2;
        static constexpr int GLES_VERSION_MINOR = 0;
        static constexpr bool GLES_REQUIRED = true;
    };
    
private:
    void setupAssetManager();
    void requestPermissions();
};

} // namespace localify

#endif // LOCALIFY_NATIVE_ACTIVITY_HANDLER_H
