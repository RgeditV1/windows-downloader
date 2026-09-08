#include <iostream>

#include "downloader.hpp"
#include "cli.hpp"

CURL* curl = nullptr;
CURLcode result;
std::string response;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;

    output->append(
        static_cast<char*>(contents),
        totalSize
    );

    return totalSize;
}

void InitCurl(){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, RAW.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        result = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl = nullptr;
    }
}

std::vector<IsoInfo> parseIsoInfo(const std::string& response)
{
    std::vector<IsoInfo> isos;

    json data = json::parse(response);

    for (const auto& item : data)
    {
        IsoInfo iso;

        iso.title = item.value("title", "");
        iso.url = item.value("link", "");
        iso.language = item.value("language", "");
        iso.architecture = item.value("architecture", "");
        iso.build = item.value("build", 0);

        iso.size = getRemoteFileSize(iso.url);

        isos.push_back(iso);
    }

    return isos;
}

uint64_t getRemoteFileSize(const std::string& url)
{
    CURL* curl_handle = curl_easy_init();

    if (!curl_handle)
        return 0;

    curl_off_t content_length = -1;

    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

    curl_easy_setopt(
        curl_handle,
        CURLOPT_USERAGENT,
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64)"
    );

    curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 10L);

    CURLcode res = curl_easy_perform(curl_handle);

    if (res == CURLE_OK)
    {
        curl_easy_getinfo(
            curl_handle,
            CURLINFO_CONTENT_LENGTH_DOWNLOAD_T,
            &content_length
        );
    }

    curl_easy_cleanup(curl_handle);

    if (content_length > 0)
        return static_cast<uint64_t>(content_length);

    return 0;
}

std::string formatFileSize(uint64_t bytes)
{
    const char* units[] = {
        "B", "KB", "MB", "GB", "TB"
    };

    double size = static_cast<double>(bytes);
    int unit = 0;

    while (size >= 1024.0 && unit < 4)
    {
        size /= 1024.0;
        unit++;
    }

    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << size;

    return stream.str() + " " + units[unit];
}

int main() {

    InitCurl();

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // Habilitar ANSI en Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;

        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    #endif

    if (result == CURLE_OK)
    {
        try
        {
            std::vector<IsoInfo> isos = parseIsoInfo(response);
            while (true)
            {
                clearScreen();
                drawMenu();

                std::cout
                    << "\n"
                    << Color::CYAN
                    << "  > "
                    << Color::RESET;

                std::string option;
                std::getline(std::cin, option);

                if (option == "1")
                {
                    chooseIso(isos);
                }
                else if (option == "2")
                {
                    checkIsos(isos);
                }
                else if (option == "0")
                {
                    clearScreen();

                    std::cout
                        << Color::GREEN
                        << "\n  Saliendo...\n\n"
                        << Color::RESET;

                    break;
                }
                else
                {
                    std::cout
                        << Color::RED
                        << "\n  Opción no válida.\n"
                        << Color::RESET;

                    pause();
                }
            }
        }
        catch (const json::exception& e)
        {
            std::cerr << "JSON error: " << e.what() << '\n';
        }
    }

    curl_global_cleanup();
    return 0;
}