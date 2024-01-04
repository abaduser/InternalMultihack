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

bool stristr(const char* haystack, const char* needle) {
    if (!haystack || !needle) return false;

    do {
        const char* h = haystack;
        const char* n = needle;
        while (tolower((unsigned char)*h) == tolower((unsigned char)*n) && *n) {
            h++;
            n++;
        }
        if (*n == 0) return true;
    } while (*haystack++);

    return false;
}

void status_message(int success, std::wstring message) {
    base_message(success, message);
}

std::wstring cchar_to_wstring(const char* source) {
    auto sourceString = std::string(source);
    return std::wstring(sourceString.begin(), sourceString.end());
}

void status_message(int success, std::wstring message, int i) {
    base_message(success, message + std::to_wstring(i));
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