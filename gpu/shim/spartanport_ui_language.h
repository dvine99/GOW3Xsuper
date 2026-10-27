// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "spartanport_launcher_text.h"
#include "spartanport_ui_catalog.h"
#include <algorithm>
#include <atomic>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <initializer_list>

namespace SpartanUi {
// Shared by the native UWP host and its statically linked renderer. Set before
// bootstrap dialogs; subsequent menu changes are visible to every UI thread.
inline std::atomic<int> language{0};
inline void Select(int value) { language = SpartanLauncherText::Normalize(value); }
inline const char* Translate(const char* text, int selected) {
    if (!text || !selected) return text;
    const auto row = std::lower_bound(Catalog.begin(), Catalog.end(), std::string_view(text),
        [](const Entry& entry, std::string_view key) { return std::string_view(entry.en) < key; });
    if (row == Catalog.end() || row->en != std::string_view(text)) return text;
    return selected == 1 ? row->pt : selected == 2 ? row->es : text;
}
inline const char* T(const char* text) { return Translate(text, language.load()); }
// Values such as paths and save titles are substituted AFTER translation. They
// are never interpreted as printf syntax or translated as fragments of a label.
inline std::string Format(const char* key, std::initializer_list<std::string_view> values) {
    const std::string_view pattern = T(key);
    std::string out;
    size_t at = 0;
    for (auto value : values) {
        const auto next = pattern.find("{}", at);
        if (next == std::string_view::npos) break;
        out.append(pattern.substr(at, next - at));
        out.append(value);
        at = next + 2;
    }
    out.append(pattern.substr(at));
    return out;
}
// Returns false when no valid saved preference exists. Only this one key is
// read here: loading renderer settings before device initialization has effects.
inline bool LoadPreference(const std::filesystem::path& path) {
    Select(0);
    std::ifstream file(path);
    bool found = false;
    for (std::string line; std::getline(file, line);) {
        const auto end = line.find_last_not_of(" \t\r");
        if (end != std::string::npos) line.erase(end + 1);
        constexpr std::string_view prefix = "launcher_language=";
        if (line.starts_with(prefix)) {
            const auto code = std::string_view(line).substr(prefix.size());
            found = std::find(SpartanLauncherText::Codes.begin(), SpartanLauncherText::Codes.end(), code)
                    != SpartanLauncherText::Codes.end();
            Select(SpartanLauncherText::Parse(code));
        }
    }
    return found;
}
} // namespace SpartanUi
