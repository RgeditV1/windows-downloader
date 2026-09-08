#include "cli.hpp"


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
    constexpr std::string_view title = "WINDOWS DOWNLOADER";

    int padding = (width - 2 - static_cast<int>(title.size())) / 2;

    std::cout << "╔══════════════════════════════════════╗\n";

    std::cout << std::string(padding, ' ')
              << Color::GREEN
              << title
              << Color::RESET
              << std::string(width - 2 - padding - title.size(), ' ')
              << "\n";

    std::cout << "╠══════════════════════════════════════╣\n\n";

    std::cout << "   [1] Elegir ISO a descargar\n";
    std::cout << "   [2] Comprobar ISO disponibles\n";
    std::cout << Color::RED
              << "   [0] Salir\n\n"
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

            // Título principal en blanco negrita
            std::cout << Color::WHITE << Color::BOLD << "  • " << iso.title << Color::RESET;

            // Build entre paréntesis en azul/cyan
            if (iso.build > 0)
            {
                std::cout << Color::BLUE << " (" << iso.build << ")" << Color::RESET;
            }

            // Detalles extra (Arquitectura / Idioma) si existen
            if (!iso.architecture.empty() || !iso.language.empty())
            {
                std::cout << Color::YELLOW << " [" << iso.architecture 
                          << (iso.architecture.empty() || iso.language.empty() ? "" : " - ") 
                          << iso.language << "]" << Color::RESET;
            }

            // Tamaño resaltado en verde al final
            std::cout << " - " << Color::GREEN << Color::BOLD << formatFileSize(iso.size) << Color::RESET << "\n";
        }
    }

    std::cout << "\n";
    pause();
}

void chooseIso(const std::vector<IsoInfo>& isos)
{
    clearScreen();

    std::cout
        << Color::CYAN
        << Color::BOLD
        << "\n=== Elegir ISO a descargar ===\n\n"
        << Color::RESET;

    if (isos.empty())
    {
        std::cout << Color::RED << "No hay ISOs cargadas.\n" << Color::RESET;
    }
    else
    {
        for (size_t i = 0; i < isos.size(); ++i)
        {
            std::cout << Color::YELLOW << " [" << (i + 1) << "] " << Color::RESET
                      << Color::WHITE << Color::BOLD << isos[i].title << Color::RESET
                      << Color::BLUE << " (" << isos[i].build << ") " << Color::RESET
                      << Color::GREEN << formatFileSize(isos[i].size) << Color::RESET
                      << "\n";
        }
    }

    std::cout << "\n";
    pause();
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}