// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <array>
#include <string_view>

namespace SpartanLauncherText {
inline constexpr const char* AppName = "God of War III";
inline constexpr std::array<const char*, 3> Languages = {"English", "Português brasileiro", "Español"};
inline constexpr std::array<const char*, 3> Codes = {"en", "pt-BR", "es"};
inline int Parse(std::string_view code) {
    for (int i = 0; i < int(Codes.size()); ++i) if (code == Codes[i]) return i;
    return 0;
}
inline int Normalize(int language) { return language >= 0 && language < int(Codes.size()) ? language : 0; }
enum class Key {
    Start, StartOnDemand, Precompile, ChooseFolder, Language, Back, Browse, Current,
    Folder, Internal, Usb, FolderHelp, Scanning, Vertex, Fragment, Prepared, SomeFailed,
    Failed, NoFolder, LoadingCache, PreparingGraphics, Controls, LanguageHelp, PreparingTextures, StartupHelp, Count
};
inline constexpr std::array<std::array<const char*, 3>, size_t(Key::Count)> Text = {{
    {"Start game", "Iniciar jogo", "Iniciar juego"},
    {"Fast start (may miss graphics)", "Início rápido (pode omitir gráficos)", "Inicio rápido (puede omitir gráficos)"},
    {"Precompile shaders", "Pré-compilar shaders", "Precompilar shaders"},
    {"Choose game folder", "Escolher pasta do jogo", "Elegir carpeta del juego"},
    {"Language", "Idioma", "Idioma"},
    {"Back", "Voltar", "Volver"},
    {"Browse USB drive...", "Procurar na unidade USB...", "Buscar en la unidad USB..."},
    {"current", "atual", "actual"},
    {"Game folder: ", "Pasta do jogo: ", "Carpeta del juego: "},
    {"internal storage", "armazenamento interno", "almacenamiento interno"},
    {"USB drive", "unidade USB", "unidad USB"},
    {"Choose the game folder\nInternal storage: put the dump anywhere under D:\\DevelopmentFiles.\nUSB drive: any folder; choose it with Browse USB drive.",
     "Escolha a pasta do jogo\nArmazenamento interno: coloque o dump em D:\\DevelopmentFiles.\nUSB: selecione a pasta em Procurar na unidade USB.",
     "Elige la carpeta del juego\nAlmacenamiento interno: coloca el dump en D:\\DevelopmentFiles.\nUSB: selecciona la carpeta en Buscar en la unidad USB."},
    {"Scanning game files...", "Verificando arquivos do jogo...", "Analizando archivos del juego..."},
    {"Precompiling vertex shaders...", "Pré-compilando shaders de vértices...", "Precompilando shaders de vértices..."},
    {"Precompiling fragment shaders...", "Pré-compilando shaders de fragmentos...", "Precompilando shaders de fragmentos..."},
    {" shader entries prepared. Others compile during gameplay.", " shaders preparados. Os demais serão compilados durante o jogo.", " shaders preparados. Los demás se compilarán durante el juego."},
    {" Some variants could not be prepared.", " Algumas variantes não puderam ser preparadas.", " No se pudieron preparar algunas variantes."},
    {"Precompilation could not finish: ", "Não foi possível concluir a pré-compilação: ", "No se pudo completar la precompilación: "},
    {"Game folder is not configured", "A pasta do jogo não está configurada", "La carpeta del juego no está configurada"},
    {"Loading shader cache...", "Carregando cache de shaders...", "Cargando caché de shaders..."},
    {"Preparing graphics...", "Preparando gráficos...", "Preparando gráficos..."},
    {"Up/Down or D-pad: choose   Enter or A: OK   Esc or B: cancel",
     "Cima/baixo ou direcional: escolher   Enter ou A: confirmar   Esc ou B: cancelar",
     "Arriba/abajo o cruceta: elegir   Enter o A: confirmar   Esc o B: cancelar"},
    {"App interface language: menus, settings and messages.",
     "Idioma da interface do app: menus, configurações e mensagens.",
     "Idioma de la interfaz de la app: menús, ajustes y mensajes."},
    {"Preparing texture uploads...", "Preparando envio de texturas...", "Preparando carga de texturas..."},
    {"Start game prepares missing shaders and waits for required graphics. First preparation can take several minutes.",
     "Iniciar jogo prepara os shaders ausentes e aguarda os gráficos necessários. A primeira preparação pode levar vários minutos.",
     "Iniciar juego prepara los shaders ausentes y espera los gráficos necesarios. La primera preparación puede tardar varios minutos."},
}};
inline const char* Get(Key key, int language) { return Text[size_t(key)][Normalize(language)]; }
} // namespace SpartanLauncherText
