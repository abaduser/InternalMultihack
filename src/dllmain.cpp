// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "hooks.h"
#include "toolbox.h"

HINSTANCE moduleHandle;

void injected_thread() {
    status_message(1, L"Injected thread started...");
    hookmanager::setup();
    // Panic key disables all hooks and breaks the while loop
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (GetAsyncKeyState(VK_DELETE) & 1) {
            hookmanager::shutdown();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            free_console();
            break;
        }
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        {
            DisableThreadLibraryCalls(hModule);
            moduleHandle = hModule;
            const auto thread = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(injected_thread), hModule, 0, NULL);
            if (thread)
                CloseHandle(thread);
        }
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

