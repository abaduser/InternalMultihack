#pragma once
#include "toolbox.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

class menu {
    int WIDTH = 400;
    int HEIGHT = 300;
    // TODO: figure out how to make a feature class that can be added to the menu class?
    std::vector<std::shared_ptr<features>> installed_features;
public:
    static ImGuiContext* CurrentContext;
    static WNDPROC hGameWindowProc;
    static bool visible;
    static bool initialized;
    static HWND hGameWindow;
    static ImGuiMemAllocFunc p_alloc_func;
    static ImGuiMemFreeFunc p_free_func;

    void* p_user_data = nullptr;
    menu() {};
    ~menu() = default;

    void set_visible(bool visible) {
        this->visible = visible;
    }

    void set_context(ImGuiContext* context) {
        CurrentContext = context;
    }

    void CreateImGui(HDC hDc) noexcept {
        LoadContext();
        ImGui_ImplWin32_Init(hGameWindow);
        ImGui_ImplOpenGL3_Init();
        ImGui::StyleColorsClassic();
        ImGui::GetStyle().AntiAliasedFill = false;
        ImGui::GetStyle().AntiAliasedLines = false;
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
        ImGui::GetStyle().WindowMinSize = ImVec2((float)WIDTH, (float)HEIGHT);
    }

    void DestroyImGui() noexcept {
        LoadContext();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext(CurrentContext);
        SetWindowLongPtr(menu::hGameWindow, GWLP_WNDPROC, (LONG_PTR)menu::hGameWindowProc);
    }

    void add_feature(std::shared_ptr<features> f) {
        installed_features.push_back(f);
    }

    void Render() noexcept {
        LoadContext();
        ImGui::ShowDemoWindow();
        if (ImGui::Begin("InternalMenu", NULL, ImGuiWindowFlags_NoTitleBar)) {
            // Header
            ImGui::Text("Glue sniffer edition | Version: 0.6a | Use at your own risk. Not for public use.");
            for (auto& f : installed_features) {
                f->draw_menu();
            }
            ImGui::End();
        }
    }

    void PreRender() noexcept {
        LoadContext();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void PostRender() noexcept {
        // Done Drawing
        ImGui::Render();
        LoadContext();
        // Draw the overlay
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void LoadContext() noexcept {
        // Required to restore context from dll
        ImGui::SetAllocatorFunctions(*p_alloc_func, *p_free_func, p_user_data);
        ImGui::SetCurrentContext(CurrentContext);
    }

    void initialize(HDC hdc, HWND gamewindow) {
        hGameWindow = gamewindow;
        hGameWindowProc = (WNDPROC)SetWindowLongPtr(hGameWindow, GWLP_WNDPROC, (LONG_PTR)windowProc_hook);
        status_message(0, L"Menu already initialized");
        return;
    }

    static LRESULT WINAPI windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
};