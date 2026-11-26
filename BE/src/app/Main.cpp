#include <windows.h>

#include <spdlog/spdlog.h>

#include <exception>

#include "app/MainWindow.h"
#include "core/Log.h"

namespace{
    int RunApp(HINSTANCE instance, int showCommand){
        underlay::app::MainWindow window(instance);
        window.Show(showCommand);
        MSG msg{};
        while(GetMessage(&msg, nullptr, 0, 0) > 0){
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return static_cast<int>(msg.wParam);
    }

}
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand){
    try{
        underlay::core::InitLog();
        spdlog::info("UNDERLAY starting");
        const int exitCode = RunApp(instance, showCommand);
        spdlog::info("UNDERLAY exiting (code{})", exitCode);
        underlay::core::ShutdownLog();
        return exitCode;
    }catch (const std::exception& e){
        spdlog::critical("fatal: {}", e.what());
        underlay::core::ShutdownLog();
   MessageBoxW(nullptr, L"UNDERLAY failed to start. See the log file.", L"UNDERLAY", MB_ICONERROR | MB_OK);
   return 1;
    }
}