#pragma once

#include <string>

#include <curl/curl.h>

struct GeniusController {
    explicit GeniusController(std::string const& ip);
    ~GeniusController();

    bool isInTestMode() const { return m_testMode; };
    void getControllerState();

    std::string GetConfigString() const {
        return "IP: " + m_ipAddress;
    }

    bool GetWarnings();

    std::vector<bool> GetEFuseStatus() const;

    static size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {

        if (data == nullptr) return 0;
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

private:
    std::string m_ipAddress;
    std::string m_description;
    CURL *m_curl;

    bool m_testMode{false};
    bool m_hasEFuses{false};

    bool postData(std::string const& url, std::string const& data, std::string const& contentType ) const;
    bool postData(std::string const& url, std::string const& data, std::string& response_string, std::string const& contentType ) const;
    bool getData(std::string const& url, std::string& response_string, std::string const& contentType ) const;
};