#include "api_service.h"
#include "json_parser.h"
#include <android/log.h>
#include <sstream>
#include <chrono>
#include <thread>

#define LOG_TAG "LocalifyAPI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace localify {

std::unique_ptr<APIService> APIService::instance = nullptr;

// Callback function for libcurl to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

APIService::APIService() : apiUrl("https://staging.localify.org") {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        LOGE("Failed to initialize libcurl");
    }
}

APIService::~APIService() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
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
    
    if (!curl) {
        response.error = "CURL not initialized";
        return response;
    }
    
    // Reset curl handle
    curl_easy_reset(curl);
    
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    // Set method
    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (!body.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }
    } else if (method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        if (!body.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }
    } else if (method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else if (method == "PATCH") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        if (!body.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }
    }
    
    // Set headers
    struct curl_slist* headers = nullptr;
    if (!body.empty()) {
        headers = curl_slist_append(headers, "Content-Type: application/json");
    }
    
    if (!ignoreAuth && !currentAuthToken.empty()) {
        std::string authHeader = "Authorization: Bearer " + currentAuthToken;
        headers = curl_slist_append(headers, authHeader.c_str());
    }
    
    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    
    // Set callback for response data
    std::string responseData;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
    
    // Set SSL options
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    
    // Set timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        response.error = curl_easy_strerror(res);
        LOGE("CURL error: %s", response.error.c_str());
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);
        response.data = responseData;
        LOGI("Request to %s completed with status %ld", url.c_str(), response.statusCode);
    }
    
    // Cleanup
    if (headers) {
        curl_slist_free_all(headers);
    }
    
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

} // namespace localify
