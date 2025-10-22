#include "screens.h"
#include "api_service.h"
#include "json_parser.h"
#include <android/log.h>

#define LOG_TAG "LocalifyScreens"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace localify {

// LoginScreen implementation
LoginScreen::LoginScreen() : Screen("Welcome to Localify") {}

void LoginScreen::initialize() {
    LOGI("Initializing Login Screen");
    
    // Clear existing components
    components.clear();
    
    float screenWidth = g_app->getWidth();
    float screenHeight = g_app->getHeight();
    
    // Create guest login button
    guestLoginButton = std::make_unique<Button>(
        Rect(screenWidth * 0.1f, screenHeight * 0.4f, screenWidth * 0.8f, 60.0f),
        "Continue as Guest"
    );
    guestLoginButton->setBackgroundColor(Color::LocalifyPink());
    guestLoginButton->setTextColor(Color::White());
    guestLoginButton->setOnClick([this]() { onGuestLogin(); });
    
    // Create Apple login button
    appleLoginButton = std::make_unique<Button>(
        Rect(screenWidth * 0.1f, screenHeight * 0.5f, screenWidth * 0.8f, 60.0f),
        "Sign in with Apple"
    );
    appleLoginButton->setBackgroundColor(Color::Black());
    appleLoginButton->setTextColor(Color::White());
    appleLoginButton->setOnClick([this]() { onAppleLogin(); });
    
    // Create Spotify login button
    spotifyLoginButton = std::make_unique<Button>(
        Rect(screenWidth * 0.1f, screenHeight * 0.6f, screenWidth * 0.8f, 60.0f),
        "Connect with Spotify"
    );
    spotifyLoginButton->setBackgroundColor(Color(0.11f, 0.73f, 0.33f, 1.0f)); // Spotify green
    spotifyLoginButton->setTextColor(Color::White());
    spotifyLoginButton->setOnClick([this]() { onSpotifyLogin(); });
    
    // Add components to screen
    addComponent(std::move(guestLoginButton));
    addComponent(std::move(appleLoginButton));
    addComponent(std::move(spotifyLoginButton));
}

void LoginScreen::onGuestLogin() {
    LOGI("Guest login initiated");
    
    // Create guest user via API
    try {
        auto future = APIService::getInstance().createGuestUser();
        AuthResponse auth = future.get();
        
        LOGI("Guest user created successfully");
        g_app->navigateToScreen(LocalifyApp::HOME_SCREEN);
    } catch (const std::exception& e) {
        LOGE("Failed to create guest user: %s", e.what());
    }
}

void LoginScreen::onAppleLogin() {
    LOGI("Apple login initiated");
    // TODO: Implement Apple Sign-In
}

void LoginScreen::onSpotifyLogin() {
    LOGI("Spotify login initiated");
    // TODO: Implement Spotify OAuth
}

// HomeScreen implementation
HomeScreen::HomeScreen() : Screen("Home") {}

void HomeScreen::initialize() {
    LOGI("Initializing Home Screen");
    
    components.clear();
    
    float screenWidth = g_app->getWidth();
    float screenHeight = g_app->getHeight();
    
    // Create refresh button
    refreshButton = std::make_unique<Button>(
        Rect(screenWidth - 120.0f, 20.0f, 100.0f, 50.0f),
        "Refresh"
    );
    refreshButton->setBackgroundColor(Color::LocalifyPink());
    refreshButton->setTextColor(Color::White());
    refreshButton->setOnClick([this]() { onRefresh(); });
    
    // Create map button
    mapButton = std::make_unique<Button>(
        Rect(20.0f, 20.0f, 100.0f, 50.0f),
        "Map"
    );
    mapButton->setBackgroundColor(Color::Gray());
    mapButton->setTextColor(Color::White());
    mapButton->setOnClick([this]() { onMapView(); });
    
    // Create recommendations list
    recommendationsList = std::make_unique<ListView>(
        Rect(0.0f, 80.0f, screenWidth, screenHeight - 160.0f), // Leave space for bottom nav
        80.0f
    );
    recommendationsList->setOnItemClick([this](int index) { onItemSelected(index); });
    
    // Add components
    addComponent(std::move(refreshButton));
    addComponent(std::move(mapButton));
    addComponent(std::move(recommendationsList));
    
    // Load initial recommendations
    loadRecommendations();
}

