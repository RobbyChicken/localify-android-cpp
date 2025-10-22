#include "api_service.h"
#include "json_parser.h"
#include "app_config.h"
#include <android/log.h>
#include <sstream>
#include <chrono>
#include <thread>

#define LOG_TAG "LocalifyAPI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace localify {

std::unique_ptr<APIService> APIService::instance = nullptr;

APIService::APIService() : apiUrl(AppConfig::API_BASE_URL) {
    LOGI("Initializing APIService with base URL: %s", apiUrl.c_str());
}

APIService::~APIService() {
    LOGI("APIService destroyed");
}

APIService& APIService::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<APIService>(new APIService());
    }
    return *instance;
}

HTTPResponse APIService::performRequest(const std::string& url, const std::string& method, 
                                       const std::string& body, bool ignoreAuth) {
    HTTPResponse response;
    
    LOGI("API Request: %s %s", method.c_str(), url.c_str());
    
    // Create HTTP request
    HttpRequest request(url, method);
    request.body = body;
    
    // Set headers
    request.setContentType("application/json");
    request.setUserAgent("Localify-Android-CPP/1.0");
    
    // Add authentication if not ignored
    if (!ignoreAuth && !currentAuthToken.empty()) {
        request.setAuthorization(currentAuthToken);
    }
    
    // Perform request using our HttpClient
    HttpResponse httpResponse = HttpClient::getInstance().request(request);
    
    // Convert HttpResponse to HTTPResponse
    response.statusCode = httpResponse.statusCode;
    response.data = httpResponse.body;
    response.error = httpResponse.error;
    
    LOGI("API Response: %ld", response.statusCode);
    
    return response;
}

std::string APIService::buildURL(const std::string& path) const {
    return apiUrl + path;
}

bool APIService::isTokenValid() const {
    // Simple token validation - in a real app, you'd check expiration time
    return !currentAuthToken.empty();
}

void APIService::storeAuth(const AuthResponse& auth) {
    currentAuthToken = auth.token;
    // In a real implementation, you'd store this securely
    LOGI("Stored authentication token");
}

std::future<AuthResponse> APIService::refreshAuth(bool force) {
    return std::async(std::launch::async, [this, force]() -> AuthResponse {
        if (!force && isTokenValid()) {
            // Return current auth if still valid
            return AuthResponse(currentAuthToken, "", 3600);
        }
        
        // Implement token refresh logic here
        std::string url = buildURL("/v1/auth/refresh");
        std::string body = R"({"token": "refresh_token_placeholder"})";
        
        HTTPResponse response = performRequest(url, "POST", body, true);
        
        if (response.statusCode >= 200 && response.statusCode < 300) {
            AuthResponse auth = JSONParser::parseAuthResponse(response.data);
            storeAuth(auth);
            return auth;
        } else {
            throw std::runtime_error("Failed to refresh token: " + response.error);
        }
    });
}

std::future<AuthResponse> APIService::exchangeToken(const std::string& token, const std::string& secret) {
    return std::async(std::launch::async, [this, token, secret]() -> AuthResponse {
        std::string url = buildURL("/v1/auth/token");
        std::string body = R"({"token": ")" + token + R"(", "secret": ")" + secret + R"("})";
        
        HTTPResponse response = performRequest(url, "POST", body, true);
        
        if (response.statusCode >= 200 && response.statusCode < 300) {
            AuthResponse auth = JSONParser::parseAuthResponse(response.data);
            storeAuth(auth);
            return auth;
        } else {
            throw std::runtime_error("Failed to exchange token: " + response.error);
        }
    });
}

std::future<AuthResponse> APIService::createGuestUser() {
    return std::async(std::launch::async, [this]() -> AuthResponse {
        std::string url = buildURL("/v1/auth/guest");
        
        HTTPResponse response = performRequest(url, "POST", "", true);
        
        if (response.statusCode >= 200 && response.statusCode < 300) {
            AuthResponse auth = JSONParser::parseAuthResponse(response.data);
            storeAuth(auth);
            return auth;
        } else {
            throw std::runtime_error("Failed to create guest user: " + response.error);
        }
    });
}

std::future<UserDetails> APIService::fetchUserDetails() {
    return std::async(std::launch::async, [this]() -> UserDetails {
        std::string url = buildURL("/v1/@me");
        
        HTTPResponse response = performRequest(url, "GET");
        
        if (response.statusCode >= 200 && response.statusCode < 300) {
            return JSONParser::parseUserDetails(response.data);
        } else {
            throw std::runtime_error("Failed to fetch user details: " + response.error);
        }
    });
}

