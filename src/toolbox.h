#pragma once
#include "../Include/MinHook.h"
#include <conio.h>
std::wstring cchar_to_wstring(const char* source);
bool stristr(const char* haystack, const char* needle);
void status_message(int success, std::wstring message);
void status_message(int success, std::wstring message, const char* value);
void status_message(int success, std::wstring message, int i);
void status_message(int success, std::wstring message, MH_STATUS status);
void status_message(int success, std::wstring message, HMODULE moduleHandle);
void free_console();

// Put this somewhere else
class features {
public:
    virtual ~features() = default;
    virtual void draw_menu() = 0;
};