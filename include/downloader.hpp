#pragma once

#include <cstdlib>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern CURL* curl;
extern CURLcode result;
extern std::string response; // Datos recibidos por Curl

const std::string RAW = "https://raw.githubusercontent.com/RgeditV1/windows-downloader/main/iso.json";

struct IsoInfo;

std::vector<IsoInfo> parseIsoInfo(const std::string& response);

// Callback que recibe los datos descargados
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

void InitCurl();

uint64_t getRemoteFileSize(const std::string& url);

std::string formatFileSize(uint64_t bytes);

enum class Architecture {
    x86,
    x64,
    ARM64,
    Multi,
    Unknown
};

struct IsoInfo {
    std::string title;
    std::string url;
    std::string language;
    std::string architecture;
    uint32_t build;
    uint64_t size;
};
