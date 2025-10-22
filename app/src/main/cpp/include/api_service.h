#ifndef LOCALIFY_API_SERVICE_H
#define LOCALIFY_API_SERVICE_H

#include "models.h"
#include <string>
#include <memory>
#include <future>
#include <functional>
#include <curl/curl.h>

namespace localify {

// API Error types
enum class APIError {
    INVALID_URL,
    ENCODING_FAILURE,
    AUTHENTICATION_FAILURE,
    CLIENT_4XX_ERROR,
    SERVER_5XX_ERROR,
    DECODING_FAILURE,
    REQUEST_FAILED,
    INVALID_RESPONSE,
    CANCELLED
};

// HTTP Response structure
struct HTTPResponse {
    long statusCode;
    std::string data;
    std::string error;
    
    HTTPResponse() : statusCode(0) {}
};

// Callback types
template<typename T>
using APICallback = std::function<void(const T&, APIError)>;

class APIService {
private:
    static std::unique_ptr<APIService> instance;
    std::string apiUrl;
    std::string currentAuthToken;
    std::string authExpiresAt;
    CURL* curl;
    
    // Private constructor for singleton
    APIService();
    
    // HTTP helper methods
    HTTPResponse performRequest(const std::string& url, const std::string& method, 
                               const std::string& body = "", bool ignoreAuth = false);
    std::string buildURL(const std::string& path) const;
    void setRequestHeaders(CURL* curl, const std::string& method, 
                          const std::string& body, bool ignoreAuth);
    
    // JSON parsing helpers
    template<typename T>
    T parseJSON(const std::string& jsonString);
    
    // Authentication helpers
    bool isTokenValid() const;
    void storeAuth(const AuthResponse& auth);

public:
    ~APIService();
    
    // Singleton access
    static APIService& getInstance();
    
    // Authentication methods
    std::future<AuthResponse> refreshAuth(bool force = false);
    std::future<AuthResponse> exchangeToken(const std::string& token, const std::string& secret);
    std::future<AuthResponse> exchangeAppleToken(const std::string& token, const std::string& name = "");
    std::future<AuthResponse> createGuestUser();
    
    // User methods
    std::future<UserDetails> fetchUserDetails();
    std::future<UserDetails> patchUserDetails(const UserDetails& userDetails);
    std::future<void> deleteUserAccount();
    
    // User Cities
    std::future<std::vector<CityResponse>> fetchUserNearestCities();
    std::future<std::vector<UserCity>> fetchUserCities();
    std::future<UserCity> putUserCity(const std::string& cityId, double radius, bool onboarding = false);
    std::future<UserCity> patchUserCities(const std::string& cityId, bool selected, double radius);
    std::future<void> deleteUserCity(const std::string& cityId);
    
    // User Seeds (Artists)
    std::future<std::vector<ArtistResponse>> fetchUserSeeds(bool all = false);
    std::future<std::vector<ArtistResponse>> putUserSeeds(const std::vector<std::string>& seeds);
    std::future<void> addArtistToUserSeeds(const std::string& seedId);
    std::future<void> deleteArtistFromUserSeeds(const std::string& seedId);
    
    // Favorites
    std::future<void> addFavorite(const std::string& id, FavoriteType type);
    std::future<void> removeFavorite(const std::string& id, FavoriteType type);
    std::future<std::vector<ArtistResponse>> fetchFavoriteArtists(int page = 0, int limit = 20);
    std::future<std::vector<EventResponse>> fetchFavoriteEvents(int page = 0, int limit = 20, bool upcoming = true);
    std::future<std::vector<VenueResponse>> fetchFavoriteVenues(int page = 0, int limit = 20);
    
    // Search
    std::future<SearchResponse> fetchSearch(const std::string& text, bool autoSearchSpotify = false);
    std::future<std::vector<ArtistResponse>> fetchSearchArtists(const std::string& text, int limit = 12);
    std::future<std::vector<CityResponse>> fetchSearchCities(const std::string& text, int limit = 10);
    
    // Artist methods
    std::future<ArtistResponse> fetchArtist(const std::string& artistId);
    std::future<std::vector<EventResponse>> fetchEventsForArtist(const std::string& artistId);
    std::future<std::vector<CityResponse>> fetchCitiesForArtist(const std::string& artistId);
    
    // Event methods
    std::future<EventResponse> fetchEvent(const std::string& eventId);
    
    // Venue methods
    std::future<VenueResponse> fetchVenue(const std::string& venueId);
    std::future<std::vector<EventResponse>> fetchVenueUpcomingEvents(const std::string& venueId);
    
    // City methods
    std::future<CityResponse> fetchCityDetails(const std::string& cityId);
    std::future<std::vector<ArtistResponse>> fetchArtistsForCities(const std::string& cityId, int page = 0, int limit = 20);
    std::future<std::vector<EventResponse>> fetchEventsForCities(const std::string& cityId, int page = 0, int limit = 20);
    std::future<std::vector<VenueResponse>> fetchVenuesForCities(const std::string& cityId, int page = 0, int limit = 20);
    
    // Recommendations
    std::future<std::vector<ArtistResponse>> fetchArtistRecommendations(const std::string& cityId);
    std::future<std::vector<EventResponse>> fetchEventRecommendations(const std::string& cityId);
    
    // Email verification
    std::future<std::string> emailVerification(const std::string& email);
    std::future<AuthResponse> emailLogin(const std::string& nonce, const std::string& code);
    
    // Spotify integration
    std::future<std::string> spotifyLink(const std::string& codeChallenge);
    
    // Feedback
    std::future<void> submitFeedback(const std::string& entry, const std::string& email);
    
    // Utility methods
    void setAuthToken(const std::string& token);
    std::string getAuthToken() const;
    void clearAuth();
};

} // namespace localify

#endif // LOCALIFY_API_SERVICE_H
