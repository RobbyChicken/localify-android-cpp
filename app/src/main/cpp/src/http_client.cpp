#include "http_client.h"
#include "app_config.h"
#include <android/log.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sstream>
#include <thread>
#include <regex>

#define LOG_TAG "LocalifyHTTP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace localify {

std::unique_ptr<HttpClient> HttpClient::instance = nullptr;

HttpClient::HttpClient() : userAgent("Localify-Android-CPP/1.0"), defaultTimeoutSeconds(30) {
    LOGI("HttpClient initialized");
}

HttpClient::~HttpClient() {
    LOGI("HttpClient destroyed");
}

HttpClient& HttpClient::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<HttpClient>(new HttpClient());
    }
    return *instance;
}

HttpResponse HttpClient::get(const std::string& url, const std::map<std::string, std::string>& headers) {
    HttpRequest request(url, "GET");
    for (const auto& header : headers) {
        request.setHeader(header.first, header.second);
    }
    return performRequest(request);
}

HttpResponse HttpClient::post(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers) {
    HttpRequest request(url, "POST");
    request.body = body;
    for (const auto& header : headers) {
        request.setHeader(header.first, header.second);
    }
    if (!body.empty() && headers.find("Content-Type") == headers.end()) {
        request.setContentType("application/json");
    }
    return performRequest(request);
}

HttpResponse HttpClient::put(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers) {
    HttpRequest request(url, "PUT");
    request.body = body;
    for (const auto& header : headers) {
        request.setHeader(header.first, header.second);
    }
    if (!body.empty() && headers.find("Content-Type") == headers.end()) {
        request.setContentType("application/json");
    }
    return performRequest(request);
}

HttpResponse HttpClient::patch(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers) {
    HttpRequest request(url, "PATCH");
    request.body = body;
    for (const auto& header : headers) {
        request.setHeader(header.first, header.second);
    }
    if (!body.empty() && headers.find("Content-Type") == headers.end()) {
        request.setContentType("application/json");
    }
    return performRequest(request);
}

HttpResponse HttpClient::delete_(const std::string& url, const std::map<std::string, std::string>& headers) {
    HttpRequest request(url, "DELETE");
    for (const auto& header : headers) {
        request.setHeader(header.first, header.second);
    }
    return performRequest(request);
}

HttpResponse HttpClient::request(const HttpRequest& request) {
    return performRequest(request);
}

std::future<HttpResponse> HttpClient::getAsync(const std::string& url, const std::map<std::string, std::string>& headers) {
    return std::async(std::launch::async, [this, url, headers]() {
        return get(url, headers);
    });
}

std::future<HttpResponse> HttpClient::postAsync(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers) {
    return std::async(std::launch::async, [this, url, body, headers]() {
        return post(url, body, headers);
    });
}

std::future<HttpResponse> HttpClient::requestAsync(const HttpRequest& request) {
    return std::async(std::launch::async, [this, request]() {
        return performRequest(request);
    });
}

void HttpClient::setUserAgent(const std::string& userAgent) {
    this->userAgent = userAgent;
}

void HttpClient::setDefaultTimeout(int seconds) {
    defaultTimeoutSeconds = seconds;
}

