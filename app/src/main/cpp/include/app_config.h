#ifndef LOCALIFY_APP_CONFIG_H
#define LOCALIFY_APP_CONFIG_H

#include <string>

namespace localify {

// App configuration constants (replacing XML resources)
class AppConfig {
public:
    // App metadata
    static constexpr const char* APP_NAME = "Localify";
    static constexpr const char* PACKAGE_NAME = "com.localify.android";
    static constexpr int VERSION_CODE = 1;
    static constexpr const char* VERSION_NAME = "1.0.0";
    
    // API configuration
    static constexpr const char* API_BASE_URL = "https://staging.localify.org";
    static constexpr const char* SPOTIFY_CLIENT_ID = "your_spotify_client_id";
    static constexpr const char* DEEP_LINK_SCHEME = "localify";
    
    // UI strings (replacing strings.xml)
    static constexpr const char* WELCOME_TITLE = "Welcome to Localify";
    static constexpr const char* DISCOVER_MUSIC = "Discover local music events";
    static constexpr const char* GUEST_LOGIN = "Continue as Guest";
    static constexpr const char* APPLE_LOGIN = "Sign in with Apple";
    static constexpr const char* SPOTIFY_LOGIN = "Connect with Spotify";
    static constexpr const char* HOME = "Home";
    static constexpr const char* SEARCH = "Search";
    static constexpr const char* FAVORITES = "Favorites";
    static constexpr const char* PROFILE = "Profile";
    static constexpr const char* MAP = "Map";
    static constexpr const char* REFRESH = "Refresh";
    static constexpr const char* BACK = "Back";
    static constexpr const char* CONFIRM = "Confirm";
    static constexpr const char* CONNECT_EMAIL = "Connect Email";
    static constexpr const char* CONNECT_SPOTIFY = "Connect Spotify";
    static constexpr const char* LOGOUT = "Logout";
    static constexpr const char* DELETE_ACCOUNT = "Delete Account";
    static constexpr const char* SEARCH_PLACEHOLDER = "Search artists, events, venues...";
    static constexpr const char* ARTISTS = "Artists";
    static constexpr const char* EVENTS = "Events";
    static constexpr const char* VENUES = "Venues";
    static constexpr const char* NO_FAVORITES = "No favorites yet. Start exploring!";
    
    // Theme colors (replacing themes.xml)
    struct Colors {
        // Localify brand colors
        static constexpr float PRIMARY_R = 0.91f;    // #E91E63
        static constexpr float PRIMARY_G = 0.12f;
        static constexpr float PRIMARY_B = 0.39f;
        
        static constexpr float SECONDARY_R = 1.0f;   // #FF4081
        static constexpr float SECONDARY_G = 0.25f;
        static constexpr float SECONDARY_B = 0.51f;
        
        static constexpr float BACKGROUND_R = 1.0f;  // White
        static constexpr float BACKGROUND_G = 1.0f;
        static constexpr float BACKGROUND_B = 1.0f;
        
        static constexpr float TEXT_R = 0.0f;        // Black
        static constexpr float TEXT_G = 0.0f;
        static constexpr float TEXT_B = 0.0f;
        
        static constexpr float GRAY_R = 0.5f;        // Gray
        static constexpr float GRAY_G = 0.5f;
        static constexpr float GRAY_B = 0.5f;
        
        static constexpr float LIGHT_GRAY_R = 0.9f;  // Light Gray
        static constexpr float LIGHT_GRAY_G = 0.9f;
        static constexpr float LIGHT_GRAY_B = 0.9f;
    };
    
    // App permissions (replacing AndroidManifest.xml permissions)
    struct Permissions {
        static constexpr const char* INTERNET = "android.permission.INTERNET";
        static constexpr const char* ACCESS_NETWORK_STATE = "android.permission.ACCESS_NETWORK_STATE";
        static constexpr const char* ACCESS_FINE_LOCATION = "android.permission.ACCESS_FINE_LOCATION";
        static constexpr const char* ACCESS_COARSE_LOCATION = "android.permission.ACCESS_COARSE_LOCATION";
    };
    
    // Screen dimensions and layout
    struct Layout {
        static constexpr float BUTTON_HEIGHT = 60.0f;
        static constexpr float BUTTON_MARGIN = 20.0f;
        static constexpr float LIST_ITEM_HEIGHT = 70.0f;
        static constexpr float SEARCH_BAR_HEIGHT = 50.0f;
        static constexpr float BOTTOM_NAV_HEIGHT = 80.0f;
        static constexpr float HEADER_HEIGHT = 60.0f;
        static constexpr float PADDING_SMALL = 10.0f;
        static constexpr float PADDING_MEDIUM = 20.0f;
        static constexpr float PADDING_LARGE = 30.0f;
    };
    
    // Map configuration
    struct Map {
        static constexpr float MIN_RADIUS_MILES = 1.0f;
        static constexpr float MAX_RADIUS_MILES = 25.0f;
        static constexpr float DEFAULT_RADIUS_MILES = 5.0f;
        static constexpr float GRID_SPACING_PX = 40.0f;
        static constexpr float RADIUS_SCALE_FACTOR = 10.0f; // pixels per mile
    };
};

} // namespace localify

#endif // LOCALIFY_APP_CONFIG_H
