#include "spartanport_xbox_performance.h"
#include "spartanport_settings.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cassert>
#include <map>
#include <string>

namespace P = SpartanXboxPerformance;
int main() {
    std::map<std::string, std::string> config, env;
    auto get = [&](const char* key) -> const char* {
        auto found = env.find(key);
        return found == env.end() ? nullptr : found->second.c_str();
    };
    auto apply = [&] {
        P::Apply([&](const char* key) { return config[key]; }, get,
            [&](const char* key, const char* value) {
                if (*value) env[key] = value;
                else env.erase(key); // _putenv_s(key, "") on Windows
            });
    };
    apply();
    assert(env.at("SPARTAN_COPY_THREADS") == "2");
    assert(env.at("SPARTAN_STREAM_MEMORY") == "cached");
    assert(env.at("SPARTAN_PIPELINE_CACHE") == "1");
    assert(!get("SPARTAN_FRAME_STATS") && !get("SPARTAN_UWP_METRICS"));
    assert(!get("SPARTAN_IO_STATS") && !get("SPARTAN_PERF_DIAG"));
    // Every menu selection reaches its environment consumer; invalid input falls back.
    for (const auto& option : P::Options) {
        for (int choice = 0; choice < option.count; ++choice) {
            env.clear();
            config.clear();
            config[option.key] = std::to_string(choice);
            apply();
            const char* value = get(option.environment);
            assert(std::string(value ? value : "") == option.values[choice]);
        }
        for (const char* invalid : {"", "-1", "3", "99", "1oops", "true"})
            assert(P::Parse(option, invalid) == option.initial);
    }
    env.clear();
    config.clear();
    config["xbox_io_stats"] = "1";
    config["xbox_wait_diagnostics"] = "1";
    apply();
    assert(env.at("SPARTAN_UWP_METRICS") == "1");
    assert(env.at("SPARTAN_FRAME_STATS") == "1");
    env.clear();
    env["SPARTAN_UWP_METRICS"] = "0";
    env["SPARTAN_FRAME_STATS"] = "0";
    apply();
    assert(env.at("SPARTAN_UWP_METRICS") == "0" && env.at("SPARTAN_FRAME_STATS") == "0");
    env.clear();
    env["SPARTAN_VK_RECORD_THREAD"] = "0";
    env["SPARTAN_COPY_THREADS"] = "4"; // Private advanced overrides remain unrestricted.
    config["xbox_record_thread"] = "1";
    apply();
    assert(env.at("SPARTAN_VK_RECORD_THREAD") == "0");
    assert(env.at("SPARTAN_COPY_THREADS") == "4");
    P::Settings settings;
    assert(settings.Changes() == 0);
    assert(settings.Set("xbox_record_thread", "1"));
    assert(!settings.Set("unrelated", "1"));
    settings.CaptureStartup(get);
    assert(settings.selected[0] == 0 && settings.Changes() == 0);
    settings.Set("xbox_record_thread", "1");
    assert(settings.Changes() == 1);
    settings.Set("xbox_record_thread", "0");
    assert(settings.Changes() == 0);

    // Exercise the actual settings serializer, not a duplicate of its format.
    const char* path = "xbox-performance-test.ini";
#ifdef _WIN32
    _putenv_s("SPARTAN_CONFIG", path);
#else
    setenv("SPARTAN_CONFIG", path, 1);
#endif
    {
        std::ofstream out(path);
        out << "xbox_record_thread=0\nxbox_copy_threads=1\nxbox_metrics=1\n"
               "xbox_stream_memory=1\nfps_limit=30\nxbox_pipeline_cache=invalid\n";
    }
    SpartanSettings::Load();
    auto& actual = SpartanSettings::Get();
    assert(actual.xbox_performance.selected[0] == 0);
    assert(actual.xbox_performance.selected[2] == 1);
    assert(actual.xbox_performance.selected[3] == 1);
    assert(actual.fps_limit == 30);
#ifdef GOW3_UWP
    assert(!SpartanSettings::GraphicsNeedRestart());
    actual.xbox_performance.selected[0] = 1;
    assert(SpartanSettings::GraphicsNeedRestart());
    actual.xbox_performance.selected[0] = 0;
#endif
    SpartanSettings::Save();
    {
        std::ifstream in(path);
        std::stringstream text;
        text << in.rdbuf();
        assert(text.str().find("xbox_record_thread=0\n") != std::string::npos);
        assert(text.str().find("xbox_metrics=1\n") != std::string::npos);
        assert(text.str().find("xbox_stream_memory=1\n") != std::string::npos);
        assert(text.str().find("fps_limit=30\n") != std::string::npos);
    }
    actual.xbox_performance.selected[0] = 1;
    SpartanSettings::Load();
    assert(actual.xbox_performance.selected[0] == 0);
    std::remove(path);
}
