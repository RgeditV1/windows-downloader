#pragma once

#include <cstdlib>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <filesystem>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <shlobj.h> // Para SHGetKnownFolderPath
#endif

using json = nlohmann::json;

extern CURL* curl;
extern CURLcode result;
extern std::string response; // Datos recibidos por Curl

const std::string RAW = "https://raw.githubusercontent.com/RgeditV1/windows-downloader/main/iso.json";

struct IsoInfo;

std::vector<IsoInfo> parseIsoInfo(const std::string& response);

// Callback que recibe los datos Json
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

// Callback para escribir en disco
size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, void* stream);

void InitCurl();

uint64_t getRemoteFileSize(const std::string& url);

std::string formatFileSize(uint64_t bytes);

std::filesystem::path getDownloadsFolderPath();

bool downloadIsoFile(const IsoInfo& iso, const std::filesystem::path& destinationFolder);

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
