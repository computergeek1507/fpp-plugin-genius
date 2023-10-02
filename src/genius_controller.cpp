#include <fpp-pch.h>

#include "genius_controller.h"
#include "common.h"
#include "settings.h"

#include "Warnings.h"
#include "log.h"

GeniusController::GeniusController(std::string const& ip):
    m_ipAddress(ip) {
    m_curl = curl_easy_init();
}

GeniusController::~GeniusController() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
}

void GeniusController::getControllerState()
 {
    // /api/state
    //"test_mode_enabled": true,

    std::string data;
    const std::string url = "/api/state";
    bool work = getData(url, data, "application/json" );
    Json::Value v = LoadJsonFromString(data);

    if (v["system"]["test_mode_enabled"].isBool()) {
        m_testMode = v["system"]["test_mode_enabled"].asBool();
    }
    //friendly_name
    if (v["system"]["friendly_name"].isString()) {
        m_description = v["system"]["friendly_name"].asString();
    }

    if (v["system"]["has_efuses"] != Json::nullValue && v["system"]["has_efuses"].isBool()) {
        m_hasEFuses = v["system"]["has_efuses"].asBool();
    }
 }

bool GeniusController::GetWarnings() 
{
    bool hasWarning{false};
    getControllerState();
    if(m_hasEFuses) {
        auto eFuseStatus = GetEFuseStatus();
        int port = 1;
        for (auto eFuse : eFuseStatus) {
            if (!eFuse) {
                WarningHolder::AddWarning(m_description + " " + m_ipAddress + " Port " + std::to_string(port) + " has a blown EFuse");
                hasWarning = true;
            }
            ++port;
        }
    }
    return hasWarning;
}

std::vector<bool> GeniusController::GetEFuseStatus() const 
{
    std::vector<bool> status;
    // /api/last_current_readings
    std::string data;
    const std::string url = "/api/last_current_readings";
    bool work = getData(url, data, "application/json" );
    //LogInfo(VB_PLUGIN, "'%s'\n", data.c_str());
    Json::Value v = LoadJsonFromString(data);

    //"power_good_states"
    for (auto eFuse : v["power_good_states"]) {
        if (eFuse.isBool()) {
            status.push_back(eFuse.asBool());
        }
    }
    return status;
}

 bool GeniusController::postData(std::string const& url, std::string const& data, std::string const& contentType ) const {
    std::string response_string;
    return postData(url, data, response_string, contentType);
}

bool GeniusController::postData(std::string const& url, std::string const& data, std::string& response_string, std::string const& contentType) const {
    try {
        curl_easy_setopt(m_curl, CURLOPT_HTTPPOST, 1L);

        std::string const content = "Content-Type: " + contentType;
        struct curl_slist *hs = NULL;
        hs = curl_slist_append(hs, content.c_str());
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, hs);

        LogInfo(VB_PLUGIN, "Data '%s'\n",data.c_str());
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());

        std::string const repURL = "http://" + m_ipAddress + url;
        LogInfo(VB_PLUGIN, "URL '%s'\n",repURL.c_str());
        curl_easy_setopt(m_curl, CURLOPT_URL, repURL.c_str());
        curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(m_curl, CURLOPT_MAXAGE_CONN, 120L);

        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode status = curl_easy_perform(m_curl);
        if (status != CURLE_OK) {            
            LogInfo(VB_PLUGIN, "failed to send curl command\n");
            return false;
        }
        return true;
    }
    catch(std::exception const& ex) {
        LogInfo(VB_PLUGIN, "Error %s \n", ex.what());
    }
    return false;
}

bool GeniusController::getData(std::string const& url, std::string& response_string, std::string const& contentType) const {
    try {
        curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1L);

        std::string const content = "Content-Type: " + contentType;
        struct curl_slist *hs = NULL;
        hs = curl_slist_append(hs, content.c_str());
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, hs);

        std::string const repURL = "http://" + m_ipAddress + url;
        LogInfo(VB_PLUGIN, "URL '%s'\n",repURL.c_str());
        curl_easy_setopt(m_curl, CURLOPT_URL, repURL.c_str());
        curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(m_curl, CURLOPT_MAXAGE_CONN, 120L);

        //std::string response_string;

        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode status = curl_easy_perform(m_curl);
        if (status != CURLE_OK) {
            LogInfo(VB_PLUGIN, "failed to send curl command\n");
            return false;
        }
        return true;
    }
    catch(std::exception const& ex) {
        LogInfo(VB_PLUGIN, "Error %s \n", ex.what());
    }
    return false;
}