void HomeScreen::update(float deltaTime) {
    Screen::update(deltaTime);
}

void HomeScreen::loadRecommendations() {
    LOGI("Loading recommendations");
    
    // TODO: Get user's selected city and load recommendations
    std::vector<std::string> dummyRecommendations = {
        "🎵 Arctic Monkeys - Tonight at The Fillmore",
        "🎵 Tame Impala - Tomorrow at Red Rocks",
        "🎵 The Strokes - This Weekend at Madison Square Garden",
        "🎵 Radiohead - Next Month at Greek Theatre",
        "🎵 Foo Fighters - Coming Soon to Your City"
    };
    
    recommendationsList->setItems(dummyRecommendations);
}

void HomeScreen::onRefresh() {
    LOGI("Refreshing recommendations");
    loadRecommendations();
}

void HomeScreen::onMapView() {
    LOGI("Opening map view");
    g_app->navigateToScreen(LocalifyApp::MAP_SCREEN);
}

void HomeScreen::onItemSelected(int index) {
    LOGI("Selected recommendation at index: %d", index);
    // TODO: Navigate to event detail view
}

// SearchScreen implementation
SearchScreen::SearchScreen() : Screen("Search"), selectedTab(0) {}

void SearchScreen::initialize() {
    LOGI("Initializing Search Screen");
    
    components.clear();
    
    float screenWidth = g_app->getWidth();
    float screenHeight = g_app->getHeight();
    
    // Create search bar
    searchBar = std::make_unique<SearchBar>(
        Rect(20.0f, 20.0f, screenWidth - 40.0f, 50.0f),
        "Search artists, events, venues..."
    );
    searchBar->setOnTextChanged([this](const std::string& query) { onSearch(query); });
    
    // Create tab buttons
    float tabWidth = screenWidth / 3.0f;
    
    artistsTab = std::make_unique<Button>(
        Rect(0.0f, 80.0f, tabWidth, 50.0f),
        "Artists"
    );
    artistsTab->setOnClick([this]() { onTabSelected(0); });
    
    eventsTab = std::make_unique<Button>(
        Rect(tabWidth, 80.0f, tabWidth, 50.0f),
        "Events"
    );
    eventsTab->setOnClick([this]() { onTabSelected(1); });
    
    venuesTab = std::make_unique<Button>(
        Rect(tabWidth * 2, 80.0f, tabWidth, 50.0f),
        "Venues"
    );
    venuesTab->setOnClick([this]() { onTabSelected(2); });
    
    // Create results list
    resultsList = std::make_unique<ListView>(
        Rect(0.0f, 140.0f, screenWidth, screenHeight - 220.0f),
        70.0f
    );
    resultsList->setOnItemClick([this](int index) { onResultSelected(index); });
    
    // Add components
    addComponent(std::move(searchBar));
    addComponent(std::move(artistsTab));
    addComponent(std::move(eventsTab));
    addComponent(std::move(venuesTab));
    addComponent(std::move(resultsList));
    
    // Set initial tab selection
    onTabSelected(0);
}

void SearchScreen::onSearch(const std::string& query) {
    LOGI("Searching for: %s", query.c_str());
    
    if (query.empty()) {
        resultsList->clearItems();
        return;
    }
    
    try {
        auto future = APIService::getInstance().fetchSearch(query, false);
        currentResults = future.get();
        updateResultsList();
    } catch (const std::exception& e) {
        LOGE("Search failed: %s", e.what());
    }
}

