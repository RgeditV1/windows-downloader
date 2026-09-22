#pragma once

#include <iostream>
#include <string>
#include <regex>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include "downloader.hpp"

enum class DistroType {
    ArchLinux,
    Debian,
    Unknown
};

inline DistroType GetDistroType(const std::string& title) {
    if (title.find("Arch") != std::string::npos)   return DistroType::ArchLinux;
    if (title.find("Debian") != std::string::npos) return DistroType::Debian;
    return DistroType::Unknown;
}

inline std::string GetHTML(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string htmlBuffer;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT.c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error en cURL al acceder a " << url << ": " 
                      << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return htmlBuffer;
}

inline std::pair<std::string, std::string> ScrapeIsoUrl(const std::string& title, const std::string& mirrorUrl) {
    std::string html = GetHTML(mirrorUrl);
    if (html.empty()) return {"", ""};

    DistroType distro = GetDistroType(title);

    switch (distro) {
        case DistroType::ArchLinux: {
            std::regex archRegex(R"(href=["'](archlinux-[^"']+\.iso)["'][^>]*>.*?(\d{4}-\d{2}-\d{2}))");
            std::smatch match;
            if (std::regex_search(html, match, archRegex)) {
                std::string isoUrl = mirrorUrl + match[1].str();
                std::string isoDate = match[2].str();
                return {isoUrl, isoDate};
            }
            break;
        }

        case DistroType::Debian: {
            std::regex debianRegex(R"(href=["'](debian-[^"']+\.iso)["'][^>]*>.*?(\d{4}-\d{2}-\d{2}))");
            std::smatch match;
            if (std::regex_search(html, match, debianRegex)) {
                std::string isoUrl = mirrorUrl + match[1].str();
                std::string isoDate = match[2].str();
                return {isoUrl, isoDate};
            }
            break;
        }

        default: {
            // capturar cualquier .iso y su fecha
            std::regex genericRegex(R"(href=["']([^"']+\.iso)["'][^>]*>.*?(\d{4}-\d{2}-\d{2}))");
            std::smatch match;
            if (std::regex_search(html, match, genericRegex)) {
                std::string filename = match[1].str();
                if (filename.find(".sig") == std::string::npos && 
                    filename.find(".torrent") == std::string::npos) {
                    return {mirrorUrl + filename, match[2].str()};
                }
            }
            break;
        }
    }

    return {"", ""};
}