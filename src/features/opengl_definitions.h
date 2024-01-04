#pragma once
#define GL_DEPTH_TEST 0x0B71
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_DEPTH_TEST 0x0B71
#define GL_TEXTURE_2D 0x0DE1
#define GL_BLEND 0x0BE2
#define GL_LINE_SMOOTH 0x0B20
#define GL_LINE_WIDTH 0x0B21
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_FRONT_AND_BACK 0x0408
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_COLOR_MATERIAL 0x0B57
#define GL_COLOR_ARRAY 0x8076
#define GL_ONE_MINUS_DST_COLOR 0x0307

//typedef void(__stdcall* glBegin_t)(unsigned int mode);
typedef FARPROC (__stdcall* GetProcAddress_t)(HMODULE hModule, LPCSTR  lpProcName);
typedef void(__stdcall* glDrawElements_t)(unsigned int mode, int count, unsigned int type, const void* indices);
typedef void(__stdcall* glBindTexture_t)(unsigned int target, unsigned int texture);
typedef BOOL(__stdcall* wglSwapBuffers_t)(HDC hdc);
typedef BOOL(__stdcall* SetCursorPos_t)(int X, int Y);
typedef BOOL(__stdcall* ClipCursor_t)(const RECT* lpRect);