void SearchScreen::onTabSelected(int tab) {
    selectedTab = tab;
    
    // Update tab button colors
    artistsTab->setBackgroundColor(tab == 0 ? Color::LocalifyPink() : Color::LightGray());
    eventsTab->setBackgroundColor(tab == 1 ? Color::LocalifyPink() : Color::LightGray());
    venuesTab->setBackgroundColor(tab == 2 ? Color::LocalifyPink() : Color::LightGray());
    
    updateResultsList();
}

void SearchScreen::onResultSelected(int index) {
    LOGI("Selected search result at index: %d in tab: %d", index, selectedTab);
    // TODO: Navigate to detail view based on selected tab and index
}

void SearchScreen::updateResultsList() {
    std::vector<std::string> items;
    
    switch (selectedTab) {
        case 0: // Artists
            for (const auto& artist : currentResults.artists) {
                items.push_back("🎤 " + artist.name);
            }
            break;
        case 1: // Events
            for (const auto& event : currentResults.events) {
                items.push_back("🎵 " + event.name + " at " + event.venueName);
            }
            break;
        case 2: // Venues
            for (const auto& venue : currentResults.venues) {
                items.push_back("🏛️ " + venue.name + " - " + venue.city);
            }
            break;
    }
    
    resultsList->setItems(items);
}

// FavoritesScreen implementation
FavoritesScreen::FavoritesScreen() : Screen("Favorites"), selectedTab(0) {}

void FavoritesScreen::initialize() {
    LOGI("Initializing Favorites Screen");
    
    components.clear();
    
    float screenWidth = g_app->getWidth();
    float screenHeight = g_app->getHeight();
    
    // Create tab buttons
    float tabWidth = screenWidth / 3.0f;
    
    artistsTab = std::make_unique<Button>(
        Rect(0.0f, 20.0f, tabWidth, 50.0f),
        "Artists"
    );
    artistsTab->setOnClick([this]() { onTabSelected(0); });
    
    eventsTab = std::make_unique<Button>(
        Rect(tabWidth, 20.0f, tabWidth, 50.0f),
        "Events"
    );
    eventsTab->setOnClick([this]() { onTabSelected(1); });
    
    venuesTab = std::make_unique<Button>(
        Rect(tabWidth * 2, 20.0f, tabWidth, 50.0f),
        "Venues"
    );
    venuesTab->setOnClick([this]() { onTabSelected(2); });
    
    // Create favorites list
    favoritesList = std::make_unique<ListView>(
        Rect(0.0f, 80.0f, screenWidth, screenHeight - 160.0f),
        70.0f
    );
    favoritesList->setOnItemClick([this](int index) { onFavoriteSelected(index); });
    
    // Add components
    addComponent(std::move(artistsTab));
    addComponent(std::move(eventsTab));
    addComponent(std::move(venuesTab));
    addComponent(std::move(favoritesList));
    
    // Set initial tab and load favorites
    onTabSelected(0);
    loadFavorites();
}

void FavoritesScreen::update(float deltaTime) {
    Screen::update(deltaTime);
}

void FavoritesScreen::loadFavorites() {
    LOGI("Loading favorites");
    
    try {
        // Load favorite artists
        auto artistsFuture = APIService::getInstance().fetchFavoriteArtists();
        favoriteArtists = artistsFuture.get();
        
        // Load favorite events  
        auto eventsFuture = APIService::getInstance().fetchFavoriteEvents();
        favoriteEvents = eventsFuture.get();
        
        // Load favorite venues
        auto venuesFuture = APIService::getInstance().fetchFavoriteVenues();
        favoriteVenues = venuesFuture.get();
        
        updateFavoritesList();
    } catch (const std::exception& e) {
        LOGE("Failed to load favorites: %s", e.what());
    }
}

void FavoritesScreen::onTabSelected(int tab) {
    selectedTab = tab;
    
    // Update tab button colors
    artistsTab->setBackgroundColor(tab == 0 ? Color::LocalifyPink() : Color::LightGray());
    eventsTab->setBackgroundColor(tab == 1 ? Color::LocalifyPink() : Color::LightGray());
    venuesTab->setBackgroundColor(tab == 2 ? Color::LocalifyPink() : Color::LightGray());
    
    updateFavoritesList();
}

