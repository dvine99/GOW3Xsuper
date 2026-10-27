#include "spartanport_launcher_text.h"
#include "spartanport_settings.h"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    namespace L = SpartanLauncherText;
    assert(L::Parse("pt-BR") == 1 && L::Parse("es") == 2 && L::Parse("en") == 0);
    assert(L::Parse("invalid") == 0 && L::Parse("") == 0);
    assert(L::Normalize(-1) == 0 && L::Normalize(3) == 0);
    for (const auto& row : L::Text)
        for (const auto* text : row) assert(text && *text);
    assert(std::string(L::Get(L::Key::Start, 1)) == "Iniciar jogo");
    assert(std::string(L::Get(L::Key::Start, 2)) == "Iniciar juego");
    const char* path = "launcher-language-test.ini";
#ifdef _WIN32
    _putenv_s("SPARTAN_CONFIG", path);
#else
    setenv("SPARTAN_CONFIG", path, 1);
#endif
    auto& settings = SpartanSettings::Get();
    // Legacy files retain English. Selecting a language must preserve performance settings.
    { std::ofstream out(path); out << "fps_limit=30\nxbox_copy_threads=1\n"; }
    SpartanSettings::Load();
    assert(settings.launcher_language == 0);
    for (int language = 0; language < 3; ++language) {
        settings.launcher_language = language;
        SpartanSettings::Save();
        std::ifstream in(path);
        std::stringstream saved;
        saved << in.rdbuf();
        assert(saved.str().find(std::string("launcher_language=") + L::Codes[language] + "\n") != std::string::npos);
        settings.launcher_language = (language + 1) % 3;
        SpartanSettings::Load();
        assert(settings.launcher_language == language);
        assert(settings.fps_limit == 30 && settings.xbox_performance.selected[2] == 1);
        assert(!SpartanSettings::GraphicsNeedRestart());
    }
    { std::ofstream out(path); out << "launcher_language=invalid\n"; }
    SpartanSettings::Load();
    assert(settings.launcher_language == 0);
    std::remove(path);
}
