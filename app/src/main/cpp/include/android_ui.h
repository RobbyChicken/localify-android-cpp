#ifndef LOCALIFY_ANDROID_UI_H
#define LOCALIFY_ANDROID_UI_H

#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <android/input.h>
#include <android/window.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace localify {

// Forward declarations
class UIComponent;
class Screen;
class Button;
class ListView;
class SearchBar;

// Color structure
struct Color {
    float r, g, b, a;
    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) 
        : r(r), g(g), b(b), a(a) {}
    
    // Localify brand colors
    static Color LocalifyPink() { return Color(0.91f, 0.12f, 0.39f, 1.0f); } // #E91E63
    static Color White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    static Color Black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    static Color Gray() { return Color(0.5f, 0.5f, 0.5f, 1.0f); }
    static Color LightGray() { return Color(0.9f, 0.9f, 0.9f, 1.0f); }
};

// Rectangle structure
struct Rect {
    float x, y, width, height;
    Rect(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f)
        : x(x), y(y), width(width), height(height) {}
    
    bool contains(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

// Touch event structure
struct TouchEvent {
    float x, y;
    int action; // AMOTION_EVENT_ACTION_DOWN, UP, MOVE
    TouchEvent(float x = 0.0f, float y = 0.0f, int action = 0) : x(x), y(y), action(action) {}
};

// Base UI Component class
class UIComponent {
protected:
    Rect bounds;
    Color backgroundColor;
    bool visible;
    bool enabled;
    
public:
    UIComponent(const Rect& bounds = Rect()) 
        : bounds(bounds), backgroundColor(Color::White()), visible(true), enabled(true) {}
    virtual ~UIComponent() = default;
    
    virtual void draw() = 0;
    virtual bool handleTouch(const TouchEvent& event) { return false; }
    virtual void update(float deltaTime) {}
    
    void setBounds(const Rect& newBounds) { bounds = newBounds; }
    Rect getBounds() const { return bounds; }
    
    void setBackgroundColor(const Color& color) { backgroundColor = color; }
    void setVisible(bool vis) { visible = vis; }
    void setEnabled(bool en) { enabled = en; }
    
    bool isVisible() const { return visible; }
    bool isEnabled() const { return enabled; }
};

// Button component
class Button : public UIComponent {
private:
    std::string text;
    Color textColor;
    std::function<void()> onClickCallback;
    bool pressed;
    
public:
    Button(const Rect& bounds, const std::string& text = "");
    
    void draw() override;
    bool handleTouch(const TouchEvent& event) override;
    
    void setText(const std::string& newText) { text = newText; }
    void setTextColor(const Color& color) { textColor = color; }
    void setOnClick(std::function<void()> callback) { onClickCallback = callback; }
};

// Search Bar component
class SearchBar : public UIComponent {
private:
    std::string text;
    std::string placeholder;
    Color textColor;
    bool focused;
    std::function<void(const std::string&)> onTextChangedCallback;
    
public:
    SearchBar(const Rect& bounds, const std::string& placeholder = "Search...");
    
    void draw() override;
    bool handleTouch(const TouchEvent& event) override;
    
    void setText(const std::string& newText);
    std::string getText() const { return text; }
    void setPlaceholder(const std::string& newPlaceholder) { placeholder = newPlaceholder; }
    void setOnTextChanged(std::function<void(const std::string&)> callback) { onTextChangedCallback = callback; }
};

// List View component
class ListView : public UIComponent {
private:
    std::vector<std::string> items;
    int selectedIndex;
    float scrollOffset;
    float itemHeight;
    std::function<void(int)> onItemClickCallback;
    
public:
    ListView(const Rect& bounds, float itemHeight = 60.0f);
    
    void draw() override;
    bool handleTouch(const TouchEvent& event) override;
    
    void setItems(const std::vector<std::string>& newItems);
    void addItem(const std::string& item);
    void clearItems();
    int getSelectedIndex() const { return selectedIndex; }
    void setOnItemClick(std::function<void(int)> callback) { onItemClickCallback = callback; }
};

// Screen base class
class Screen {
protected:
    std::vector<std::unique_ptr<UIComponent>> components;
    std::string title;
    
public:
    Screen(const std::string& title = "") : title(title) {}
    virtual ~Screen() = default;
    
    virtual void initialize() = 0;
    virtual void draw();
    virtual bool handleTouch(const TouchEvent& event);
    virtual void update(float deltaTime);
    
    void addComponent(std::unique_ptr<UIComponent> component);
    void setTitle(const std::string& newTitle) { title = newTitle; }
    std::string getTitle() const { return title; }
};

// Main Application class
class LocalifyApp {
private:
    ANativeActivity* activity;
    ANativeWindow* window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    
    int32_t width, height;
    bool initialized;
    bool running;
    
    std::unique_ptr<Screen> currentScreen;
    std::vector<std::unique_ptr<Screen>> screens;
    
public:
    enum ScreenType {
        LOGIN_SCREEN = 0,
        HOME_SCREEN = 1,
        SEARCH_SCREEN = 2,
        FAVORITES_SCREEN = 3,
        PROFILE_SCREEN = 4,
        MAP_SCREEN = 5
    };
    
    ScreenType currentScreenType;
    
    void drawRect(const Rect& rect, const Color& color);
    void drawText(const std::string& text, float x, float y, const Color& color = Color::Black());
    
    LocalifyApp(ANativeActivity* activity);
    ~LocalifyApp();
    
    bool initialize();
    void shutdown();
    
    void setWindow(ANativeWindow* window);
    void handleInput(AInputEvent* event);
    void handleCommand(int32_t cmd);
    
    void render();
    void update(float deltaTime);
    
    void navigateToScreen(ScreenType screenType);
    
    // Getters
    int32_t getWidth() const { return width; }
    int32_t getHeight() const { return height; }
    bool isRunning() const { return running; }
    
private:
    bool initializeEGL();
    void shutdownEGL();
    void createScreens();
    
    // OpenGL helper methods
    void clearScreen(const Color& color = Color::White());
};

// Global app instance
extern std::unique_ptr<LocalifyApp> g_app;

} // namespace localify

#endif // LOCALIFY_ANDROID_UI_H
