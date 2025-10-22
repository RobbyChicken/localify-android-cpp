#ifndef LOCALIFY_HTTP_CLIENT_H
#define LOCALIFY_HTTP_CLIENT_H

#include <string>
#include <map>
#include <future>

namespace localify {

// Simple HTTP client implementation for Android (replacing libcurl)
struct HttpResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string error;
    
    HttpResponse() : statusCode(0) {}
    
    bool isSuccess() const {
        return statusCode >= 200 && statusCode < 300;
    }
};

struct HttpRequest {
    std::string url;
    std::string method;
    std::string body;
    std::map<std::string, std::string> headers;
    int timeoutSeconds;
    
    HttpRequest(const std::string& url = "", const std::string& method = "GET")
        : url(url), method(method), timeoutSeconds(30) {}
    
    void setHeader(const std::string& key, const std::string& value) {
        headers[key] = value;
    }
    
    void setContentType(const std::string& contentType) {
        headers["Content-Type"] = contentType;
    }
    
    void setAuthorization(const std::string& token) {
        headers["Authorization"] = "Bearer " + token;
    }
    
    void setUserAgent(const std::string& userAgent) {
        headers["User-Agent"] = userAgent;
    }
};

class HttpClient {
private:
    static std::unique_ptr<HttpClient> instance;
    std::string userAgent;
    
    HttpClient();
    
public:
    ~HttpClient();
    
    static HttpClient& getInstance();
    
    // Synchronous methods
    HttpResponse get(const std::string& url, const std::map<std::string, std::string>& headers = {});
    HttpResponse post(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers = {});
    HttpResponse put(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers = {});
    HttpResponse patch(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers = {});
    HttpResponse delete_(const std::string& url, const std::map<std::string, std::string>& headers = {});
    
    // Generic request method
    HttpResponse request(const HttpRequest& request);
    
    // Asynchronous methods
    std::future<HttpResponse> getAsync(const std::string& url, const std::map<std::string, std::string>& headers = {});
    std::future<HttpResponse> postAsync(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers = {});
    std::future<HttpResponse> requestAsync(const HttpRequest& request);
    
    // Configuration
    void setUserAgent(const std::string& userAgent);
    void setDefaultTimeout(int seconds);
    
private:
    // Platform-specific HTTP implementation using Android's native networking
    HttpResponse performRequest(const HttpRequest& request);
    
    // URL encoding utilities
    std::string urlEncode(const std::string& value);
    std::string urlDecode(const std::string& value);
    
    // Header parsing utilities
    std::map<std::string, std::string> parseHeaders(const std::string& headerString);
    std::string buildHeaderString(const std::map<std::string, std::string>& headers);
    
    int defaultTimeoutSeconds;
};

} // namespace localify

#endif // LOCALIFY_HTTP_CLIENT_H
