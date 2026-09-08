#include <iostream>
#include <fstream>

#include "downloader.hpp"
#include "cli.hpp"

CURL* curl = nullptr;
CURLcode result;
std::string response;

const std::string USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64)";

namespace fs = std::filesystem;

fs::path getDownloadsFolderPath()
{
#ifdef _WIN32
    PWSTR pathPtr = nullptr;
    // FOLDERID_Downloads obtiene automáticamente la ubicación exacta
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &pathPtr)))
    {
        fs::path downloadsPath(pathPtr);
        CoTaskMemFree(pathPtr);
        return downloadsPath;
    }
#endif

    // solo para compatiblidad con otras OS
    const char* homeDir = std::getenv("HOME");
    if (!homeDir)
    {
        homeDir = std::getenv("USERPROFILE");
    }

    if (homeDir)
    {
        fs::path downloadsPath = fs::path(homeDir) / "Downloads";
        if (fs::exists(downloadsPath))
            return downloadsPath;

        // Intentar versión en español si existe :/
        fs::path descargasPath = fs::path(homeDir) / "Descargas";
        if (fs::exists(descargasPath))
            return descargasPath;

        return fs::path(homeDir); // Fallback al directorio Home si no existe carpeta
    }

    return fs::current_path(); // Fallback al directorio actual
}

size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
    std::ofstream* file = static_cast<std::ofstream*>(stream);
    size_t totalBytes = size * nmemb;
    file->write(static_cast<char*>(ptr), totalBytes);
    return totalBytes;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;

    output->append(
        static_cast<char*>(contents),
        totalSize
    );

    return totalSize;
}

bool downloadIsoFile(const IsoInfo& iso, const fs::path& destinationFolder)
{
    // Crear la carpeta si no existe
    if (!fs::exists(destinationFolder))
    {
        fs::create_directories(destinationFolder);
    }

    std::string filename = iso.title + ".iso";
    fs::path destinationPath = destinationFolder / filename;

    std::ofstream outFile(destinationPath, std::ios::binary);
    if (!outFile.is_open())
    {
        std::cerr << "No se pudo abrir el archivo para escritura: " << destinationPath << "\n";
        return false;
    }

    CURL* curl_handle = curl_easy_init();
    if (!curl_handle)
    {
        return false;
    }

    std::cout << Color::CYAN << "Guardando en: " << Color::WHITE << destinationPath.string() << Color::RESET << "\n\n";

    curl_easy_setopt(curl_handle, CURLOPT_URL, iso.url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteFileCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &outFile);

    // Activar barra de progreso estándar de libcurl
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);

    CURLcode res = curl_easy_perform(curl_handle);

    curl_easy_cleanup(curl_handle);
    outFile.close();

    return (res == CURLE_OK);
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
        USER_AGENT
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

    clearScreen();

    std::atomic<bool> loading(true);
    std::vector<IsoInfo> isos;

    // Iniciar la animación del spinner en un hilo secundario
    std::thread spinnerThread(showSpinner, std::ref(loading), "Cargando información de ISOs...");

    InitCurl();

    if (result == CURLE_OK)
    {
        try
        {
            isos = parseIsoInfo(response);
        }
        catch (const json::exception& e)
        {
            loading = false;
            if (spinnerThread.joinable()) spinnerThread.join();
            std::cerr << "JSON error: " << e.what() << '\n';
            pause();
            return 1;
        }
    }

    loading = false;
    if (spinnerThread.joinable())
    {
        spinnerThread.join();
    }

    if (result != CURLE_OK)
    {
        std::cout << Color::RED << "Error de Conexion.\n" << Color::RESET;
        pause();
        return 1;
    }

    // Menú principal
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

    curl_global_cleanup();
    return 0;
}