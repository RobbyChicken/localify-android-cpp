#include "android_ui.h"
#include <android/log.h>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cmath>
#include <algorithm>

#define LOG_TAG "LocalifyUI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace localify {

std::unique_ptr<LocalifyApp> g_app = nullptr;

// OpenGL shader sources
const char* vertexShaderSource = R"(
attribute vec2 aPosition;
attribute vec4 aColor;
varying vec4 vColor;
uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
    vColor = aColor;
}
)";

const char* fragmentShaderSource = R"(
precision mediump float;
varying vec4 vColor;

void main() {
    gl_FragColor = vColor;
}
)";

// Button implementation
Button::Button(const Rect& bounds, const std::string& text) 
    : UIComponent(bounds), text(text), textColor(Color::Black()), pressed(false) {
    backgroundColor = Color::LightGray();
}

void Button::draw() {
    if (!visible) return;
    
    // Draw button background
    Color bgColor = pressed ? Color::LocalifyPink() : backgroundColor;
    g_app->drawRect(bounds, bgColor);
    
    // Draw button text (centered)
    float textX = bounds.x + bounds.width / 2.0f - text.length() * 4.0f; // Rough centering
    float textY = bounds.y + bounds.height / 2.0f + 8.0f;
    g_app->drawText(text, textX, textY, textColor);
}

bool Button::handleTouch(const TouchEvent& event) {
    if (!visible || !enabled) return false;
    
    bool wasPressed = pressed;
    
    if (event.action == AMOTION_EVENT_ACTION_DOWN) {
        if (bounds.contains(event.x, event.y)) {
            pressed = true;
            return true;
        }
    } else if (event.action == AMOTION_EVENT_ACTION_UP) {
        if (pressed && bounds.contains(event.x, event.y)) {
            if (onClickCallback) {
                onClickCallback();
            }
        }
        pressed = false;
        return wasPressed;
    }
    
    return false;
}

// SearchBar implementation
SearchBar::SearchBar(const Rect& bounds, const std::string& placeholder)
    : UIComponent(bounds), placeholder(placeholder), textColor(Color::Black()), focused(false) {
    backgroundColor = Color::White();
}

void SearchBar::draw() {
    if (!visible) return;
    
    // Draw search bar background
    Color bgColor = focused ? Color::White() : Color::LightGray();
    g_app->drawRect(bounds, bgColor);
    
    // Draw border
    Rect borderRect(bounds.x - 1, bounds.y - 1, bounds.width + 2, bounds.height + 2);
    g_app->drawRect(borderRect, Color::Gray());
    
    // Draw text or placeholder
    std::string displayText = text.empty() ? placeholder : text;
    Color displayColor = text.empty() ? Color::Gray() : textColor;
    
    float textX = bounds.x + 10.0f;
    float textY = bounds.y + bounds.height / 2.0f + 8.0f;
    g_app->drawText(displayText, textX, textY, displayColor);
}

bool SearchBar::handleTouch(const TouchEvent& event) {
    if (!visible || !enabled) return false;
    
    if (event.action == AMOTION_EVENT_ACTION_DOWN) {
        focused = bounds.contains(event.x, event.y);
        return focused;
    }
    
    return false;
}

void SearchBar::setText(const std::string& newText) {
    text = newText;
    if (onTextChangedCallback) {
        onTextChangedCallback(text);
    }
}

// ListView implementation
ListView::ListView(const Rect& bounds, float itemHeight)
    : UIComponent(bounds), selectedIndex(-1), scrollOffset(0.0f), itemHeight(itemHeight) {
    backgroundColor = Color::White();
}

void ListView::draw() {
    if (!visible) return;
    
    // Draw list background
    g_app->drawRect(bounds, backgroundColor);
    
    // Calculate visible items
    int startIndex = std::max(0, (int)(scrollOffset / itemHeight));
    int endIndex = std::min((int)items.size(), startIndex + (int)(bounds.height / itemHeight) + 2);
    
    // Draw items
    for (int i = startIndex; i < endIndex; ++i) {
        float itemY = bounds.y + (i * itemHeight) - scrollOffset;
        if (itemY + itemHeight < bounds.y || itemY > bounds.y + bounds.height) continue;
        
        Rect itemRect(bounds.x, itemY, bounds.width, itemHeight);
        
        // Draw item background
        Color itemBg = (i == selectedIndex) ? Color::LocalifyPink() : Color::White();
        g_app->drawRect(itemRect, itemBg);
        
        // Draw item text
        Color textColor = (i == selectedIndex) ? Color::White() : Color::Black();
        g_app->drawText(items[i], itemRect.x + 15.0f, itemRect.y + itemHeight / 2.0f + 8.0f, textColor);
        
        // Draw separator line
        if (i < items.size() - 1) {
            Rect separatorRect(bounds.x, itemY + itemHeight - 1, bounds.width, 1);
            g_app->drawRect(separatorRect, Color::LightGray());
        }
    }
}

bool ListView::handleTouch(const TouchEvent& event) {
    if (!visible || !enabled) return false;
    
    if (event.action == AMOTION_EVENT_ACTION_DOWN && bounds.contains(event.x, event.y)) {
        // Calculate which item was touched
        float relativeY = event.y - bounds.y + scrollOffset;
        int itemIndex = (int)(relativeY / itemHeight);
        
        if (itemIndex >= 0 && itemIndex < items.size()) {
            selectedIndex = itemIndex;
            if (onItemClickCallback) {
                onItemClickCallback(selectedIndex);
            }
        }
        return true;
    }
    
    return false;
}

void ListView::setItems(const std::vector<std::string>& newItems) {
    items = newItems;
    selectedIndex = -1;
    scrollOffset = 0.0f;
}

