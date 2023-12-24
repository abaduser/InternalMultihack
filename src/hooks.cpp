#include "pch.h"
#include "hooks.h"
#include "toolbox.h"
#include "../Include/MinHook.h"

HMODULE opengl_handle;

const char* renderer = nullptr;
const char* vendor = nullptr;
const char* version = nullptr;

// glBegin
glBegin_t pGlBegin = nullptr;
glBegin_t pGlBeginTarget;
// glDrawElements
glDrawElements_t pGlDrawElements = nullptr;
glDrawElements_t pGlDrawElementsTarget;
// glBindTexture
glBindTexture_t pGlBindTexture = nullptr;
glBindTexture_t pGlBindTextureTarget;

struct myHook {
    const char* functionName;
    LPVOID hook_routine;
    LPVOID* original_function;
    LPVOID* target_function;
};

namespace hookmanager {
    const char* (__stdcall* glGetString)(unsigned int);

    void __stdcall hook_glDrawElements(unsigned int mode, int count, unsigned int type, const void* indices) {
        return pGlDrawElements(mode, count, type, indices);
    }

    void __stdcall hook_glBindTexture(unsigned int target, unsigned int texture) {
        return pGlBindTexture(target, texture);
    }

    void __stdcall hook_glBegin(unsigned int mode) {
        if (vendor == nullptr) {
            vendor = glGetString(GL_VENDOR);
            status_message(1, L"Vendor: ", vendor);
        }
        if (renderer == nullptr) {
            renderer = glGetString(GL_RENDERER);
            status_message(1, L"Renderer: ", renderer);
        }
        if (version == nullptr) {
               version = glGetString(GL_VERSION);
               status_message(1, L"Version: ", version);
        }
        return pGlBegin(mode);
    }
    
    void setup() {
        status_message(1, L"Hooking Manager initializing...");
        opengl_handle = nullptr;
        do {
            opengl_handle = GetModuleHandleW(L"opengl32.dll");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (opengl_handle == nullptr);
        status_message(1, L"opengl32.dll found... ", opengl_handle);

        // Setup opengl function pointers
        glGetString = (const char* (__stdcall*)(unsigned int))GetProcAddress(opengl_handle, "glGetString");
        
        // const wchar_t* modules[2] = { L"opengl32", L"bad" };
        // Map of modules and their minhooks
        std::map<const wchar_t *, std::vector<myHook> > myMap;
        // Opengl32 hooks
        std::vector<myHook> opengl32Hooks = {
            { "glBegin", &hook_glBegin, reinterpret_cast<void**>(&pGlBegin), reinterpret_cast<void**>(&pGlBeginTarget) },
            { "glDrawElements", &hook_glDrawElements, reinterpret_cast<void**>(&pGlDrawElements), reinterpret_cast<void**>(&pGlDrawElementsTarget) },
            { "glBindTexture", &hook_glBindTexture, reinterpret_cast<void**>(&pGlBindTexture), reinterpret_cast<void**>(&pGlBindTextureTarget) }
        };
        // Add the opengl32 hooks to the map
        myMap.insert(std::pair<const wchar_t*, std::vector<myHook> >(L"opengl32", opengl32Hooks));


        if (MH_Initialize() != MH_OK) {
            status_message(0, L"Failed to initialize MinHook");
            throw std::runtime_error("MH_Initialize() Failure.");
        }
        else {
            status_message(1, L"MinHook initialized...");
        }
        
        status_message(1, L"Creating Hooks...");
        for (const auto& pair : myMap) {
            auto mod = pair.first;
            auto hooks = pair.second;
            status_message(1, L"Module: " + *mod);
            for (const auto& hook : hooks) {
                status_message(1, L"Creating Hook: " + cchar_to_wstring(hook.functionName));
                MH_STATUS status = MH_CreateHookApiEx(mod, hook.functionName, hook.hook_routine, hook.original_function, hook.target_function);
                if (status != MH_OK) {
                    status_message(0, L"Failed to create hook for " + cchar_to_wstring(hook.functionName), status);
                }
                else{
                    status_message(1, L"Succeded with Code: ", status);
                }
            }
            status_message(1, L"Done Creating Hooks.");
            status_message(1, L"Enabling Hooks...");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
        MH_STATUS status = MH_Uninitialize();
        if (status != MH_OK) {
            status_message(0, L"Failed to uninitialize MinHook", status);
        }
        else {
            status_message(1, L"MinHook uninitialized...");
        }
    }
}