#include "../Include/MinHook.h"
#include "pch.h"
#include "hooks.h"
#include "toolbox.h"
#include "menu.h"
#include "features/visuals.h"
#include "features/glx_info.h"
#include "features/opengl_definitions.h"

// wglSwapBuffers
wglSwapBuffers_t pWglSwapBuffers = nullptr;
wglSwapBuffers_t pWglSwapBuffersTarget;
// glDrawElements
glDrawElements_t pGlDrawElements = nullptr;
glDrawElements_t pGlDrawElementsTarget;
// glBindTexture
glBindTexture_t pGlBindTexture = nullptr;
glBindTexture_t pGlBindTextureTarget;
// ClipCursor
ClipCursor_t pClipCursor = nullptr;
ClipCursor_t pClipCursorTarget;
// SetCursorPos
SetCursorPos_t pSetCursorPos = nullptr;
SetCursorPos_t pSetCursorPosTarget;

struct myHook {
    const char* functionName;
    LPVOID hook_routine;
    LPVOID* original_function;
    LPVOID* target_function;
};

namespace hookmanager {
    HMODULE opengl_handle;
    glx_info* glx_info_ptr = nullptr;
    menu* main_menu = nullptr;
    visuals* visuals_ptr = nullptr;

    // Map of modules and their minhooks
    std::map<const wchar_t*, std::vector<myHook> > modulehookmap;

    // TODO: finish menu logic
    BOOL __stdcall hook_ClipCursor(const RECT* lpRect) {
        static RECT WindowSize;
        if (main_menu->visible) {
            GetWindowRect(main_menu->hGameWindow, &WindowSize);
            return pClipCursor(&WindowSize);
        }
        return pClipCursor(lpRect);
    }

    BOOL  __stdcall hook_SetCursorPos(int X, int Y) {
        if (main_menu->visible) {
            return true;
        }
        return pSetCursorPos(X, Y);
    }
    
    void __stdcall hook_glDrawElements(unsigned int mode, int count, unsigned int type, const void* indices) {
        visuals_ptr->draw_visuals(mode, count, type, indices, pGlDrawElements);
        return pGlDrawElements(mode, count, type, indices);
    }

    void __stdcall hook_glBindTexture(unsigned int target, unsigned int texture) {
        const unsigned char* modelPath = nullptr;
        __asm {
            pushad
            mov modelPath, esi
            popad
            pushad
        }
        if (modelPath != nullptr && (unsigned char*)(0x1000) < modelPath && (texture != 0)) {
            visuals_ptr->setCurrentmodel(modelPath);
        }
        __asm {
            popad
        }
        return pGlBindTexture(target, texture);
    }

    BOOL __stdcall hook_wglSwapBuffers(HDC hdc) {
        if (glx_info_ptr != nullptr) {
            glx_info_ptr->print_glx_info(hdc);
        }

        // SetAllocatorFunctions 
        // Initialize glew and imgui but only once
        if (!main_menu->initialized) {
            // Setup menu
            main_menu->initialize(hdc, WindowFromDC(hdc));
            // Store context
            // create menu and set initialized to true
            // Get the game's window from it's handle
            // Overwrite the game's wndProc function
            glewInit();
            // Store the context for later use
            main_menu->set_context(ImGui::CreateContext());
            // Get the allocator functions, see imgui.cpp:1108 for more info
            // tl;dr for ImGui calls from outside the DLL boundaries
            ImGui::GetAllocatorFunctions(&main_menu->p_alloc_func, &main_menu->p_free_func, &main_menu->p_user_data);
            // Setup the imgui window
            main_menu->CreateImGui(hdc);
            main_menu->initialized = true;
        }

        // If the menu is shown, start a new frame and draw the demo window
        if (main_menu->visible) {
            main_menu->PreRender();
            main_menu->Render();
            main_menu->PostRender();
        }

        return pWglSwapBuffers(hdc);
    }
    
