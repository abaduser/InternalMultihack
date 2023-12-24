#pragma once
#include "../Include/MinHook.h"
std::wstring cchar_to_wstring(const char* source);
void status_message(int success, std::wstring message);
void status_message(int success, std::wstring message, const char* value);
void status_message(int success, std::wstring message, MH_STATUS status);
void status_message(int success, std::wstring message, HMODULE moduleHandle);
void free_console();