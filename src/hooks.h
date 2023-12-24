#pragma once

#include "../Include/MinHook.h"
#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif
#define GL_DEPTH_TEST 0x0B71
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02

typedef void(__stdcall* glBegin_t)(unsigned int mode);
typedef void(__stdcall* glDrawElements_t)(unsigned int mode, int count, unsigned int type, const void* indices);
typedef void(__stdcall* glBindTexture_t)(unsigned int target, unsigned int texture);
typedef BOOL(__stdcall* wglSwapBuffers_t)(HDC hdc);
typedef BOOL(__stdcall* SetCursorPos_t)(int X, int Y);
typedef BOOL(__stdcall* ClipCursor_t)(const RECT* lpRect);



namespace hookmanager {
    void __stdcall hook_glBindTexture(unsigned int target, unsigned int texture);
    void __stdcall hook_glDrawElements(unsigned int mode, int count, unsigned int type, const void* indices);
    void setup();
    void shutdown();
}