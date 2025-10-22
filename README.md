# Localify Android C++ App

A **pure C++ Android application** based on the iOS Localify app, written entirely in C++ using the Android NDK without any Java, Kotlin, or XML files.

## Features

✅ **Pure C++ Implementation** - No Java/Kotlin/XML dependencies
✅ **Native Android UI** - Custom OpenGL ES 2.0 based UI components  
✅ **API Integration** - HTTP client for Localify API calls
✅ **Authentication** - Guest login, Apple Sign-In, Spotify OAuth support
✅ **Core Screens** - Home, Search, Favorites, Profile screens
✅ **Map Functionality** - Canvas-based radius selection (no Google Maps dependency)
✅ **Bottom Navigation** - Consistent navigation across all screens
✅ **Real-time Search** - Artists, events, venues search
✅ **Favorites Management** - Add/remove favorites functionality

## Architecture

### Core Components

- **`android_ui.h/cpp`** - Custom UI framework with OpenGL ES rendering
- **`api_service.h/cpp`** - HTTP API client matching iOS APIService functionality  
- **`models.h`** - Data models (User, Artist, Event, Venue, etc.)
- **`screens.h/cpp`** - All app screens (Login, Home, Search, Favorites, Profile)
- **`map_screen.cpp`** - Canvas-based map with iOS-style appearance
- **`http_client.h/cpp`** - Native HTTP client (replaces libcurl)
- **`json_parser.h/cpp`** - JSON parsing for API responses
- **`app_config.h`** - App configuration (replaces XML resources)
- **`manifest.h`** - App manifest constants (replaces AndroidManifest.xml)

### Key Features

#### 🎨 **Custom UI Framework**
- OpenGL ES 2.0 based rendering
- Custom components: Button, ListView, SearchBar, BottomNavigationBar
- Localify brand colors (#E91E63 pink theme)
- Touch event handling
- Screen navigation system

#### 🌐 **API Integration**
- RESTful API client matching iOS implementation
- Authentication with token management
- User management (guest users, profile management)
- Search functionality (artists, events, venues)
- Favorites system
- City and radius selection

#### 🗺️ **Map Implementation**
- Canvas-based map visualization (no external dependencies)
- iOS-style appearance with system blue colors
- Interactive radius selection (1-25 miles)
- Grid pattern streets, park areas, water bodies
- Touch-based radius adjustment

#### 📱 **Screen Structure**
- **LoginScreen** - Guest login, Apple Sign-In, Spotify OAuth
- **HomeScreen** - Event recommendations, artist suggestions
- **SearchScreen** - Multi-tab search (Artists/Events/Venues)
- **FavoritesScreen** - Categorized favorites management
- **ProfileScreen** - Account management, connections
- **MapScreen** - Radius selection with visual feedback

## Building

### Prerequisites
- Android NDK r25 or later
- CMake 3.22.1 or later
- Android SDK API 24+ (minimum) / API 34 (target)

### Build Commands
```bash
cd localify-android-cpp
./gradlew assembleDebug
```

### Project Structure
```
app/src/main/cpp/
├── include/           # Header files
│   ├── android_ui.h
│   ├── api_service.h
│   ├── models.h
│   ├── screens.h
│   ├── http_client.h
│   ├── json_parser.h
│   ├── app_config.h
│   └── manifest.h
├── src/              # Source files
│   ├── main.cpp
│   ├── android_ui.cpp
│   ├── api_service.cpp
│   ├── screens.cpp
│   ├── map_screen.cpp
│   ├── http_client.cpp
│   ├── json_parser.cpp
│   └── native_activity_handler.cpp
└── CMakeLists.txt    # Build configuration
```

## API Compatibility

The C++ implementation maintains full compatibility with the iOS Localify API:

- **Authentication**: `/v1/auth/guest`, `/v1/auth/token`, `/v1/auth/apple/token`
- **User Management**: `/v1/@me` (GET/PATCH/DELETE)
- **Cities**: `/v1/@me/cities`, `/v1/cities/search`
- **Seeds**: `/v1/@me/seeds` (artist preferences)
- **Favorites**: `/v1/@me/{artists|events|venues}/favorites`
- **Search**: `/v1/search`, `/v1/artists/search`
- **Recommendations**: `/v1/@me/cities/{id}/artist-recommendations`

## Configuration

All app configuration is defined in C++ constants (no XML):

```cpp
// App metadata
AppConfig::APP_NAME = "Localify"
AppConfig::API_BASE_URL = "https://staging.localify.org"

// UI colors  
AppConfig::Colors::PRIMARY_R/G/B = Localify pink (#E91E63)

// Layout dimensions
AppConfig::Layout::BUTTON_HEIGHT = 60.0f
AppConfig::Layout::BOTTOM_NAV_HEIGHT = 80.0f
```

## Development Notes

- **No Java/XML**: Completely eliminates Android's Java layer
- **Native Performance**: Direct OpenGL rendering and native networking
- **Memory Efficient**: Manual memory management, no garbage collection
- **Cross-Platform Ready**: Core logic can be shared with other platforms
- **Debugging**: Full C++ debugging support with NDK

## Comparison with iOS App

| Feature | iOS (Swift) | Android (C++) |
|---------|-------------|---------------|
| UI Framework | SwiftUI | Custom OpenGL ES |
| Networking | URLSession | Native sockets |
| JSON Parsing | Codable | Custom parser |
| Map | MapKit | Canvas-based |
| Navigation | NavigationView | Custom navigation |
| Authentication | AuthenticationServices | Custom implementation |

This C++ Android app provides the same functionality as the iOS version while being completely native and dependency-free.
