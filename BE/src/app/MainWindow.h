#pragma once

#include <windows.h>

namespace underlay::app{
    class MainWindow{
        public:
        explicit MainWindow(HINSTANCE instance);
        ~MainWindow();
        MainWindow(const MainWindow&) = delete;
        MainWindow& operator=(const MainWindow&) = delete;
        void Show(int showCommand);
        HWND Handle() const { return hwnd_; }

        private:
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
        HINSTANCE instance_ = nullptr;
        HWND hwnd_ = nullptr;
    };
}// namespace underlay:: app