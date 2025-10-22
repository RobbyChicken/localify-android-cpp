#include "json_parser.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <android/log.h>

#define LOG_TAG "LocalifyJSON"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace localify {

// Helper function to extract string value from JSON
std::string JSONParser::extractStringValue(const std::string& json, const std::string& key) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]*?)\"");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        return unescapeJsonString(match[1].str());
    }
    return "";
}

int JSONParser::extractIntValue(const std::string& json, const std::string& key) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*([0-9]+)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        return std::stoi(match[1].str());
    }
    return 0;
}

double JSONParser::extractDoubleValue(const std::string& json, const std::string& key) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*([0-9]+\\.?[0-9]*)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        return std::stod(match[1].str());
    }
    return 0.0;
}

bool JSONParser::extractBoolValue(const std::string& json, const std::string& key) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*(true|false)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        return match[1].str() == "true";
    }
    return false;
}

std::optional<std::string> JSONParser::extractOptionalStringValue(const std::string& json, const std::string& key) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*(\"([^\"]*?)\"|null)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        if (match[1].str() == "null") {
            return std::nullopt;
        }
        return unescapeJsonString(match[2].str());
    }
    return std::nullopt;
}

std::optional<int> JSONParser::extractOptionalIntValue(const std::string& json, const std::string& key) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*(([0-9]+)|null)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        if (match[1].str() == "null") {
            return std::nullopt;
        }
        return std::stoi(match[2].str());
    }
    return std::nullopt;
}

std::string JSONParser::findJsonArray(const std::string& json, const std::string& key) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*(\\[.*?\\])");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        return match[1].str();
    }
    return "[]";
}

std::vector<std::string> JSONParser::splitJsonArray(const std::string& jsonArray) {
    std::vector<std::string> result;
    if (jsonArray.length() < 2) return result;
    
    std::string content = jsonArray.substr(1, jsonArray.length() - 2); // Remove [ and ]
    if (content.empty()) return result;
    
    // Simple JSON object splitting (assumes objects don't contain nested arrays)
    int braceCount = 0;
    int start = 0;
    bool inString = false;
    bool escaped = false;
    
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        
        if (!inString) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            else if (c == '"') inString = true;
            else if (c == ',' && braceCount == 0) {
                result.push_back(trim(content.substr(start, i - start)));
                start = i + 1;
            }
        } else {
            if (!escaped) {
                if (c == '"') inString = false;
                else if (c == '\\') escaped = true;
            } else {
                escaped = false;
            }
        }
    }
    
    if (start < content.length()) {
        result.push_back(trim(content.substr(start)));
    }
    
    return result;
}

std::string JSONParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string JSONParser::unescapeJsonString(const std::string& str) {
    std::string result = str;
    std::regex escapePattern("\\\\([\"\\\\/bfnrt])");
    result = std::regex_replace(result, escapePattern, "$1");
    return result;
}

std::string JSONParser::escapeJsonString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

// Parse AuthResponse
AuthResponse JSONParser::parseAuthResponse(const std::string& json) {
    AuthResponse auth;
    auth.token = extractStringValue(json, "token");
    auth.refreshToken = extractStringValue(json, "refreshToken");
    auth.expiresIn = extractIntValue(json, "expiresIn");
    return auth;
}

// Parse UserDetails
UserDetails JSONParser::parseUserDetails(const std::string& json) {
    UserDetails user;
    user.id = extractStringValue(json, "id");
    user.name = extractStringValue(json, "name");
    user.email = extractOptionalStringValue(json, "email");
    user.appleId = extractOptionalStringValue(json, "appleId");
    user.spotifyId = extractOptionalStringValue(json, "spotifyId");
    user.accountCreationDate = extractIntValue(json, "accountCreationDate");
    user.profileImage = extractOptionalStringValue(json, "profileImage");
    user.spotifyProfileImage = extractOptionalStringValue(json, "spotifyProfileImage");
    user.playlistLocalSongsPerSeed = extractOptionalIntValue(json, "playlistLocalSongsPerSeed");
    user.anonymousUser = extractBoolValue(json, "anonymousUser");
    user.emailConnected = extractBoolValue(json, "emailConnected");
    user.appleConnected = extractBoolValue(json, "appleConnected");
    user.spotifyConnected = extractBoolValue(json, "spotifyConnected");
    user.emailVerified = extractBoolValue(json, "emailVerified");
    user.emailOptIn = extractBoolValue(json, "emailOptIn");
    user.isAdmin = extractBoolValue(json, "isAdmin");
    user.isTeamMember = extractBoolValue(json, "isTeamMember");
    user.playlistUseSeedSongs = extractBoolValue(json, "playlistUseSeedSongs");
    user.playlistGeneration = extractBoolValue(json, "playlistGeneration");
    return user;
}

