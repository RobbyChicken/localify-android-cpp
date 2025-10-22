#include "screens.h"
#include <android/log.h>
#include <cmath>

#define LOG_TAG "LocalifyMap"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace localify {

// MapScreen implementation - Canvas-based map following the reliable approach from memories
MapScreen::MapScreen() : Screen("Select Radius"), currentRadius(5.0f), dragging(false) {}

void MapScreen::initialize() {
    LOGI("Initializing Map Screen");
    
    components.clear();
    
    float screenWidth = g_app->getWidth();
    float screenHeight = g_app->getHeight();
    
    // Set center of map
    centerX = screenWidth / 2.0f;
    centerY = screenHeight / 2.0f;
    
    // Create back button
    backButton = std::make_unique<Button>(
        Rect(20.0f, 20.0f, 80.0f, 50.0f),
        "Back"
    );
    backButton->setBackgroundColor(Color::Gray());
    backButton->setTextColor(Color::White());
    backButton->setOnClick([this]() { onBack(); });
    
    // Create confirm button
    confirmButton = std::make_unique<Button>(
        Rect(screenWidth - 120.0f, 20.0f, 100.0f, 50.0f),
        "Confirm"
    );
    confirmButton->setBackgroundColor(Color::LocalifyPink());
    confirmButton->setTextColor(Color::White());
    confirmButton->setOnClick([this]() { onConfirm(); });
    
    // Add components
    addComponent(std::move(backButton));
    addComponent(std::move(confirmButton));
}

void MapScreen::draw() {
    if (!g_app) return;
    
    // Draw the custom map background first
    drawMap();
    
    // Draw the radius circle
    drawRadiusCircle();
    
    // Draw UI components on top
    Screen::draw();
    
    // Draw radius info text
    std::string radiusText = "Radius: " + std::to_string((int)currentRadius) + " miles";
    g_app->drawText(radiusText, 20.0f, g_app->getHeight() - 100.0f, Color::Black());
}

bool MapScreen::handleTouch(const TouchEvent& event) {
    // First check if UI components handled the touch
    if (Screen::handleTouch(event)) {
        return true;
    }
    
    // Handle radius adjustment by dragging
    if (event.action == AMOTION_EVENT_ACTION_DOWN) {
        float dx = event.x - centerX;
        float dy = event.y - centerY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Check if touch is near the radius circle edge (within 30 pixels)
        float radiusPixels = currentRadius * 10.0f; // Scale factor for display
        if (std::abs(distance - radiusPixels) < 30.0f) {
            dragging = true;
            return true;
        }
    } else if (event.action == AMOTION_EVENT_ACTION_MOVE && dragging) {
        float dx = event.x - centerX;
        float dy = event.y - centerY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Convert pixel distance to miles (with min/max constraints)
        float newRadius = distance / 10.0f; // Scale factor
        newRadius = std::max(1.0f, std::min(25.0f, newRadius)); // 1-25 miles range
        
        updateRadius(newRadius);
        return true;
    } else if (event.action == AMOTION_EVENT_ACTION_UP) {
        dragging = false;
        return true;
    }
    
    return false;
}