HttpResponse HttpClient::performRequest(const HttpRequest& request) {
    HttpResponse response;
    
    LOGI("Performing %s request to: %s", request.method.c_str(), request.url.c_str());
    
    // Parse URL to extract host, port, and path
    std::regex urlRegex(R"(^https?://([^:/]+)(?::(\d+))?(/.*)?$)");
    std::smatch matches;
    
    if (!std::regex_match(request.url, matches, urlRegex)) {
        response.error = "Invalid URL format";
        LOGE("Invalid URL: %s", request.url.c_str());
        return response;
    }
    
    std::string host = matches[1].str();
    int port = matches[2].matched ? std::stoi(matches[2].str()) : (request.url.find("https://") == 0 ? 443 : 80);
    std::string path = matches[3].matched ? matches[3].str() : "/";
    bool isHttps = request.url.find("https://") == 0;
    
    LOGI("Connecting to %s:%d%s (HTTPS: %s)", host.c_str(), port, path.c_str(), isHttps ? "yes" : "no");
    
    // For HTTPS, we'd need to implement SSL/TLS support
    // For now, we'll simulate the response for development
    if (isHttps) {
        // Simulate API responses for development
        response.statusCode = 200;
        
        if (request.url.find("/v1/auth/guest") != std::string::npos) {
            response.body = R"({"token":"guest_token_123","refreshToken":"refresh_token_456","expiresIn":3600})";
        } else if (request.url.find("/v1/@me") != std::string::npos) {
            response.body = R"({"id":"user123","name":"Guest User","anonymousUser":true,"emailConnected":false,"spotifyConnected":false,"appleConnected":false,"emailVerified":false,"emailOptIn":false,"isAdmin":false,"isTeamMember":false,"playlistUseSeedSongs":false,"playlistGeneration":false})";
        } else if (request.url.find("/v1/search") != std::string::npos) {
            response.body = R"({"artists":[{"id":"artist1","name":"Arctic Monkeys","popularity":85}],"events":[{"id":"event1","name":"Concert Tonight","venueName":"The Fillmore"}],"venues":[{"id":"venue1","name":"The Fillmore","city":"San Francisco"}],"cities":[{"id":"city1","name":"San Francisco","country":"USA"}]})";
        } else {
            response.body = R"({"message":"API endpoint not implemented in development mode"})";
        }
        
        LOGI("Simulated HTTPS response (development mode)");
        return response;
    }
    
    // For HTTP (development/testing), implement basic socket connection
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        response.error = "Host not found: " + host;
        LOGE("Host not found: %s", host.c_str());
        return response;
    }
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        response.error = "Failed to create socket";
        LOGE("Failed to create socket");
        return response;
    }
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        response.error = "Failed to connect to server";
        LOGE("Failed to connect to %s:%d", host.c_str(), port);
        close(sockfd);
        return response;
    }
    
    // Build HTTP request
    std::ostringstream requestStream;
    requestStream << request.method << " " << path << " HTTP/1.1\r\n";
    requestStream << "Host: " << host << "\r\n";
    requestStream << "User-Agent: " << userAgent << "\r\n";
    requestStream << "Connection: close\r\n";
    
    // Add custom headers
    for (const auto& header : request.headers) {
        requestStream << header.first << ": " << header.second << "\r\n";
    }
    
    // Add content length if body exists
    if (!request.body.empty()) {
        requestStream << "Content-Length: " << request.body.length() << "\r\n";
    }
    
    requestStream << "\r\n";
    
    // Add body if exists
    if (!request.body.empty()) {
        requestStream << request.body;
    }
    
    std::string requestStr = requestStream.str();
    LOGI("Sending HTTP request (%zu bytes)", requestStr.length());
    
    // Send request
    if (send(sockfd, requestStr.c_str(), requestStr.length(), 0) < 0) {
        response.error = "Failed to send request";
        LOGE("Failed to send request");
        close(sockfd);
        return response;
    }
    
    // Read response
    std::string responseStr;
    char buffer[4096];
    ssize_t bytesRead;
    
    while ((bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        responseStr += buffer;
    }
    
    close(sockfd);
    
    if (responseStr.empty()) {
        response.error = "Empty response from server";
        LOGE("Empty response from server");
        return response;
    }
    
    // Parse HTTP response
    size_t headerEnd = responseStr.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        response.error = "Invalid HTTP response format";
        LOGE("Invalid HTTP response format");
        return response;
    }
    
    std::string headerSection = responseStr.substr(0, headerEnd);
    response.body = responseStr.substr(headerEnd + 4);
    
    // Parse status line
    size_t firstLine = headerSection.find("\r\n");
    std::string statusLine = headerSection.substr(0, firstLine);
    
    std::regex statusRegex(R"(HTTP/\d\.\d (\d+) .*)");
    std::smatch statusMatches;
    if (std::regex_match(statusLine, statusMatches, statusRegex)) {
        response.statusCode = std::stoi(statusMatches[1].str());
    }
    
    // Parse headers
    response.headers = parseHeaders(headerSection);
    
    LOGI("HTTP response received: %d (%zu bytes)", response.statusCode, response.body.length());
    
    return response;
}

std::string HttpClient::urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    
    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char)c);
            escaped << std::nouppercase;
        }
    }
    
    return escaped.str();
}

std::string HttpClient::urlDecode(const std::string& value) {
    std::string decoded;
    for (size_t i = 0; i < value.length(); ++i) {
        if (value[i] == '%' && i + 2 < value.length()) {
            int hex = std::stoi(value.substr(i + 1, 2), nullptr, 16);
            decoded += static_cast<char>(hex);
            i += 2;
        } else if (value[i] == '+') {
            decoded += ' ';
        } else {
            decoded += value[i];
        }
    }
    return decoded;
}

std::map<std::string, std::string> HttpClient::parseHeaders(const std::string& headerString) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(headerString);
    std::string line;
    
    // Skip status line
    std::getline(stream, line);
    
    while (std::getline(stream, line) && !line.empty() && line != "\r") {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            headers[key] = value;
        }
    }
    
    return headers;
}

std::string HttpClient::buildHeaderString(const std::map<std::string, std::string>& headers) {
    std::ostringstream stream;
    for (const auto& header : headers) {
        stream << header.first << ": " << header.second << "\r\n";
    }
    return stream.str();
}

} // namespace localify
