#include "pch.h"
#include "menu.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImGuiContext* menu::CurrentContext = nullptr;
WNDPROC menu::hGameWindowProc = nullptr;
bool menu::visible = false;
bool menu::initialized = false;
HWND menu::hGameWindow = nullptr;
ImGuiMemAllocFunc menu::p_alloc_func = nullptr;
ImGuiMemFreeFunc menu::p_free_func = nullptr;

LRESULT WINAPI menu::windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
        visible = !visible;
        if (visible) {
            ShowCursor(true);
        }
        else {
            ShowCursor(false);
        }
        return false;
    }
    if (visible) {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        return true;
    }

    return CallWindowProc(hGameWindowProc, hWnd, uMsg, wParam, lParam);
}