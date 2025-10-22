#ifndef LOCALIFY_JNI_BRIDGE_H
#define LOCALIFY_JNI_BRIDGE_H

#include <jni.h>
#include <string>

extern "C" {

// Authentication methods
JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_createGuestUser(JNIEnv *env, jobject thiz);

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_exchangeToken(JNIEnv *env, jobject thiz, 
                                                       jstring token, jstring secret);

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_refreshAuth(JNIEnv *env, jobject thiz, jboolean force);

// User methods
JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_fetchUserDetails(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_localify_android_LocalifyNative_setAuthToken(JNIEnv *env, jobject thiz, jstring token);

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_getAuthToken(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_localify_android_LocalifyNative_clearAuth(JNIEnv *env, jobject thiz);

// Search methods
JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_fetchSearch(JNIEnv *env, jobject thiz, 
                                                     jstring text, jboolean autoSearchSpotify);

JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_fetchSearchArtists(JNIEnv *env, jobject thiz, 
                                                            jstring text, jint limit);

// Favorites methods
JNIEXPORT void JNICALL
Java_com_localify_android_LocalifyNative_addFavorite(JNIEnv *env, jobject thiz, 
                                                     jstring id, jint type);

JNIEXPORT void JNICALL
Java_com_localify_android_LocalifyNative_removeFavorite(JNIEnv *env, jobject thiz, 
                                                        jstring id, jint type);

// Utility methods
JNIEXPORT jstring JNICALL
Java_com_localify_android_LocalifyNative_getVersion(JNIEnv *env, jobject thiz);

}

// Helper functions for JNI
std::string jstring_to_string(JNIEnv *env, jstring jstr);
jstring string_to_jstring(JNIEnv *env, const std::string& str);

#endif // LOCALIFY_JNI_BRIDGE_H