void FavoritesScreen::onFavoriteSelected(int index) {
    LOGI("Selected favorite at index: %d in tab: %d", index, selectedTab);
    // TODO: Navigate to detail view or remove from favorites
}

void FavoritesScreen::updateFavoritesList() {
    std::vector<std::string> items;
    
    switch (selectedTab) {
        case 0: // Artists
            for (const auto& artist : favoriteArtists) {
                items.push_back("⭐ " + artist.name);
            }
            break;
        case 1: // Events
            for (const auto& event : favoriteEvents) {
                items.push_back("⭐ " + event.name + " at " + event.venueName);
            }
            break;
        case 2: // Venues
            for (const auto& venue : favoriteVenues) {
                items.push_back("⭐ " + venue.name + " - " + venue.city);
            }
            break;
    }
    
    if (items.empty()) {
        items.push_back("No favorites yet. Start exploring!");
    }
    
    favoritesList->setItems(items);
}

// ProfileScreen implementation
ProfileScreen::ProfileScreen() : Screen("Profile"), userLoaded(false) {}

void ProfileScreen::initialize() {
    LOGI("Initializing Profile Screen");
    
    components.clear();
    
    float screenWidth = g_app->getWidth();
    float screenHeight = g_app->getHeight();
    
    // Create profile buttons
    connectEmailButton = std::make_unique<Button>(
        Rect(screenWidth * 0.1f, screenHeight * 0.3f, screenWidth * 0.8f, 60.0f),
        "Connect Email"
    );
    connectEmailButton->setBackgroundColor(Color::LocalifyPink());
    connectEmailButton->setTextColor(Color::White());
    connectEmailButton->setOnClick([this]() { onConnectEmail(); });
    
    connectSpotifyButton = std::make_unique<Button>(
        Rect(screenWidth * 0.1f, screenHeight * 0.4f, screenWidth * 0.8f, 60.0f),
        "Connect Spotify"
    );
    connectSpotifyButton->setBackgroundColor(Color(0.11f, 0.73f, 0.33f, 1.0f)); // Spotify green
    connectSpotifyButton->setTextColor(Color::White());
    connectSpotifyButton->setOnClick([this]() { onConnectSpotify(); });
    
    logoutButton = std::make_unique<Button>(
        Rect(screenWidth * 0.1f, screenHeight * 0.6f, screenWidth * 0.8f, 60.0f),
        "Logout"
    );
    logoutButton->setBackgroundColor(Color::Gray());
    logoutButton->setTextColor(Color::White());
    logoutButton->setOnClick([this]() { onLogout(); });
    
    deleteAccountButton = std::make_unique<Button>(
        Rect(screenWidth * 0.1f, screenHeight * 0.7f, screenWidth * 0.8f, 60.0f),
        "Delete Account"
    );
    deleteAccountButton->setBackgroundColor(Color(0.8f, 0.2f, 0.2f, 1.0f)); // Red
    deleteAccountButton->setTextColor(Color::White());
    deleteAccountButton->setOnClick([this]() { onDeleteAccount(); });
    
    // Add components
    addComponent(std::move(connectEmailButton));
    addComponent(std::move(connectSpotifyButton));
    addComponent(std::move(logoutButton));
    addComponent(std::move(deleteAccountButton));
    
    loadUserProfile();
}

void ProfileScreen::update(float deltaTime) {
    Screen::update(deltaTime);
}

void ProfileScreen::loadUserProfile() {
    LOGI("Loading user profile");
    
    try {
        auto future = APIService::getInstance().fetchUserDetails();
        currentUser = future.get();
        userLoaded = true;
        updateUI();
    } catch (const std::exception& e) {
        LOGE("Failed to load user profile: %s", e.what());
    }
}

void ProfileScreen::onConnectEmail() {
    LOGI("Connect email initiated");
    // TODO: Implement email connection flow
}