void ListView::addItem(const std::string& item) {
    items.push_back(item);
}

void ListView::clearItems() {
    items.clear();
    selectedIndex = -1;
    scrollOffset = 0.0f;
}

// Screen base class implementation
void Screen::draw() {
    for (auto& component : components) {
        component->draw();
    }
}

bool Screen::handleTouch(const TouchEvent& event) {
    // Handle components in reverse order (top to bottom)
    for (auto it = components.rbegin(); it != components.rend(); ++it) {
        if ((*it)->handleTouch(event)) {
            return true;
        }
    }
    return false;
}

void Screen::update(float deltaTime) {
    for (auto& component : components) {
        component->update(deltaTime);
    }
}

void Screen::addComponent(std::unique_ptr<UIComponent> component) {
    components.push_back(std::move(component));
}

// LocalifyApp implementation
LocalifyApp::LocalifyApp(ANativeActivity* activity) 
    : activity(activity), window(nullptr), display(EGL_NO_DISPLAY), 
      surface(EGL_NO_SURFACE), context(EGL_NO_CONTEXT),
      width(0), height(0), initialized(false), running(false),
      currentScreenType(LOGIN_SCREEN) {
}

LocalifyApp::~LocalifyApp() {
    shutdown();
}

bool LocalifyApp::initialize() {
    if (initialized) return true;
    
    LOGI("Initializing Localify App");
    
    // Window will be set via setWindow() method when available
    if (!window) {
        LOGI("Window not yet available, will initialize when window is created");
        initialized = true; // Mark as initialized, EGL will be set up when window is available
        return true;
    }
    
    width = ANativeWindow_getWidth(window);
    height = ANativeWindow_getHeight(window);
    
    LOGI("Window size: %dx%d", width, height);
    
    if (!initializeEGL()) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    
    // Initialize OpenGL state
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    createScreens();
    
    initialized = true;
    running = true;
    
    LOGI("Localify App initialized successfully");
    return true;
}

void LocalifyApp::shutdown() {
    if (!initialized) return;
    
    LOGI("Shutting down Localify App");
    
    screens.clear();
    currentScreen.reset();
    
    shutdownEGL();
    
    initialized = false;
    running = false;
}

void LocalifyApp::setWindow(ANativeWindow* newWindow) {
    LOGI("Setting window: %p", newWindow);
    window = newWindow;
    
    if (window && initialized) {
        width = ANativeWindow_getWidth(window);
        height = ANativeWindow_getHeight(window);
        LOGI("Window size updated: %dx%d", width, height);
        
        // Initialize EGL if not already done
        if (display == EGL_NO_DISPLAY) {
            initializeEGL();
        }
    }
}

bool LocalifyApp::initializeEGL() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }
    
    if (!eglInitialize(display, nullptr, nullptr)) {
        LOGE("eglInitialize failed");
        return false;
    }
    
    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig failed");
        return false;
    }
    
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return false;
    }
    
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return false;
    }
    
    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("eglMakeCurrent failed");
        return false;
    }
    
    return true;
}

void LocalifyApp::shutdownEGL() {
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }
        
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }
        
        eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }
}

void LocalifyApp::createScreens() {
    // Create all screens
    screens.resize(5);
    // We'll implement these in screens.cpp
    
    // Start with login screen
    navigateToScreen(LOGIN_SCREEN);
}

void LocalifyApp::navigateToScreen(ScreenType screenType) {
    currentScreenType = screenType;
    
    if (screenType < screens.size() && screens[screenType]) {
        currentScreen = std::move(screens[screenType]);
        currentScreen->initialize();
    }
}

void LocalifyApp::render() {
    if (!initialized || !currentScreen) return;
    
    clearScreen(Color::White());
    
    currentScreen->draw();
    
    eglSwapBuffers(display, surface);
}

void LocalifyApp::update(float deltaTime) {
    if (!initialized || !currentScreen) return;
    
    currentScreen->update(deltaTime);
}

void LocalifyApp::clearScreen(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void LocalifyApp::drawRect(const Rect& rect, const Color& color) {
    if (!initialized || display == EGL_NO_DISPLAY) return;
    
    // Simplified rectangle drawing for OpenGL ES 2.0
    // In a full implementation, this would use shaders
    
    // For now, just clear the screen with the color if it's a full-screen rect
    if (rect.width >= width && rect.height >= height) {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    // TODO: Implement proper shader-based rectangle rendering
    // This is a placeholder implementation
}

void LocalifyApp::drawText(const std::string& text, float x, float y, const Color& color) {
    // Simple text rendering - in a real implementation you'd use a proper text rendering library
    // For now, this is a placeholder that would draw simple bitmap text
    LOGI("Drawing text: %s at (%.1f, %.1f)", text.c_str(), x, y);
}

void LocalifyApp::handleInput(AInputEvent* event) {
    if (!currentScreen) return;
    
    int32_t eventType = AInputEvent_getType(event);
    
    if (eventType == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event);
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        
        TouchEvent touchEvent(x, y, action);
        currentScreen->handleTouch(touchEvent);
    }
}

void LocalifyApp::handleCommand(int32_t cmd) {
    switch (cmd) {
        case 1: // APP_CMD_INIT_WINDOW
            LOGI("Window initialized command received");
            // Window will be set via setWindow() method
            break;
        case 2: // APP_CMD_TERM_WINDOW  
            LOGI("Window terminated command received");
            setWindow(nullptr);
            break;
        case 10: // APP_CMD_GAINED_FOCUS
            LOGI("App gained focus");
            running = true;
            break;
        case 11: // APP_CMD_LOST_FOCUS
            LOGI("App lost focus");
            running = false;
            break;
        default:
            LOGI("Unhandled command: %d", cmd);
            break;
    }
}

} // namespace localify
