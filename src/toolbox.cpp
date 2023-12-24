#include "pch.h"
#include "toolbox.h"

void base_message(int success, std::wstring message) {
    // Check if we have a console.
    if (GetConsoleWindow() == NULL) {
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
    }
    if (success) {
        std::wcout << L"[+] " << message << std::endl;
    }
    else {
        std::wcout << L"[-] " << message << std::endl;
    }
}

void status_message(int success, std::wstring message) {
    base_message(success, message);
}

std::wstring cchar_to_wstring(const char* source) {
    auto sourceString = std::string(source);
    return std::wstring(sourceString.begin(), sourceString.end());
}

void status_message(int success, std::wstring message, const char* value) {
    base_message(success, message + cchar_to_wstring(value));
}

void status_message(int success, std::wstring message, MH_STATUS status) {
    auto statusString = std::string(MH_StatusToString(status));
    auto statusWString = std::wstring(statusString.begin(), statusString.end());
    base_message(success, message + statusWString);
}

void status_message(int success, std::wstring message, HMODULE moduleHandle) {
    wchar_t buffer[32];
    swprintf_s(buffer, L"0x%p", moduleHandle);
    base_message(success, message + std::wstring(buffer));
}

void free_console() {
    if (GetConsoleWindow() != NULL) {
        fclose(stdout);
        FreeConsole();
    }
}