#include <android/native_activity.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <chrono>
#include "android_ui.h"
#include "screens.h"

#define LOG_TAG "LocalifyMain"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace localify;

// Global variables
static bool g_running = false;
static std::chrono::steady_clock::time_point g_lastTime;

// Handle app commands
static void handle_cmd(struct android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW");
            if (app->window != nullptr) {
                g_app = std::make_unique<LocalifyApp>(app->activity);
                if (g_app->initialize()) {
                    g_running = true;
                    g_lastTime = std::chrono::steady_clock::now();
                }
            }
            break;
            
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            g_running = false;
            if (g_app) {
                g_app->shutdown();
                g_app.reset();
            }
            break;
            
        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS");
            g_running = true;
            break;
            
        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS");
            g_running = false;
            break;
            
        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY");
            g_running = false;
            break;
    }
    
    if (g_app) {
        g_app->handleCommand(cmd);
    }
}

// Handle input events
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    if (g_app) {
        g_app->handleInput(event);
        return 1; // Event handled
    }
    return 0; // Event not handled
}

// Main application loop
void android_main(struct android_app* state) {
    LOGI("Starting Localify Android C++ App");
    
    // Set up app callbacks
    state->onAppCmd = handle_cmd;
    state->onInputEvent = handle_input;
    
    // Main loop
    while (true) {
        int events;
        struct android_poll_source* source;
        
        // Poll for events
        while (ALooper_pollAll(g_running ? 0 : -1, nullptr, &events, (void**)&source) >= 0) {
            if (source != nullptr) {
                source->process(state, source);
            }
            
            // Check if we are exiting
            if (state->destroyRequested != 0) {
                LOGI("Destroy requested, exiting");
                if (g_app) {
                    g_app->shutdown();
                    g_app.reset();
                }
                return;
            }
        }
        
        // Update and render if running
        if (g_running && g_app && g_app->isRunning()) {
            auto currentTime = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - g_lastTime).count();
            g_lastTime = currentTime;
            
            // Cap delta time to prevent large jumps
            if (deltaTime > 0.1f) {
                deltaTime = 0.1f;
            }
            
            g_app->update(deltaTime);
            g_app->render();
        }
    }
}
