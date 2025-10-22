package com.localify.android.models;

public class User {
    private String id;
    private String name;
    private String email;
    private boolean anonymousUser;
    private boolean emailConnected;
    private boolean spotifyConnected;
    private boolean appleConnected;
    private boolean emailVerified;
    private boolean isAdmin;
    
    public User() {}
    
    public User(String id, String name, String email, boolean anonymousUser, 
                boolean emailConnected, boolean spotifyConnected) {
        this.id = id;
        this.name = name;
        this.email = email;
        this.anonymousUser = anonymousUser;
        this.emailConnected = emailConnected;
        this.spotifyConnected = spotifyConnected;
    }
    
    // Getters and setters
    public String getId() { return id; }
    public void setId(String id) { this.id = id; }
    
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    
    public String getEmail() { return email; }
    public void setEmail(String email) { this.email = email; }
    
    public boolean isAnonymousUser() { return anonymousUser; }
    public void setAnonymousUser(boolean anonymousUser) { this.anonymousUser = anonymousUser; }
    
    public boolean isEmailConnected() { return emailConnected; }
    public void setEmailConnected(boolean emailConnected) { this.emailConnected = emailConnected; }
    
    public boolean isSpotifyConnected() { return spotifyConnected; }
    public void setSpotifyConnected(boolean spotifyConnected) { this.spotifyConnected = spotifyConnected; }
    
    public boolean isAppleConnected() { return appleConnected; }
    public void setAppleConnected(boolean appleConnected) { this.appleConnected = appleConnected; }
    
    public boolean isEmailVerified() { return emailVerified; }
    public void setEmailVerified(boolean emailVerified) { this.emailVerified = emailVerified; }
    
    public boolean isAdmin() { return isAdmin; }
    public void setAdmin(boolean admin) { isAdmin = admin; }
}
