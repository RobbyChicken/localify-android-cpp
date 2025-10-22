#ifndef LOCALIFY_SCREENS_H
#define LOCALIFY_SCREENS_H

#include "android_ui.h"
#include "models.h"
#include <memory>

namespace localify {

// Login Screen
class LoginScreen : public Screen {
private:
    std::unique_ptr<Button> guestLoginButton;
    std::unique_ptr<Button> appleLoginButton;
    std::unique_ptr<Button> spotifyLoginButton;
    
public:
    LoginScreen();
    void initialize() override;
    
private:
    void onGuestLogin();
    void onAppleLogin();
    void onSpotifyLogin();
};

// Home Screen
class HomeScreen : public Screen {
private:
    std::unique_ptr<ListView> recommendationsList;
    std::unique_ptr<Button> refreshButton;
    std::unique_ptr<Button> mapButton;
    
    std::vector<EventResponse> currentEvents;
    std::vector<ArtistResponse> currentArtists;
    
public:
    HomeScreen();
    void initialize() override;
    void update(float deltaTime) override;
    
private:
    void loadRecommendations();
    void onRefresh();
    void onMapView();
    void onItemSelected(int index);
};

// Search Screen
class SearchScreen : public Screen {
private:
    std::unique_ptr<SearchBar> searchBar;
    std::unique_ptr<ListView> resultsList;
    std::unique_ptr<Button> artistsTab;
    std::unique_ptr<Button> eventsTab;
    std::unique_ptr<Button> venuesTab;
    
    SearchResponse currentResults;
    int selectedTab; // 0=artists, 1=events, 2=venues
    
public:
    SearchScreen();
    void initialize() override;
    
private:
    void onSearch(const std::string& query);
    void onTabSelected(int tab);
    void onResultSelected(int index);
    void updateResultsList();
};

// Favorites Screen
class FavoritesScreen : public Screen {
private:
    std::unique_ptr<Button> artistsTab;
    std::unique_ptr<Button> eventsTab;
    std::unique_ptr<Button> venuesTab;
    std::unique_ptr<ListView> favoritesList;
    
    std::vector<ArtistResponse> favoriteArtists;
    std::vector<EventResponse> favoriteEvents;
    std::vector<VenueResponse> favoriteVenues;
    int selectedTab; // 0=artists, 1=events, 2=venues
    
public:
    FavoritesScreen();
    void initialize() override;
    void update(float deltaTime) override;
    
private:
    void loadFavorites();
    void onTabSelected(int tab);
    void onFavoriteSelected(int index);
    void updateFavoritesList();
};

// Profile Screen
class ProfileScreen : public Screen {
private:
    std::unique_ptr<Button> connectEmailButton;
    std::unique_ptr<Button> connectSpotifyButton;
    std::unique_ptr<Button> logoutButton;
    std::unique_ptr<Button> deleteAccountButton;
    
    UserDetails currentUser;
    bool userLoaded;
    
public:
    ProfileScreen();
    void initialize() override;
    void update(float deltaTime) override;
    
private:
    void loadUserProfile();
    void onConnectEmail();
    void onConnectSpotify();
    void onLogout();
    void onDeleteAccount();
    void updateUI();
};

// Map Screen (for radius selection)
class MapScreen : public Screen {
private:
    std::unique_ptr<Button> backButton;
    std::unique_ptr<Button> confirmButton;
    
    float currentRadius;
    float centerX, centerY;
    bool dragging;
    
public:
    MapScreen();
    void initialize() override;
    void draw() override;
    bool handleTouch(const TouchEvent& event) override;
    
private:
    void drawMap();
    void drawRadiusCircle();
    void onBack();
    void onConfirm();
    void updateRadius(float newRadius);
};

// Bottom Navigation Bar
class BottomNavigationBar : public UIComponent {
private:
    struct NavItem {
        std::string title;
        int screenType;
        bool selected;
        Rect bounds;
    };
    
    std::vector<NavItem> navItems;
    std::function<void(int)> onNavigationCallback;
    
public:
    BottomNavigationBar(const Rect& bounds);
    
    void draw() override;
    bool handleTouch(const TouchEvent& event) override;
    
    void setSelectedItem(int index);
    void setOnNavigation(std::function<void(int)> callback) { onNavigationCallback = callback; }
    
private:
    void setupNavItems();
};

} // namespace localify

#endif // LOCALIFY_SCREENS_H