void ProfileScreen::onConnectSpotify() {
    LOGI("Connect Spotify initiated");
    // TODO: Implement Spotify connection flow
}

void ProfileScreen::onLogout() {
    LOGI("Logout initiated");
    
    // Clear authentication
    APIService::getInstance().clearAuth();
    
    // Navigate back to login
    g_app->navigateToScreen(LocalifyApp::LOGIN_SCREEN);
}

void ProfileScreen::onDeleteAccount() {
    LOGI("Delete account initiated");
    
    try {
        auto future = APIService::getInstance().deleteUserAccount();
        future.get();
        
        // Clear authentication and navigate to login
        APIService::getInstance().clearAuth();
        g_app->navigateToScreen(LocalifyApp::LOGIN_SCREEN);
    } catch (const std::exception& e) {
        LOGE("Failed to delete account: %s", e.what());
    }
}

void ProfileScreen::updateUI() {
    if (!userLoaded) return;
    
    // Update button text based on user state
    if (currentUser.emailConnected) {
        connectEmailButton->setText("Email Connected ✓");
        connectEmailButton->setEnabled(false);
    }
    
    if (currentUser.spotifyConnected) {
        connectSpotifyButton->setText("Spotify Connected ✓");
        connectSpotifyButton->setEnabled(false);
    }
}

// BottomNavigationBar implementation
BottomNavigationBar::BottomNavigationBar(const Rect& bounds) : UIComponent(bounds) {
    backgroundColor = Color::White();
    setupNavItems();
}

void BottomNavigationBar::setupNavItems() {
    navItems.clear();
    
    float itemWidth = bounds.width / 4.0f;
    
    navItems.push_back({"Home", 1, true, Rect(bounds.x, bounds.y, itemWidth, bounds.height)});
    navItems.push_back({"Search", 2, false, Rect(bounds.x + itemWidth, bounds.y, itemWidth, bounds.height)});
    navItems.push_back({"Favorites", 3, false, Rect(bounds.x + itemWidth * 2, bounds.y, itemWidth, bounds.height)});
    navItems.push_back({"Profile", 4, false, Rect(bounds.x + itemWidth * 3, bounds.y, itemWidth, bounds.height)});
}

void BottomNavigationBar::draw() {
    if (!visible) return;
    
    // Draw navigation bar background
    g_app->drawRect(bounds, backgroundColor);
    
    // Draw top border
    Rect borderRect(bounds.x, bounds.y, bounds.width, 2);
    g_app->drawRect(borderRect, Color::LightGray());
    
    // Draw navigation items
    for (const auto& item : navItems) {
        Color itemColor = item.selected ? Color::LocalifyPink() : Color::Gray();
        
        // Draw item background if selected
        if (item.selected) {
            g_app->drawRect(item.bounds, Color(0.91f, 0.12f, 0.39f, 0.1f)); // Light pink
        }
        
        // Draw item text (centered)
        float textX = item.bounds.x + item.bounds.width / 2.0f - item.title.length() * 4.0f;
        float textY = item.bounds.y + item.bounds.height / 2.0f + 8.0f;
        g_app->drawText(item.title, textX, textY, itemColor);
    }
}

bool BottomNavigationBar::handleTouch(const TouchEvent& event) {
    if (!visible || !enabled) return false;
    
    if (event.action == AMOTION_EVENT_ACTION_DOWN && bounds.contains(event.x, event.y)) {
        for (size_t i = 0; i < navItems.size(); ++i) {
            if (navItems[i].bounds.contains(event.x, event.y)) {
                setSelectedItem(i);
                if (onNavigationCallback) {
                    onNavigationCallback(navItems[i].screenType);
                }
                return true;
            }
        }
    }
    
    return false;
}

void BottomNavigationBar::setSelectedItem(int index) {
    for (size_t i = 0; i < navItems.size(); ++i) {
        navItems[i].selected = (i == index);
    }
}

} // namespace localify