// Parse ArtistResponse
ArtistResponse JSONParser::parseArtistResponse(const std::string& json) {
    ArtistResponse artist;
    artist.id = extractStringValue(json, "id");
    artist.name = extractStringValue(json, "name");
    artist.imageUrl = extractOptionalStringValue(json, "imageUrl");
    artist.spotifyId = extractOptionalStringValue(json, "spotifyId");
    artist.popularity = extractIntValue(json, "popularity");
    artist.isFavorite = extractBoolValue(json, "isFavorite");
    
    // Parse genres array
    std::string genresArray = findJsonArray(json, "genres");
    std::vector<std::string> genreObjects = splitJsonArray(genresArray);
    for (const std::string& genreStr : genreObjects) {
        // Remove quotes if it's a simple string
        std::string genre = genreStr;
        if (genre.front() == '"' && genre.back() == '"') {
            genre = genre.substr(1, genre.length() - 2);
        }
        artist.genres.push_back(genre);
    }
    
    return artist;
}

// Parse EventResponse
EventResponse JSONParser::parseEventResponse(const std::string& json) {
    EventResponse event;
    event.id = extractStringValue(json, "id");
    event.name = extractStringValue(json, "name");
    event.description = extractStringValue(json, "description");
    event.startDate = extractStringValue(json, "startDate");
    event.endDate = extractStringValue(json, "endDate");
    event.imageUrl = extractOptionalStringValue(json, "imageUrl");
    event.venueId = extractStringValue(json, "venueId");
    event.venueName = extractStringValue(json, "venueName");
    event.isFavorite = extractBoolValue(json, "isFavorite");
    event.latitude = extractDoubleValue(json, "latitude");
    event.longitude = extractDoubleValue(json, "longitude");
    
    // Parse artists array
    std::string artistsArray = findJsonArray(json, "artists");
    std::vector<std::string> artistObjects = splitJsonArray(artistsArray);
    for (const std::string& artistJson : artistObjects) {
        event.artists.push_back(parseArtistResponse(artistJson));
    }
    
    return event;
}

// Parse VenueResponse
VenueResponse JSONParser::parseVenueResponse(const std::string& json) {
    VenueResponse venue;
    venue.id = extractStringValue(json, "id");
    venue.name = extractStringValue(json, "name");
    venue.address = extractStringValue(json, "address");
    venue.city = extractStringValue(json, "city");
    venue.state = extractStringValue(json, "state");
    venue.country = extractStringValue(json, "country");
    venue.latitude = extractDoubleValue(json, "latitude");
    venue.longitude = extractDoubleValue(json, "longitude");
    venue.imageUrl = extractOptionalStringValue(json, "imageUrl");
    venue.isFavorite = extractBoolValue(json, "isFavorite");
    return venue;
}

// Parse CityResponse
CityResponse JSONParser::parseCityResponse(const std::string& json) {
    CityResponse city;
    city.id = extractStringValue(json, "id");
    city.name = extractStringValue(json, "name");
    city.state = extractStringValue(json, "state");
    city.country = extractStringValue(json, "country");
    city.latitude = extractDoubleValue(json, "latitude");
    city.longitude = extractDoubleValue(json, "longitude");
    return city;
}

// Parse SearchResponse
SearchResponse JSONParser::parseSearchResponse(const std::string& json) {
    SearchResponse search;
    
    // Parse artists array
    std::string artistsArray = findJsonArray(json, "artists");
    search.artists = parseArtistArray(artistsArray);
    
    // Parse events array
    std::string eventsArray = findJsonArray(json, "events");
    search.events = parseEventArray(eventsArray);
    
    // Parse venues array
    std::string venuesArray = findJsonArray(json, "venues");
    search.venues = parseVenueArray(venuesArray);
    
    // Parse cities array
    std::string citiesArray = findJsonArray(json, "cities");
    search.cities = parseCityArray(citiesArray);
    
    return search;
}

// Parse arrays
std::vector<ArtistResponse> JSONParser::parseArtistArray(const std::string& json) {
    std::vector<ArtistResponse> artists;
    std::vector<std::string> artistObjects = splitJsonArray(json);
    for (const std::string& artistJson : artistObjects) {
        artists.push_back(parseArtistResponse(artistJson));
    }
    return artists;
}

std::vector<EventResponse> JSONParser::parseEventArray(const std::string& json) {
    std::vector<EventResponse> events;
    std::vector<std::string> eventObjects = splitJsonArray(json);
    for (const std::string& eventJson : eventObjects) {
        events.push_back(parseEventResponse(eventJson));
    }
    return events;
}

std::vector<VenueResponse> JSONParser::parseVenueArray(const std::string& json) {
    std::vector<VenueResponse> venues;
    std::vector<std::string> venueObjects = splitJsonArray(json);
    for (const std::string& venueJson : venueObjects) {
        venues.push_back(parseVenueResponse(venueJson));
    }
    return venues;
}

std::vector<CityResponse> JSONParser::parseCityArray(const std::string& json) {
    std::vector<CityResponse> cities;
    std::vector<std::string> cityObjects = splitJsonArray(json);
    for (const std::string& cityJson : cityObjects) {
        cities.push_back(parseCityResponse(cityJson));
    }
    return cities;
}

} // namespace localify
