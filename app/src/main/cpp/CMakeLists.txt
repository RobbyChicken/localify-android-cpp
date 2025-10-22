cmake_minimum_required(VERSION 3.22.1)

project("localify")

set(CMAKE_CXX_STANDARD 17)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add source files explicitly
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/json_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/android_ui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screens.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/map_screen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/native_activity_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/http_client.cpp
)

# Create shared library
add_library(localify SHARED ${SOURCES})

# Find required libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv2-lib GLESv2)

# Add native app glue
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# Include native app glue headers
target_include_directories(native_app_glue PUBLIC
    ${ANDROID_NDK}/sources/android/native_app_glue)

# For now, we'll use a simple HTTP implementation instead of curl
# In a production app, you'd want to include prebuilt curl libraries

target_link_libraries(localify
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv2-lib}
    native_app_glue
)
