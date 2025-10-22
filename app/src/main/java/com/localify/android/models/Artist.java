package com.localify.android.models;

import java.util.List;

public class Artist {
    private String id;
    private String name;
    private String imageUrl;
    private String spotifyId;
    private List<String> genres;
    private int popularity;
    private boolean isFavorite;
    
    public Artist() {}
    
    public Artist(String id, String name, String imageUrl, int popularity, boolean isFavorite) {
        this.id = id;
        this.name = name;
        this.imageUrl = imageUrl;
        this.popularity = popularity;
        this.isFavorite = isFavorite;
    }
    
    // Getters and setters
    public String getId() { return id; }
    public void setId(String id) { this.id = id; }
    
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    
    public String getImageUrl() { return imageUrl; }
    public void setImageUrl(String imageUrl) { this.imageUrl = imageUrl; }
    
    public String getSpotifyId() { return spotifyId; }
    public void setSpotifyId(String spotifyId) { this.spotifyId = spotifyId; }
    
    public List<String> getGenres() { return genres; }
    public void setGenres(List<String> genres) { this.genres = genres; }
    
    public int getPopularity() { return popularity; }
    public void setPopularity(int popularity) { this.popularity = popularity; }
    
    public boolean isFavorite() { return isFavorite; }
    public void setFavorite(boolean favorite) { isFavorite = favorite; }
}