std::future<SearchResponse> APIService::fetchSearch(const std::string& text, bool autoSearchSpotify) {
    return std::async(std::launch::async, [this, text, autoSearchSpotify]() -> SearchResponse {
        if (text.empty()) {
            return SearchResponse();
        }
        
        std::string url = buildURL("/v1/search?q=" + text + "&autoSearchSpotify=" + 
                                 (autoSearchSpotify ? "true" : "false"));
        
        HTTPResponse response = performRequest(url, "GET");
        
        if (response.statusCode >= 200 && response.statusCode < 300) {
            return JSONParser::parseSearchResponse(response.data);
        } else {
            throw std::runtime_error("Failed to perform search: " + response.error);
        }
    });
}

std::future<std::vector<ArtistResponse>> APIService::fetchSearchArtists(const std::string& text, int limit) {
    return std::async(std::launch::async, [this, text, limit]() -> std::vector<ArtistResponse> {
        if (text.empty()) {
            return std::vector<ArtistResponse>();
        }
        
        std::string url = buildURL("/v1/artists/search?q=" + text + "&limit=" + std::to_string(limit));
        
        HTTPResponse response = performRequest(url, "GET");
        
        if (response.statusCode >= 200 && response.statusCode < 300) {
            return JSONParser::parseArtistArray(response.data);
        } else {
            throw std::runtime_error("Failed to search artists: " + response.error);
        }
    });
}

std::future<void> APIService::addFavorite(const std::string& id, FavoriteType type) {
    return std::async(std::launch::async, [this, id, type]() -> void {
        std::string typeStr;
        switch (type) {
            case FavoriteType::ARTISTS: typeStr = "artists"; break;
            case FavoriteType::EVENTS: typeStr = "events"; break;
            case FavoriteType::VENUES: typeStr = "venues"; break;
        }
        
        std::string url = buildURL("/v1/@me/" + typeStr + "/" + id + "/favorite");
        
        HTTPResponse response = performRequest(url, "PUT");
        
        if (response.statusCode < 200 || response.statusCode >= 300) {
            throw std::runtime_error("Failed to add favorite: " + response.error);
        }
    });
}

std::future<void> APIService::removeFavorite(const std::string& id, FavoriteType type) {
    return std::async(std::launch::async, [this, id, type]() -> void {
        std::string typeStr;
        switch (type) {
            case FavoriteType::ARTISTS: typeStr = "artists"; break;
            case FavoriteType::EVENTS: typeStr = "events"; break;
            case FavoriteType::VENUES: typeStr = "venues"; break;
        }
        
        std::string url = buildURL("/v1/@me/" + typeStr + "/" + id + "/favorite");
        
        HTTPResponse response = performRequest(url, "DELETE");
        
        if (response.statusCode < 200 || response.statusCode >= 300) {
            throw std::runtime_error("Failed to remove favorite: " + response.error);
        }
    });
}

// Utility methods
void APIService::setAuthToken(const std::string& token) {
    currentAuthToken = token;
}

std::string APIService::getAuthToken() const {
    return currentAuthToken;
}

void APIService::clearAuth() {
    currentAuthToken.clear();
    authExpiresAt.clear();
}

// Stub implementations for missing methods (to be implemented later)
std::future<std::vector<ArtistResponse>> APIService::fetchFavoriteArtists(int page, int limit) {
    return std::async(std::launch::async, []() -> std::vector<ArtistResponse> {
        LOGI("fetchFavoriteArtists stub called");
        return std::vector<ArtistResponse>();
    });
}

std::future<std::vector<EventResponse>> APIService::fetchFavoriteEvents(int page, int limit, bool upcoming) {
    return std::async(std::launch::async, []() -> std::vector<EventResponse> {
        LOGI("fetchFavoriteEvents stub called");
        return std::vector<EventResponse>();
    });
}

std::future<std::vector<VenueResponse>> APIService::fetchFavoriteVenues(int page, int limit) {
    return std::async(std::launch::async, []() -> std::vector<VenueResponse> {
        LOGI("fetchFavoriteVenues stub called");
        return std::vector<VenueResponse>();
    });
}

std::future<void> APIService::deleteUserAccount() {
    return std::async(std::launch::async, [this]() -> void {
        LOGI("deleteUserAccount stub called");
        clearAuth();
    });
}

} // namespace localify
