#include "app/MainWindow.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

#include "resource.h"

namespace underlay::app {
namespace {

constexpr wchar_t kClassName[] = L"UnderlayMainWindow";
constexpr wchar_t kTitle[] = L"UNDERLAY";
constexpr int kInitialWidth = 960;
constexpr int kInitialHeight = 540;

}  // namespace

MainWindow::MainWindow(HINSTANCE instance) : instance_(instance) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &MainWindow::WndProc;
    wc.hInstance = instance_;
    wc.hIcon = LoadIconW(instance_, MAKEINTRESOURCEW(IDI_APP_ICON));
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    wc.lpszClassName = kClassName;
    if (RegisterClassExW(&wc) == 0) {
        throw std::runtime_error("RegisterClassExW failed");
    }

    // 마지막 인자 this: WM_NCCREATE에서 꺼내 창(HWND)과 이 객체를 잇는다
    hwnd_ = CreateWindowExW(0, kClassName, kTitle, WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, kInitialWidth, kInitialHeight,
                            nullptr, nullptr, instance_, this);
    if (hwnd_ == nullptr) {
        throw std::runtime_error("CreateWindowExW failed");
    }
    spdlog::info("main window created");
}

MainWindow::~MainWindow() {
    if (hwnd_ != nullptr) {
        DestroyWindow(hwnd_);
    }
    UnregisterClassW(kClassName, instance_);
}

void MainWindow::Show(int showCommand) {
    ShowWindow(hwnd_, showCommand);
    UpdateWindow(hwnd_);
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCCREATE) {
        const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lParam);
        auto* window = static_cast<MainWindow*>(create->lpCreateParams);
        window->hwnd_ = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    }

    auto* self = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (self == nullptr) {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return self->HandleMessage(message, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            spdlog::info("main window destroyed");
            SetWindowLongPtrW(hwnd_, GWLP_USERDATA, 0);
            hwnd_ = nullptr;  // 창은 이미 사라지는 중: 소멸자가 두 번 닫지 않게
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd_, message, wParam, lParam);
    }
}

}  // namespace underlay::app