    void setup() {
        status_message(1, L"Hooking Manager initializing...");
        
        // Get a handle to opengl32.dlll
        opengl_handle = nullptr;
        do {
            opengl_handle = GetModuleHandleW(L"opengl32.dll");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (opengl_handle == nullptr);
        status_message(1, L"opengl32.dll found... ", opengl_handle);

        glx_info_ptr = new glx_info(opengl_handle);
        visuals_ptr = new visuals(opengl_handle);
        main_menu = new menu();
        main_menu->add_feature(std::shared_ptr<features>(glx_info_ptr));
        main_menu->add_feature(std::shared_ptr<features>(visuals_ptr));
        // user32 hooks
        std::vector<myHook> user32Hooks = {
            { "ClipCursor", &hook_ClipCursor, reinterpret_cast<void**>(&pClipCursor), reinterpret_cast<void**>(&pClipCursorTarget) },
            { "SetCursorPos", &hook_SetCursorPos, reinterpret_cast<void**>(&pSetCursorPos), reinterpret_cast<void**>(&pSetCursorPosTarget) }
        };
        // Opengl32 hooks
        std::vector<myHook> opengl32Hooks = {
            { "wglSwapBuffers", &hook_wglSwapBuffers, reinterpret_cast<void**>(&pWglSwapBuffers), reinterpret_cast<void**>(&pWglSwapBuffersTarget) },
            { "glDrawElements", &hook_glDrawElements, reinterpret_cast<void**>(&pGlDrawElements), reinterpret_cast<void**>(&pGlDrawElementsTarget) },
            { "glBindTexture", &hook_glBindTexture, reinterpret_cast<void**>(&pGlBindTexture), reinterpret_cast<void**>(&pGlBindTextureTarget) }
        };
        modulehookmap.insert(std::pair<const wchar_t*, std::vector<myHook> >(L"user32", user32Hooks));
        modulehookmap.insert(std::pair<const wchar_t*, std::vector<myHook> >(L"opengl32", opengl32Hooks));
        

        // Begin MinHook
        if (MH_Initialize() != MH_OK) {
            status_message(0, L"Failed to initialize MinHook");
            throw std::runtime_error("MH_Initialize() Failure.");
        }
        else {
            status_message(1, L"MinHook initialized...");
        }
        // Install the Hooks (Live)
        status_message(1, L"Installing Hooks...");
        for (const auto& pair : modulehookmap) {
            auto mod = pair.first;
            auto hooks = pair.second;
            status_message(1, L"Module: " + std::wstring(mod));
            for (const auto& hook : hooks) {
                status_message(1, L"Installing Hook: " + cchar_to_wstring(hook.functionName));
                MH_STATUS status = MH_CreateHookApiEx(mod, hook.functionName, hook.hook_routine, hook.original_function, hook.target_function);
                if (status != MH_OK) {
                    status_message(0, L"Failed to create hook for " + cchar_to_wstring(hook.functionName), status);
                }
            }
            status_message(1, L"Done Creating Hooks.");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        for (const auto& pair : modulehookmap) {
            auto mod = pair.first;
            auto hooks = pair.second;
            status_message(1, L"Enabling Hooks...");
            for (const auto& hook : hooks) {
                MH_STATUS status = MH_EnableHook(*hook.target_function);
                if (status != MH_OK) {
                    status_message(0, L"Failed to enable hook for " + cchar_to_wstring(hook.functionName), status);
                }
            }
            status_message(1, L"Done Enabling Hooks.");
        }

        status_message(1, L"Hooking Manager Done.");
    }

    void shutdown() {
        status_message(1, L"Hooking Manager shutting down...");
        for (const auto& pair : modulehookmap) {
            auto mod = pair.first;
            auto hooks = pair.second;
            status_message(0, L"Disabling Hooks...");
            for (const auto& hook : hooks) {
                MH_STATUS status = MH_DisableHook(*hook.target_function);
                if (status != MH_OK) {
                    status_message(0, L"Failed to disable hook for " + cchar_to_wstring(hook.functionName), status);
                }
            }
            status_message(1, L"Done Disabling Hooks.");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        main_menu->DestroyImGui();
        delete main_menu;

        MH_STATUS status = MH_Uninitialize();
        if (status != MH_OK) {
            status_message(0, L"Failed to uninitialize MinHook", status);
        }
        else {
            status_message(1, L"MinHook uninitialized...");
        }
    }
}