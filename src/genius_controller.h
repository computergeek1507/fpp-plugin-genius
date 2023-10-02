#pragma once

#include <string>

#include "log.h"

#include <curl/curl.h>

struct GeniusController {
    explicit GeniusController(std::string const& ip);
    ~GeniusController();

    bool isInTestMode() const;

        virtual std::string GetConfigString() const {
        return "IP: " + m_ipAddress;
    }

    static size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {

        if (data == nullptr) return 0;
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

private:
    std::string m_ipAddress;
    CURL *m_curl;

    bool postData(std::string const& url, std::string const& data, std::string const& contentType ) const;
    bool postData(std::string const& url, std::string const& data, std::string& response_string, std::string const& contentType ) const;
    bool getData(std::string const& url, std::string& response_string, std::string const& contentType ) const;
};