#include "cli.hpp"

namespace CLI {
    void pause()
    {
        std::cout << "\n"
                << Color::YELLOW
                << "Presiona ENTER para continuar..."
                << Color::RESET;

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cin.get();
    }

    void drawMenu()
    {
        constexpr int width = 38;
        constexpr std::string_view title = MenuConfig::TITLE;

        int padding = (width - 2 - static_cast<int>(title.size())) / 2;

        std::cout << "╔══════════════════════════════════════╗\n";

        std::cout << std::string(padding, ' ')
                << Color::GREEN
                << title
                << Color::RESET
                << std::string(width - 2 - padding - title.size(), ' ')
                << "\n";

        std::cout << "╠══════════════════════════════════════╣\n\n";

        std::cout << "   [1] " << MenuConfig::OPTION_DOWNLOAD_WINDOWS << "\n";
        std::cout << "   [2] " << MenuConfig::OPTION_DOWNLOAD_LINUX << "\n";
        std::cout << "   [3] " << MenuConfig::OPTION_CHECK << "\n";
        std::cout << Color::RED
                << "   [0] " << MenuConfig::OPTION_EXIT << "\n\n"
                << Color::RESET;

        std::cout << "╚══════════════════════════════════════╝\n";
    }

    void checkIsos(const std::vector<IsoInfo>& isos)
    {
        clearScreen();

        std::cout
            << Color::CYAN
            << Color::BOLD
            << "\n=== ISOs Disponibles ===\n\n"
            << Color::RESET;

        if (isos.empty())
        {
            std::cout << Color::RED << "  No se encontraron ISOs disponibles o fallo en la carga.\n" << Color::RESET;
        }
        else
        {
            for (size_t i = 0; i < isos.size(); ++i)
            {
                const auto& iso = isos[i];

                // Título principal
                std::cout << Color::WHITE << Color::BOLD << "  • " << iso.title << Color::RESET;

                if (iso.osType == OSCategory::Windows && iso.build > 0)
                {
                    std::cout << Color::BLUE << " (Build " << iso.build << ")" << Color::RESET;
                }
                else if (iso.osType == OSCategory::Linux && !iso.date.empty())
                {
                    std::cout << Color::CYAN << " (" << iso.date << ")" << Color::RESET;
                }

                // Detalles de Arquitectura e Idioma
                if (!iso.architecture.empty() || !iso.language.empty())
                {
                    std::cout << Color::YELLOW << " [" << iso.architecture 
                            << (iso.architecture.empty() || iso.language.empty() ? "" : " - ") 
                            << iso.language << "]" << Color::RESET;
                }

                std::cout << " - " << Color::GREEN << Color::BOLD << formatFileSize(iso.size) << Color::RESET << "\n";
            }
        }

        std::cout << "\n"
                << Color::YELLOW
                << "Presiona cualquier tecla para volver al menú..."
                << Color::RESET << std::flush;

        #ifdef _WIN32
            _getch();
        #else
            system("stty raw -echo");
            std::cin.get();
            system("stty cooked echo");
        #endif
    }

    void chooseIso(const std::vector<IsoInfo>& isos, OSCategory targetCategory)
    {
        clearScreen();

        std::string categoryTitle = (targetCategory == OSCategory::Windows) ? "Windows" : "Linux";

        std::cout
            << Color::CYAN
            << Color::BOLD
            << "\n=== Elegir ISO de " << categoryTitle << " a descargar ===\n\n"
            << Color::RESET;

        // Filtrar según la categoría solicitada
        std::vector<IsoInfo> filteredIsos;
        for (const auto& iso : isos)
        {
            if (iso.osType == targetCategory)
            {
                filteredIsos.push_back(iso);
            }
        }

        if (filteredIsos.empty())
        {
            std::cout << Color::RED << "No hay ISOs de " << categoryTitle << " disponibles para descargar.\n" << Color::RESET;
            pause();
            return;
        }

        for (size_t i = 0; i < filteredIsos.size(); ++i)
        {
            const auto& iso = filteredIsos[i];

            std::cout << Color::YELLOW << "  [" << (i + 1) << "] " << Color::RESET
                    << Color::WHITE << Color::BOLD << iso.title << Color::RESET;

            if (iso.osType == OSCategory::Windows && iso.build > 0)
            {
                std::cout << Color::BLUE << " (Build " << iso.build << ")" << Color::RESET;
            }
            else if (iso.osType == OSCategory::Linux && !iso.date.empty())
            {
                std::cout << Color::CYAN << " (" << iso.date << ")" << Color::RESET;
            }

            std::cout << Color::CYAN << " - [" << iso.architecture << "]" << Color::RESET
                    << " - " << Color::GREEN << formatFileSize(iso.size) << Color::RESET
                    << "\n";
        }

        std::cout << Color::RED << "\n  [0] Cancelar y volver al menú\n\n" << Color::RESET;

        std::cout << Color::CYAN << "  Selecciona una opción > " << Color::RESET;
        std::string input;
        std::getline(std::cin, input);

        try
        {
            int index = std::stoi(input);

            if (index > 0 && static_cast<size_t>(index) <= filteredIsos.size())
            {
                const auto& selectedIso = filteredIsos[index - 1];
                
                std::cout << "\n" << Color::GREEN << "Has seleccionado: " << Color::WHITE << Color::BOLD << selectedIso.title << Color::RESET << "\n";
                
                std::filesystem::path downloadsPath = getDownloadsFolderPath();

                std::cout << Color::YELLOW << "Iniciando descarga...\n" << Color::RESET;
                
                if (downloadIsoFile(selectedIso, downloadsPath))
                {
                    std::cout << "\n" << Color::GREEN << Color::BOLD << "¡Descarga completada con éxito!" << Color::RESET << "\n";
                }
                else
                {
                    std::cout << "\n" << Color::RED << Color::BOLD << "Error durante la descarga de la ISO." << Color::RESET << "\n";
                }
            }
            else if (index == 0)
            {
                return;
            }
            else
            {
                std::cout << Color::RED << "\nOpción no válida.\n" << Color::RESET;
            }
        }
        catch (...)
        {
            std::cout << Color::RED << "\nEntrada no válida.\n" << Color::RESET;
        }

        pause();
    }

    void showSpinner(const std::atomic<bool>& loading, const std::string& message)
    {
        const char frames[] = {'|', '/', '-', '\\'};
        size_t index = 0;

        std::cout << "\033[?25l";

        while (loading.load())
        {
            std::cout << "\r" << Color::CYAN << Color::BOLD << "[" << frames[index] << "] " 
                    << Color::RESET << message << std::flush;
            index = (index + 1) % 4;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "\r\033[K" << "\033[?25h" << std::flush;
    }

    void clearScreen()
    {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    }
} // CLI