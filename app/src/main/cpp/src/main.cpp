#include <android/native_activity.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <unistd.h>

#define LOG_TAG "LocalifyMain"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Simple state structure
struct AppState {
    bool running;
    bool initialized;
};

// Handle app commands
static void handle_cmd(struct android_app* app, int32_t cmd) {
    AppState* state = (AppState*)app->userData;
    
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW - Window available");
            if (app->window != nullptr) {
                state->initialized = true;
                LOGI("Localify app initialized successfully");
            }
            break;
            
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW - Window destroyed");
            state->initialized = false;
            break;
            
        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS - App gained focus");
            state->running = true;
            break;
            
        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS - App lost focus");
            state->running = false;
            break;
            
        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY - App destroying");
            state->running = false;
            break;
            
        default:
            LOGI("Unhandled command: %d", cmd);
            break;
    }
}

// Handle input events
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    LOGI("Input event received");
    return 1; // Event handled
}

// Main application entry point
void android_main(struct android_app* app) {
    LOGI("=== Starting Localify Android C++ App ===");
    
    // Initialize app state
    AppState state = {false, false};
    app->userData = &state;
    
    // Set up callbacks
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    
    LOGI("App callbacks set up, entering main loop");
    
    // Main event loop
    while (true) {
        int events;
        struct android_poll_source* source;
        
        // Poll for events (blocking when not running, non-blocking when running)
        int timeout = state.running ? 0 : -1;
        
        while (ALooper_pollAll(timeout, nullptr, &events, (void**)&source) >= 0) {
            // Process the event
            if (source != nullptr) {
                source->process(app, source);
            }
            
            // Check if we are exiting
            if (app->destroyRequested != 0) {
                LOGI("Destroy requested, exiting main loop");
                return;
            }
        }
        
        // Simple "rendering" - just log that we're running
        if (state.running && state.initialized) {
            // In a real app, this is where you'd render frames
            // For now, just sleep to prevent excessive logging
            usleep(16667); // ~60 FPS
        }
    }
    
    LOGI("=== Localify Android C++ App Exited ===");
}
