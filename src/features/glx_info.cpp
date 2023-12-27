#include "../pch.h"
#include "glx_info.h"
#include "opengl_definitions.h"


void glx_info::print_glx_info(HDC& hdc) {
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
}

