#ifndef LOCALIFY_MODELS_H
#define LOCALIFY_MODELS_H

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace localify {

// Forward declarations
struct AuthResponse;
struct UserDetails;
struct ArtistResponse;
struct EventResponse;
struct VenueResponse;
struct CityResponse;

// Enums
enum class FavoriteType {
    ARTISTS,
    EVENTS,
    VENUES
};

enum class FavoriteTimeFrame {
    PREVIOUS,
    UPCOMING
};

// Auth Response
struct AuthResponse {
    std::string token;
    std::string refreshToken;
    int expiresIn;
    
    AuthResponse() = default;
    AuthResponse(const std::string& token, const std::string& refreshToken, int expiresIn)
        : token(token), refreshToken(refreshToken), expiresIn(expiresIn) {}
};

// User Details
struct UserDetails {
    std::string id;
    std::string name;
    std::optional<std::string> email;
    std::optional<std::string> appleId;
    std::optional<std::string> spotifyId;
    long accountCreationDate;
    std::optional<std::string> profileImage;
    std::optional<std::string> spotifyProfileImage;
    std::optional<int> playlistLocalSongsPerSeed;
    bool anonymousUser;
    bool emailConnected;
    bool appleConnected;
    bool spotifyConnected;
    bool emailVerified;
    bool emailOptIn;
    bool isAdmin;
    bool isTeamMember;
    bool playlistUseSeedSongs;
    bool playlistGeneration;
    
    UserDetails() = default;
};

// Artist Response
struct ArtistResponse {
    std::string id;
    std::string name;
    std::optional<std::string> imageUrl;
    std::optional<std::string> spotifyId;
    std::vector<std::string> genres;
    int popularity;
    bool isFavorite;
    
    ArtistResponse() = default;
};

// Event Response
struct EventResponse {
    std::string id;
    std::string name;
    std::string description;
    std::string startDate;
    std::string endDate;
    std::optional<std::string> imageUrl;
    std::string venueId;
    std::string venueName;
    std::vector<ArtistResponse> artists;
    bool isFavorite;
    double latitude;
    double longitude;
    
    EventResponse() = default;
};

// Venue Response
struct VenueResponse {
    std::string id;
    std::string name;
    std::string address;
    std::string city;
    std::string state;
    std::string country;
    double latitude;
    double longitude;
    std::optional<std::string> imageUrl;
    bool isFavorite;
    
    VenueResponse() = default;
};

// City Response
struct CityResponse {
    std::string id;
    std::string name;
    std::string state;
    std::string country;
    double latitude;
    double longitude;
    
    CityResponse() = default;
};

// User City
struct UserCity {
    std::string id;
    std::string cityId;
    std::string cityName;
    double radius;
    bool selected;
    
    UserCity() = default;
};

// Search Response
struct SearchResponse {
    std::vector<ArtistResponse> artists;
    std::vector<EventResponse> events;
    std::vector<VenueResponse> venues;
    std::vector<CityResponse> cities;
    
    SearchResponse() = default;
};

// Error Response
struct ErrorResponse {
    int status;
    std::string error;
    std::optional<std::string> message;
    std::string path;
    std::string timestamp;
    std::string requestId;
    
    ErrorResponse() = default;
};

} // namespace localify

#endif // LOCALIFY_MODELS_H
