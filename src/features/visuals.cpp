#include "../pch.h"
#include "visuals.h"

// Pointers to OpenGL functions inside opengl32.dll (quakelive_steam.exe)
//const char*(__stdcall* pGlGetString)(unsigned int) = nullptr;
void(__stdcall* pGlEnable)(unsigned int) = nullptr;
void(__stdcall* pGlDisable)(unsigned int) = nullptr;
void(__stdcall* pGlBlendFunc)(unsigned int, unsigned int) = nullptr;
void(__stdcall* pGlPushMatrix)() = nullptr;
void(__stdcall* pGlColor4f)(float, float, float, float) = nullptr;
void(__stdcall* pGlPopMatrix)() = nullptr;
void(__stdcall* pGlDisableClientState)(unsigned int) = nullptr;
void(__stdcall* pGlLineWidth)(float) = nullptr;
void(__stdcall* pGlPolygonMode)(unsigned int, unsigned int) = nullptr;
void(__stdcall* pGlPushAttrib)(unsigned int) = nullptr;
void(__stdcall* pGlPopAttrib)() = nullptr;

void draw_visuals() {
    // draw a cham
    // requires : openglhook: glbindtexture, gldrawelements. options().
    
}