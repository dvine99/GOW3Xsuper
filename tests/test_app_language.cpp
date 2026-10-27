#include "spartanport_ui_language.h"
#include "spartanport_settings.h"
#include "spartanport_perf_stats.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

int main() {
    namespace U = SpartanUi;
    using SpartanSettings::SetInterfaceLanguage;
    const char* path = "app-language-test.ini";
#ifdef _WIN32
    _putenv_s("SPARTAN_CONFIG", path);
#else
    setenv("SPARTAN_CONFIG", path, 1);
#endif
    for (const auto& row : U::Catalog) {
        assert(row.en && *row.en && row.pt && *row.pt && row.es && *row.es);
        assert(std::string(U::Translate(row.en, 0)) == row.en);
        assert(std::string(U::Translate(row.en, 1)) == row.pt);
        assert(std::string(U::Translate(row.en, 2)) == row.es);
    }
    assert(U::Translate(nullptr, 1) == nullptr);
    assert(std::string(U::Translate("private/path%20/test", 1)) == "private/path%20/test");
    SetInterfaceLanguage(1);
    assert(std::string(U::T("Performance")) == "Desempenho");
    assert(SpartanSettings::Get().launcher_language == 1);
    SpartanSettings::Save();
    SetInterfaceLanguage(2);
    assert(std::string(U::T("Performance")) == "Rendimiento");
    // The pre-renderer bootstrap must load the same persisted preference.
    assert(U::LoadPreference(path) && U::language == 1);
    assert(std::string(U::T("Choose folder")) == "Escolher pasta");
    SpartanSettings::Load();
    assert(SpartanSettings::Get().launcher_language == 1 && U::language == 1);
    char text[128];
    std::snprintf(text, sizeof(text), U::T("Changes applied on next start: %d"), 13);
    assert(std::string(text) == "Alterações aplicadas na próxima inicialização: 13");
    std::snprintf(text, sizeof(text), U::T("Loading shader cache %u%%"), 75u);
    assert(std::string(text) == "Carregando cache de shaders 75%");
    // Substituted file paths remain literal, even if they contain format syntax.
    const auto message = U::Format("Writing the prepared game file {} failed.", {"dados%20/{}.bin"});
    assert(message == "Falha ao gravar o arquivo preparado dados%20/{}.bin.");
    SpartanPerfStats::Sample unknown;
    unknown.upscaler = "";
    assert(SpartanPerfStats::Format(unknown, SpartanPerfStats::Cpu | SpartanPerfStats::Upscaler, true, 1)
           == "Nativo   CPU n/d");
    { std::ofstream file(path); file << "launcher_language=es\r\nfps_limit=30\n"; }
    assert(U::LoadPreference(path) && U::language == 2);
    SpartanSettings::Load();
    assert(U::language == 2 && SpartanSettings::Get().launcher_language == 2);
    assert(std::string(U::T("Save game")) == "Guardar partida");
    assert(std::string(U::T("Close")) == "Cerrar");
    { std::ofstream file(path); file << "launcher_language=bogus\n"; }
    assert(!U::LoadPreference(path) && U::language == 0);
    { std::ofstream file(path); file << "fps_limit=30\n"; }
    SetInterfaceLanguage(2);
    SpartanSettings::Load();
    assert(U::language == 0 && SpartanSettings::Get().launcher_language == 0);
    std::remove(path);
    assert(!U::LoadPreference(path) && U::language == 0);
    SetInterfaceLanguage(-1);
    assert(U::language == 0 && SpartanSettings::Get().launcher_language == 0);
    SetInterfaceLanguage(99);
    assert(U::language == 0);
}
