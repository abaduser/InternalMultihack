#pragma once

#include "../Include/MinHook.h"
#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

namespace hookmanager {
    void __stdcall hook_glBindTexture(unsigned int target, unsigned int texture);
    void __stdcall hook_glDrawElements(unsigned int mode, int count, unsigned int type, const void* indices);
    BOOL __stdcall hook_ClipCursor(const RECT* lpRect);
    BOOL __stdcall hook_SetCursorPos(int X, int Y);
    BOOL __stdcall hook_wglSwapBuffers(HDC hdc);
    void setup();
    void shutdown();
}