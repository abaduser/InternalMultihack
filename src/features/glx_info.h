#pragma once
#include "../toolbox.h"
class glx_info : public features {
    const char* renderer = nullptr;
    const char* vendor = nullptr;
    const char* version = nullptr;
    const char* (__stdcall* glGetString)(unsigned int);
public:
    glx_info(HMODULE opengl_handle) {
        glGetString = (const char* (__stdcall*)(unsigned int))GetProcAddress(opengl_handle, "glGetString");
    }
    ~glx_info() {}
    void print_glx_info(HDC& hdc);
    void draw_menu() override {
        ImGui::Text("GL version:");
        ImGui::SameLine();
        if (!std::string(version).empty()) {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), version);
        }
        else {
            ImGui::Text("null");
        }
        ImGui::SameLine();
        ImGui::Text("Renderer:");
        ImGui::SameLine();
        if (!std::string(renderer).empty()) {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), renderer);
        }
        else {
            ImGui::Text("null");
        }
        ImGui::Separator();
    }
};