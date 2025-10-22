#ifndef LOCALIFY_JSON_PARSER_H
#define LOCALIFY_JSON_PARSER_H

#include "models.h"
#include <string>
#include <vector>

namespace localify {

class JSONParser {
public:
    // Parse individual objects
    static AuthResponse parseAuthResponse(const std::string& json);
    static UserDetails parseUserDetails(const std::string& json);
    static ArtistResponse parseArtistResponse(const std::string& json);
    static EventResponse parseEventResponse(const std::string& json);
    static VenueResponse parseVenueResponse(const std::string& json);
    static CityResponse parseCityResponse(const std::string& json);
    static UserCity parseUserCity(const std::string& json);
    static SearchResponse parseSearchResponse(const std::string& json);
    static ErrorResponse parseErrorResponse(const std::string& json);
    
    // Parse arrays
    static std::vector<ArtistResponse> parseArtistArray(const std::string& json);
    static std::vector<EventResponse> parseEventArray(const std::string& json);
    static std::vector<VenueResponse> parseVenueArray(const std::string& json);
    static std::vector<CityResponse> parseCityArray(const std::string& json);
    static std::vector<UserCity> parseUserCityArray(const std::string& json);
    static std::vector<std::string> parseStringArray(const std::string& json);
    
    // Serialize objects to JSON
    static std::string serializeUserDetails(const UserDetails& user);
    static std::string serializeUserCity(const UserCity& userCity);
    static std::string serializeStringArray(const std::vector<std::string>& strings);
    
private:
    // Helper methods for parsing
    static std::string extractStringValue(const std::string& json, const std::string& key);
    static int extractIntValue(const std::string& json, const std::string& key);
    static double extractDoubleValue(const std::string& json, const std::string& key);
    static bool extractBoolValue(const std::string& json, const std::string& key);
    static std::optional<std::string> extractOptionalStringValue(const std::string& json, const std::string& key);
    static std::optional<int> extractOptionalIntValue(const std::string& json, const std::string& key);
    
    // Helper methods for array parsing
    static std::vector<std::string> splitJsonArray(const std::string& jsonArray);
    static std::string findJsonObject(const std::string& json, const std::string& key);
    static std::string findJsonArray(const std::string& json, const std::string& key);
    
    // String utilities
    static std::string trim(const std::string& str);
    static std::string escapeJsonString(const std::string& str);
    static std::string unescapeJsonString(const std::string& str);
};

} // namespace localify

#endif // LOCALIFY_JSON_PARSER_H
