#include "jni_bridge.h"
#include "api_service.h"
#include "json_parser.h"
#include <android/log.h>

#define LOG_TAG "LocalifyJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace localify;

// Helper functions
std::string jstring_to_string(JNIEnv *env, jstring jstr) {
    if (!jstr) return "";
    
    const char *chars = env->GetStringUTFChars(jstr, nullptr);
    std::string result(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    return result;
}

jstring string_to_jstring(JNIEnv *env, const std::string& str) {
    return env->NewStringUTF(str.c_str());
}

// Convert C++ AuthResponse to JSON string
std::string authResponseToJson(const AuthResponse& auth) {
    return "{\"token\":\"" + auth.token + 
           "\",\"refreshToken\":\"" + auth.refreshToken + 
           "\",\"expiresIn\":" + std::to_string(auth.expiresIn) + "}";
}

// Convert C++ UserDetails to JSON string
std::string userDetailsToJson(const UserDetails& user) {
    std::string json = "{";
    json += "\"id\":\"" + user.id + "\",";
    json += "\"name\":\"" + user.name + "\",";
    
    if (user.email.has_value()) {
        json += "\"email\":\"" + user.email.value() + "\",";
    } else {
        json += "\"email\":null,";
    }
    
    json += "\"anonymousUser\":" + std::string(user.anonymousUser ? "true" : "false") + ",";
    json += "\"emailConnected\":" + std::string(user.emailConnected ? "true" : "false") + ",";
    json += "\"spotifyConnected\":" + std::string(user.spotifyConnected ? "true" : "false");
    json += "}";
    
    return json;
}

// Convert C++ SearchResponse to JSON string
std::string searchResponseToJson(const SearchResponse& search) {
    std::string json = "{";
    
    // Artists array
    json += "\"artists\":[";
    for (size_t i = 0; i < search.artists.size(); ++i) {
        if (i > 0) json += ",";
        const auto& artist = search.artists[i];
        json += "{\"id\":\"" + artist.id + "\",\"name\":\"" + artist.name + "\",\"popularity\":" + std::to_string(artist.popularity) + "}";
    }
    json += "],";
    
    // Events array
    json += "\"events\":[";
    for (size_t i = 0; i < search.events.size(); ++i) {
        if (i > 0) json += ",";
        const auto& event = search.events[i];
        json += "{\"id\":\"" + event.id + "\",\"name\":\"" + event.name + "\",\"venueName\":\"" + event.venueName + "\"}";
    }
    json += "],";
    
    // Venues array
    json += "\"venues\":[";
    for (size_t i = 0; i < search.venues.size(); ++i) {
        if (i > 0) json += ",";
        const auto& venue = search.venues[i];
        json += "{\"id\":\"" + venue.id + "\",\"name\":\"" + venue.name + "\",\"city\":\"" + venue.city + "\"}";
    }
    json += "],";
    
    // Cities array
    json += "\"cities\":[";
    for (size_t i = 0; i < search.cities.size(); ++i) {
        if (i > 0) json += ",";
        const auto& city = search.cities[i];
        json += "{\"id\":\"" + city.id + "\",\"name\":\"" + city.name + "\",\"country\":\"" + city.country + "\"}";
    }
    json += "]";
    
    json += "}";
    return json;
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_createGuestUser(JNIEnv *env, jobject thiz) {
    try {
        LOGI("Creating guest user");
        auto future = APIService::getInstance().createGuestUser();
        AuthResponse auth = future.get();
        std::string jsonResult = authResponseToJson(auth);
        LOGI("Guest user created successfully");
        return string_to_jstring(env, jsonResult);
    } catch (const std::exception& e) {
        LOGE("Error creating guest user: %s", e.what());
        std::string errorJson = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return string_to_jstring(env, errorJson);
    }
}

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_exchangeToken(JNIEnv *env, jobject thiz, 
                                                       jstring token, jstring secret) {
    try {
        std::string tokenStr = jstring_to_string(env, token);
        std::string secretStr = jstring_to_string(env, secret);
        
        LOGI("Exchanging token");
        auto future = APIService::getInstance().exchangeToken(tokenStr, secretStr);
        AuthResponse auth = future.get();
        std::string jsonResult = authResponseToJson(auth);
        LOGI("Token exchanged successfully");
        return string_to_jstring(env, jsonResult);
    } catch (const std::exception& e) {
        LOGE("Error exchanging token: %s", e.what());
        std::string errorJson = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return string_to_jstring(env, errorJson);
    }
}

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_refreshAuth(JNIEnv *env, jobject thiz, jboolean force) {
    try {
        LOGI("Refreshing auth token");
        auto future = APIService::getInstance().refreshAuth(force == JNI_TRUE);
        AuthResponse auth = future.get();
        std::string jsonResult = authResponseToJson(auth);
        LOGI("Auth token refreshed successfully");
        return string_to_jstring(env, jsonResult);
    } catch (const std::exception& e) {
        LOGE("Error refreshing auth: %s", e.what());
        std::string errorJson = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return string_to_jstring(env, errorJson);
    }
}

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_fetchUserDetails(JNIEnv *env, jobject thiz) {
    try {
        LOGI("Fetching user details");
        auto future = APIService::getInstance().fetchUserDetails();
        UserDetails user = future.get();
        std::string jsonResult = userDetailsToJson(user);
        LOGI("User details fetched successfully");
        return string_to_jstring(env, jsonResult);
    } catch (const std::exception& e) {
        LOGE("Error fetching user details: %s", e.what());
        std::string errorJson = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return string_to_jstring(env, errorJson);
    }
}

JNIEXPORT void JNICALL
Java_com_localify_android_LocalifyNative_setAuthToken(JNIEnv *env, jobject thiz, jstring token) {
    std::string tokenStr = jstring_to_string(env, token);
    APIService::getInstance().setAuthToken(tokenStr);
    LOGI("Auth token set");
}

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_getAuthToken(JNIEnv *env, jobject thiz) {
    std::string token = APIService::getInstance().getAuthToken();
    return string_to_jstring(env, token);
}

JNIEXPORT void JNICALL
Java_com_localify_android_LocalifyNative_clearAuth(JNIEnv *env, jobject thiz) {
    APIService::getInstance().clearAuth();
    LOGI("Auth cleared");
}

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_fetchSearch(JNIEnv *env, jobject thiz, 
                                                     jstring text, jboolean autoSearchSpotify) {
    try {
        std::string textStr = jstring_to_string(env, text);
        bool autoSearch = (autoSearchSpotify == JNI_TRUE);
        
        LOGI("Performing search for: %s", textStr.c_str());
        auto future = APIService::getInstance().fetchSearch(textStr, autoSearch);
        SearchResponse search = future.get();
        std::string jsonResult = searchResponseToJson(search);
        LOGI("Search completed successfully");
        return string_to_jstring(env, jsonResult);
    } catch (const std::exception& e) {
        LOGE("Error performing search: %s", e.what());
        std::string errorJson = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return string_to_jstring(env, errorJson);
    }
}

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_fetchSearchArtists(JNIEnv *env, jobject thiz, 
                                                            jstring text, jint limit) {
    try {
        std::string textStr = jstring_to_string(env, text);
        int limitInt = static_cast<int>(limit);
        
        LOGI("Searching artists for: %s", textStr.c_str());
        auto future = APIService::getInstance().fetchSearchArtists(textStr, limitInt);
        std::vector<ArtistResponse> artists = future.get();
        
        // Convert to JSON array
        std::string json = "[";
        for (size_t i = 0; i < artists.size(); ++i) {
            if (i > 0) json += ",";
            const auto& artist = artists[i];
            json += "{\"id\":\"" + artist.id + "\",\"name\":\"" + artist.name + "\",\"popularity\":" + std::to_string(artist.popularity) + "}";
        }
        json += "]";
        
        LOGI("Artist search completed successfully");
        return string_to_jstring(env, json);
    } catch (const std::exception& e) {
        LOGE("Error searching artists: %s", e.what());
        std::string errorJson = "{\"error\":\"" + std::string(e.what()) + "\"}";
        return string_to_jstring(env, errorJson);
    }
}

JNIEXPORT void JNICALL
Java_com_localify_android_LocalifyNative_addFavorite(JNIEnv *env, jobject thiz, 
                                                     jstring id, jint type) {
    try {
        std::string idStr = jstring_to_string(env, id);
        FavoriteType favoriteType = static_cast<FavoriteType>(type);
        
        LOGI("Adding favorite: %s", idStr.c_str());
        auto future = APIService::getInstance().addFavorite(idStr, favoriteType);
        future.get(); // Wait for completion
        LOGI("Favorite added successfully");
    } catch (const std::exception& e) {
        LOGE("Error adding favorite: %s", e.what());
    }
}

JNIEXPORT void JNICALL
Java_com_localify_android_LocalifyNative_removeFavorite(JNIEnv *env, jobject thiz, 
                                                        jstring id, jint type) {
    try {
        std::string idStr = jstring_to_string(env, id);
        FavoriteType favoriteType = static_cast<FavoriteType>(type);
        
        LOGI("Removing favorite: %s", idStr.c_str());
        auto future = APIService::getInstance().removeFavorite(idStr, favoriteType);
        future.get(); // Wait for completion
        LOGI("Favorite removed successfully");
    } catch (const std::exception& e) {
        LOGE("Error removing favorite: %s", e.what());
    }
}

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_getVersion(JNIEnv *env, jobject thiz) {
    return string_to_jstring(env, "Localify Android C++ v1.0.0");
}

}