void MapScreen::drawMap() {
    if (!g_app) return;
    
    float screenWidth = g_app->getWidth();
    float screenHeight = g_app->getHeight();
    
    // Draw iOS-style map background (light gray)
    Color mapBackground(0.95f, 0.95f, 0.97f, 1.0f); // #F2F2F7 iOS style
    Rect mapRect(0.0f, 0.0f, screenWidth, screenHeight);
    g_app->drawRect(mapRect, mapBackground);
    
    // Draw grid pattern for streets (40px spacing as mentioned in memory)
    Color streetColor = Color::White();
    float gridSpacing = 40.0f;
    
    // Draw vertical streets
    for (float x = 0; x < screenWidth; x += gridSpacing) {
        Rect streetRect(x, 0.0f, 2.0f, screenHeight);
        g_app->drawRect(streetRect, streetColor);
    }
    
    // Draw horizontal streets
    for (float y = 0; y < screenHeight; y += gridSpacing) {
        Rect streetRect(0.0f, y, screenWidth, 2.0f);
        g_app->drawRect(streetRect, streetColor);
    }
    
    // Add some park areas (green patches) for realism
    Color parkColor(0.78f, 0.90f, 0.79f, 1.0f); // #C8E6C9 light green
    
    // Draw a few random park areas
    Rect park1(screenWidth * 0.2f, screenHeight * 0.3f, 80.0f, 60.0f);
    g_app->drawRect(park1, parkColor);
    
    Rect park2(screenWidth * 0.7f, screenHeight * 0.6f, 100.0f, 80.0f);
    g_app->drawRect(park2, parkColor);
    
    // Add water bodies (blue patches)
    Color waterColor(0.39f, 0.71f, 0.96f, 1.0f); // #64B5F6 light blue
    
    Rect water1(screenWidth * 0.1f, screenHeight * 0.7f, 120.0f, 40.0f);
    g_app->drawRect(water1, waterColor);
    
    // Draw city label with semi-transparent background (iOS style)
    Color labelBg(0.0f, 0.0f, 0.0f, 0.6f); // Semi-transparent black
    Rect labelRect(20.0f, screenHeight - 150.0f, 120.0f, 30.0f);
    g_app->drawRect(labelRect, labelBg);
    
    g_app->drawText("Your City", 25.0f, screenHeight - 135.0f, Color::White());
}

void MapScreen::drawRadiusCircle() {
    if (!g_app) return;
    
    // iOS system blue color (#007AFF) with transparency
    Color radiusColor(0.0f, 0.48f, 1.0f, 0.15f); // 15% transparency fill
    Color radiusStroke(0.0f, 0.48f, 1.0f, 1.0f); // Solid stroke
    
    float radiusPixels = currentRadius * 10.0f; // Scale factor for display
    
    // Draw filled circle (simplified as square for this implementation)
    Rect radiusRect(
        centerX - radiusPixels,
        centerY - radiusPixels,
        radiusPixels * 2,
        radiusPixels * 2
    );
    g_app->drawRect(radiusRect, radiusColor);
    
    // Draw stroke (border)
    float strokeWidth = 3.0f;
    
    // Top border
    Rect topBorder(centerX - radiusPixels, centerY - radiusPixels, radiusPixels * 2, strokeWidth);
    g_app->drawRect(topBorder, radiusStroke);
    
    // Bottom border
    Rect bottomBorder(centerX - radiusPixels, centerY + radiusPixels - strokeWidth, radiusPixels * 2, strokeWidth);
    g_app->drawRect(bottomBorder, radiusStroke);
    
    // Left border
    Rect leftBorder(centerX - radiusPixels, centerY - radiusPixels, strokeWidth, radiusPixels * 2);
    g_app->drawRect(leftBorder, radiusStroke);
    
    // Right border
    Rect rightBorder(centerX + radiusPixels - strokeWidth, centerY - radiusPixels, strokeWidth, radiusPixels * 2);
    g_app->drawRect(rightBorder, radiusStroke);
    
    // Draw center marker (iOS style: blue circle with white center)
    float markerRadius = 12.0f;
    
    // Outer blue circle
    Rect outerMarker(
        centerX - markerRadius,
        centerY - markerRadius,
        markerRadius * 2,
        markerRadius * 2
    );
    g_app->drawRect(outerMarker, radiusStroke);
    
    // Inner white dot
    float innerRadius = 6.0f;
    Rect innerMarker(
        centerX - innerRadius,
        centerY - innerRadius,
        innerRadius * 2,
        innerRadius * 2
    );
    g_app->drawRect(innerMarker, Color::White());
}

void MapScreen::onBack() {
    LOGI("Map back button pressed");
    g_app->navigateToScreen(LocalifyApp::HOME_SCREEN);
}

void MapScreen::onConfirm() {
    LOGI("Map confirm button pressed - radius: %.1f miles", currentRadius);
    
    // TODO: Save the selected radius to user preferences
    // For now, just go back to home screen
    g_app->navigateToScreen(LocalifyApp::HOME_SCREEN);
}

void MapScreen::updateRadius(float newRadius) {
    currentRadius = newRadius;
    LOGI("Updated radius to: %.1f miles", currentRadius);
}

} // namespace localify
