#ifndef LOCALIFY_MANIFEST_H
#define LOCALIFY_MANIFEST_H

// Pure C++ Android Manifest equivalent
// This replaces AndroidManifest.xml with compile-time constants

namespace localify {

struct AndroidManifest {
    // Application info
    static constexpr const char* PACKAGE_NAME = "com.localify.android";
    static constexpr const char* APPLICATION_LABEL = "Localify";
    static constexpr int VERSION_CODE = 1;
    static constexpr const char* VERSION_NAME = "1.0.0";
    
    // SDK versions
    static constexpr int MIN_SDK_VERSION = 24;
    static constexpr int TARGET_SDK_VERSION = 34;
    static constexpr int COMPILE_SDK_VERSION = 34;
    
    // Permissions
    struct Permissions {
        static constexpr const char* INTERNET = "android.permission.INTERNET";
        static constexpr const char* ACCESS_NETWORK_STATE = "android.permission.ACCESS_NETWORK_STATE";
        static constexpr const char* ACCESS_FINE_LOCATION = "android.permission.ACCESS_FINE_LOCATION";
        static constexpr const char* ACCESS_COARSE_LOCATION = "android.permission.ACCESS_COARSE_LOCATION";
    };
    
    // Features
    struct Features {
        static constexpr const char* OPENGL_ES_2_0 = "android.hardware.opengles.aep";
        static constexpr int GLES_VERSION = 0x00020000; // OpenGL ES 2.0
        static constexpr bool GLES_REQUIRED = true;
    };
    
    // Application settings
    struct Application {
        static constexpr bool ALLOW_BACKUP = true;
        static constexpr bool SUPPORTS_RTL = true;
        static constexpr bool DEBUGGABLE = true;
        static constexpr const char* THEME = "@android:style/Theme.NoTitleBar.Fullscreen";
        static constexpr const char* NATIVE_LIBRARY = "localify";
    };
    
    // Activity settings
    struct Activity {
        static constexpr const char* NAME = "android.app.NativeActivity";
        static constexpr bool EXPORTED = true;
        static constexpr const char* LABEL = "Localify";
        static constexpr const char* CONFIG_CHANGES = "orientation|keyboardHidden|screenSize";
        static constexpr const char* SCREEN_ORIENTATION = "portrait";
        static constexpr const char* LAUNCH_MODE = "singleTask";
        
        // Intent filters
        struct IntentFilters {
            // Main launcher intent
            struct Main {
                static constexpr const char* ACTION = "android.intent.action.MAIN";
                static constexpr const char* CATEGORY = "android.intent.category.LAUNCHER";
            };
            
            // Deep link intent for Spotify OAuth
            struct DeepLink {
                static constexpr const char* ACTION = "android.intent.action.VIEW";
                static constexpr const char* CATEGORY_DEFAULT = "android.intent.category.DEFAULT";
                static constexpr const char* CATEGORY_BROWSABLE = "android.intent.category.BROWSABLE";
                static constexpr const char* SCHEME = "localify";
            };
        };
        
        // Meta-data
        struct MetaData {
            static constexpr const char* LIB_NAME_KEY = "android.app.lib_name";
            static constexpr const char* LIB_NAME_VALUE = "localify";
        };
    };
    
    // Network security config (for HTTPS API calls)
    struct NetworkSecurity {
        static constexpr bool CLEAR_TEXT_TRAFFIC_PERMITTED = false;
        static constexpr const char* TRUSTED_CA = "system";
    };
};

} // namespace localify

#endif // LOCALIFY_MANIFEST_H
