#include "core/Log.h"

#include <windows.h>
#include <shlobj.h>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <stdexcept>

namespace underlay::core {
namespace {

// %LOCALAPPDATA% 경로를 Windows에 물어본다 (예: C:\Users\<이름>\AppData\Local)
std::filesystem::path LocalAppDataDir() {
    PWSTR raw = nullptr;
    const HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &raw);

    std::filesystem::path result;
    if (SUCCEEDED(hr)) {
        result = raw;
    }
    CoTaskMemFree(raw);  // 성공·실패와 상관없이 Windows가 준 메모리는 돌려준다

    if (FAILED(hr)) {
        throw std::runtime_error("SHGetKnownFolderPath(FOLDERID_LocalAppData) failed");
    }
    return result;
}

}  // namespace

std::filesystem::path InitLog() {
    const std::filesystem::path dir = LocalAppDataDir() / L"UNDERLAY" / L"logs";
    std::filesystem::create_directories(dir);
    const std::filesystem::path file = dir / L"underlay.log";

    constexpr std::size_t kMaxFileSize = 5 * 1024 * 1024;  // 5 MB
    constexpr std::size_t kMaxFiles = 3;
    auto logger = spdlog::rotating_logger_mt("underlay", file.string(), kMaxFileSize, kMaxFiles);

    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::info);
    spdlog::info("UNDERLAY log started");
    return file;
}

void ShutdownLog() {
    spdlog::shutdown();
}

}  // namespace underlay::core