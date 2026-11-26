#pragma once
#include <filesystem>
namespace underlay::core{
    std::filesystem::path InitLog();
    void ShutdownLog();
}