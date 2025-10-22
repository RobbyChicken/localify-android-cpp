package com.localify.android;

public class LocalifyNative {
    
    // Load the native library
    static {
        System.loadLibrary("localify");
    }
    
    // Authentication methods
    public native String createGuestUser();
    public native String exchangeToken(String token, String secret);
    public native String refreshAuth(boolean force);
    
    // User methods
    public native String fetchUserDetails();
    public native void setAuthToken(String token);
    public native String getAuthToken();
    public native void clearAuth();
    
    // Search methods
    public native String fetchSearch(String text, boolean autoSearchSpotify);
    public native String fetchSearchArtists(String text, int limit);
    
    // Favorites methods (type: 0=ARTISTS, 1=EVENTS, 2=VENUES)
    public native void addFavorite(String id, int type);
    public native void removeFavorite(String id, int type);
    
    // Utility methods
    public native String getVersion();
}
