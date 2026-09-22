#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#endif

#include "downloader.hpp"

namespace Color {
    constexpr const char* RESET  = "\033[0m";
    constexpr const char* RED    = "\033[31m";
    constexpr const char* GREEN  = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BLUE   = "\033[34m";
    constexpr const char* CYAN   = "\033[36m";
    constexpr const char* WHITE  = "\033[37m";
    constexpr const char* BOLD   = "\033[1m";
}

namespace MenuConfig {
    constexpr const char* TITLE = "WINDOWS-DOWNLOADER";

    constexpr int WIDTH = 38;

    constexpr const char* OPTION_DOWNLOAD_WINDOWS = "Elegir ISO Windows";
    constexpr const char* OPTION_DOWNLOAD_LINUX   = "Elegir ISO Linux";
    constexpr const char* OPTION_CHECK            = "Comprobar ISOs disponibles";
    constexpr const char* OPTION_EXIT             = "Salir";
}

namespace CLI {
    void clearScreen();
    void pause();
    void drawMenu();
    void showSpinner(const std::atomic<bool>& loading, const std::string& message);
    
    // Filtrar y seleccionar por tipo de SO
    void chooseIso(const std::vector<IsoInfo>& isos, OSCategory targetCategory);
    
    // Comprobar todas las ISOs disponibles
    void checkIsos(const std::vector<IsoInfo>& isos);
}