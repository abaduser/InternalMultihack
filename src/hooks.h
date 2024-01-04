#pragma once

#include "../Include/MinHook.h"
#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

struct myHook {
    const char* functionName;
    LPVOID hook_routine;
    LPVOID* original_function;
    LPVOID* target_function;
};
namespace hookmanager {
    typedef int (*VMMain)(int, int, int, int, int, int, int, int, int, int, int, int, int);
    extern VMMain originalVMMain;
    typedef int (*DLLENTRY)(int(QDECL*)(int, ...));
    extern DLLENTRY originalDLLEntry;
    typedef void(__stdcall* VMMain_t)(intptr_t(QDECL* syscallptr)(intptr_t arg, ...));

    FARPROC WINAPI __stdcall hook_GetProcAddress(HMODULE hModule, LPCSTR  lpProcName);
    void __stdcall hook_glBindTexture(unsigned int target, unsigned int texture);
    void __stdcall hook_glDrawElements(unsigned int mode, int count, unsigned int type, const void* indices);
    BOOL __stdcall hook_ClipCursor(const RECT* lpRect);
    BOOL __stdcall hook_SetCursorPos(int X, int Y);
    BOOL __stdcall hook_wglSwapBuffers(HDC hdc);
    int syscall_hook(int cmd, ...);
    void setup();
    void shutdown();
}