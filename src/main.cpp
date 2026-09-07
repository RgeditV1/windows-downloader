#include <iostream>
#include "downloader.hpp"

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
    }
    curl_global_cleanup();
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

        // Por ahora
        iso.size = 0;

        isos.push_back(iso);
    }

    return isos;
}

int main() {

    InitCurl();

    if (result == CURLE_OK)
    {
        try
        {
            std::vector<IsoInfo> isos = parseIsoInfo(response);

            for (const auto& iso : isos)
            {
                std::cout << "Title: " << iso.title << '\n';
                std::cout << "URL: " << iso.url << '\n';
                std::cout << "Language: " << iso.language << '\n';
                std::cout << "Architecture: " << iso.architecture << '\n';
                std::cout << "Build: " << iso.build << '\n';
                std::cout << "Size: " << iso.size << '\n';

                std::cout << "------------------\n";
            }
        }
        catch (const json::exception& e)
        {
            std::cerr << "JSON error: " << e.what() << '\n';
        }
    }

    return 0;
}