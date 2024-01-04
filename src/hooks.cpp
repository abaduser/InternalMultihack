#include "../Include/MinHook.h"
#include "pch.h"
#include "hooks.h"
#include "toolbox.h"
#include "menu.h"
#include "features/visuals.h"
#include "features/glx_info.h"
#include "features/opengl_definitions.h"


// getProcAddress
GetProcAddress_t pGetProcAddress = nullptr;
GetProcAddress_t pGetProcAddressTarget;

// wglSwapBuffers
wglSwapBuffers_t pWglSwapBuffers = nullptr;
wglSwapBuffers_t pWglSwapBuffersTarget;
// glDrawElements
glDrawElements_t pGlDrawElements = nullptr;
glDrawElements_t pGlDrawElementsTarget;
// glBindTexture
glBindTexture_t pGlBindTexture = nullptr;
glBindTexture_t pGlBindTextureTarget;
// ClipCursor
ClipCursor_t pClipCursor = nullptr;
ClipCursor_t pClipCursorTarget;
// SetCursorPos
SetCursorPos_t pSetCursorPos = nullptr;
SetCursorPos_t pSetCursorPosTarget;

int(QDECL* syscall)(int arg, ...) = (int(QDECL*)(int, ...)) - 1;

namespace hookmanager {
    // handle for opengl
    HMODULE opengl_handle;
    
    VMMain originalVMMain;
    DLLENTRY originalDLLEntry;
    cgs_t* client_gameState = nullptr;
    cg_t* client_game = nullptr;
    centity_t* pPlayerEnt;
    centity_t* pEntities[MAX_GENTITIES];
    gameState_t* gameState = nullptr;
    playerState_t* ps = nullptr;

    // Pointers to feature objects. These are used to call the feature's functions
    glx_info* glx_info_ptr = nullptr;
    menu* main_menu = nullptr;
    visuals* visuals_ptr = nullptr;
    // Map of modules and pointers minhook uses to hook functions
    std::map<const wchar_t*, std::vector<myHook> > modulehookmap;

    int syscall_hook(int cmd, ...) {

        // this is a variadic function, so this gets all parameters
        // using va_arg
        int arg[14];
        va_list arglist;
        int count;
        va_start(arglist, cmd);
        for (count = 0; count < 14; count++)
        {
            arg[count] = va_arg(arglist, int);
        }
        va_end(arglist);

        // check which syscall has been requested
        switch (cmd)
        {
        /* Doesn't exist in q3
        // Gets information on an entity using an engine struct
        case CG_GETDEFAULTSTATE:
        {
            centity_t* cur = (centity_t*)arg[1];
            pEntities[arg[0]] = cur;

            // if it's our local player entity, save the pointer
            if (ps && cur->currentState.clientNum == ps->clientNum)
            {
                pPlayerEnt = cur;
            }
            break;
        }
        */

        // the VM processed the current game state
        // --> steal the parameters
        case CG_GETGAMESTATE:
        {
            gameState = (gameState_t*)arg[0];
            // the gameState_t* element is wrapped by a cgs_t struct, so
            // using pointer arithmetic it's possible to get the "parent" element
            // --> the cgs_t struct
            cgs_t* _tmp = 0;
            client_gameState = (cgs_t*)((int)gameState - (int)&_tmp->gameState);
            break;
        }

        // get own playerstate
        case CG_GETSNAPSHOT:
        {
            // call the real syscall first so the struct will be prepared for the following calls
            auto result = syscall(cmd, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9], arg[10], arg[11], arg[12], arg[13]);

            snapshot_t* snap = (snapshot_t*)arg[1];
            // get the current player state
            ps = &(snap->ps);

            cg_t* tmp = 0;
            // we have an `activeSnapshots` object, so let's subtract its length and get the parent element
            client_game = (cg_t*)((int)arg[1] - (int)&tmp->activeSnapshots);

            // Process the snapshot data
            for (int i = 0; i < snap->numEntities; i++) {
                // Save all entities into pEntities
                pEntities[i] = (centity_t *) & snap->entities[i];
                // Process each centity_t as needed
                if (pEntities[i]->currentState.eType == ET_PLAYER) {
                    // Save the local player entity
                    if (pEntities[i]->currentState.clientNum == ps->clientNum) {
                        pPlayerEnt = pEntities[i];
                        status_message(1, L"Found local player entity... clientNum", ps->clientNum);
                    }
                }
            }
            return result;
            break;
        }

        default:
            break;
        }

        // execute the original syscall using the passed parameters
        return syscall(cmd, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9], arg[10], arg[11], arg[12], arg[13]);
    }

    int hookVMMain(int cmd, int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l) {
        switch (cmd)
        {

            // initializing the game vm (game startup)
        case CG_INIT:
        {
            // Menu should be marked ready here

            // delete data from previous game VMs
            pPlayerEnt = nullptr;

            // call the real method
            int result = originalVMMain(cmd, a, b, c, d, e, f, g, h, i, j, k, l);
            // init any shaders here
            return result;
        }

        // this gets called each time a frame will be drawn
        case CG_DRAW_ACTIVE_FRAME:
        {
            int result = originalVMMain(cmd, a, b, c, d, e, f, g, h, i, j, k, l);

            /*
                // enable cheat protected cvars
                if (CHEATS.value)
                {
                    // unlock blocked and cheat protected cvars
                    // by overriding the server provided cvar
                    // prevent overwriting it by setting it each frame
                    syscall_hook(CG_CVAR_SET, "sv_cheats", "1");
                }



            if (TRIGGERBOT.value && client_game)
            {
                // only trigger with a shootable weapon
                if (ps->weapon > WP_SABER && ps->weapon != WP_THERMAL && ps->weapon != WP_TRIP_MINE && ps->weapon != WP_DET_PACK)
                {
                    int crosshairClientNum = client_game->crosshairClientNum;
                    if (crosshairClientNum >= 0 && crosshairClientNum <= MAX_CLIENTS)
                    {
                        // only shoot at enemies
                        if (isEnemy(crosshairClientNum))
                        {
                            // attack
                            syscall_hook(CG_SENDCONSOLECOMMAND, "+attack; -attack;");
                        }
                    }
                }
            }

            if (ANTIGRIP.value && client_game)
            {
                // If we are currently being gripped
                if (ps && (ps->fd.forceGripBeingGripped || ps->fd.forceGripCripple))
                {
                    setEntToLastAttacker(pCurPushTarget);
                    focusEnt(pCurPushTarget);
                    syscall_hook(CG_SENDCONSOLECOMMAND, "force_throw;");
                    syscall_hook(CG_SENDCONSOLECOMMAND, "force_throw;");
                    syscall_hook(CG_SENDCONSOLECOMMAND, "force_throw;");
                    syscall_hook(CG_SENDCONSOLECOMMAND, "force_throw;");
                }
            }

            if (AIM.value && client_game)
            {
                if (GetAsyncKeyState(AIM_KEY))
                {
                    // no target, get it first
                    if (!pCurAimTarget)
                    {
                        int crosshairClientNum = client_game->crosshairClientNum;
                        auto ent = entFromClientNum(crosshairClientNum);
                        if (ent)
                        {
                            pCurAimTarget = ent;

                            // tell the local player who we are aiming on
                            if (client_gameState)
                            {
                                auto tmpTarget = client_gameState->clientinfo[crosshairClientNum];
                                curAimTargetName = tmpTarget.name;
                                std::string txt = " Aiming @ " + curAimTargetName;
                                log(txt);
                            }
                        }
                    }
                    if (pCurAimTarget)
                    {
                        if (!focusEnt(pCurAimTarget))
                        {
                            // enemy dead or not valid
                            pCurAimTarget = nullptr;
                        }
                    }
                }
                // reset the target
                else
                {
                    pCurAimTarget = nullptr;
                }
            }

            // reset or it's not possible to distinguish between nothing and client 0
            if (client_game)
            {
                client_game->crosshairClientNum = -1;
            }
            */
            return result;
        }

        default:
            break;
        }
        return originalVMMain(cmd, a, b, c, d, e, f, g, h, i, j, k, l);
    }

    void hookDLLEntry(int(QDECL* syscallptr)(int arg, ...))
    {
        syscall = syscallptr;
        originalDLLEntry(syscall_hook);
    }

    FARPROC WINAPI __stdcall hook_GetProcAddress(HMODULE hModule, LPCSTR  lpProcName) {
        CHAR moduleName[MAX_PATH];
        // check in which dll the call will be executed
        if (!GetModuleFileName(hModule, moduleName, sizeof(moduleName))) {
            return (FARPROC)pGetProcAddress(hModule, lpProcName);
        }
        // are we in the game vm?
        if (stristr(moduleName, "cgamex86.dll")) {
            if (stristr(lpProcName, "dllEntry")) {
                status_message(1, L"Hooking cgamex86.dll : dllEntry ... ", hModule);
                // Modify returned function here
                // --> execute modified function instead
                // (modified function calls original function after doing hax using `originalDLLEntry`)
                originalDLLEntry = (DLLENTRY)pGetProcAddress(hModule, lpProcName);
                return (PROC)hookDLLEntry;
            }
            // Save things as above, only for vmMain
            if (stristr(lpProcName, "vmMain")) {
                status_message(1, L"Hooking cgamex86.dll : vmMain ... ", hModule);
                originalVMMain = (VMMain)pGetProcAddress(hModule, lpProcName);
                return (PROC)hookVMMain;
            }
        }
        return (FARPROC)pGetProcAddress(hModule, lpProcName);
    }

    // TODO: finish menu logic
    BOOL __stdcall hook_ClipCursor(const RECT* lpRect) {
        static RECT WindowSize;
        if (main_menu->visible) {
            GetWindowRect(main_menu->hGameWindow, &WindowSize);
            return pClipCursor(&WindowSize);
        }
        return pClipCursor(lpRect);
    }

    BOOL  __stdcall hook_SetCursorPos(int X, int Y) {
        if (main_menu->visible) {
            return true;
        }
        return pSetCursorPos(X, Y);
    }
    
    void __stdcall hook_glDrawElements(unsigned int mode, int count, unsigned int type, const void* indices) {
        visuals_ptr->draw_visuals(mode, count, type, indices, pGlDrawElements);
        return pGlDrawElements(mode, count, type, indices);
    }

    void __stdcall hook_glBindTexture(unsigned int target, unsigned int texture) {
        const unsigned char* modelPath = nullptr;
        __asm {
            pushad
            mov modelPath, esi
            popad
            pushad
        }
        if (modelPath != nullptr && (unsigned char*)(0x1000) < modelPath && (texture != 0)) {
            visuals_ptr->setCurrentmodel(modelPath);
        }
        __asm {
            popad
        }
        return pGlBindTexture(target, texture);
    }

    BOOL __stdcall hook_wglSwapBuffers(HDC hdc) {
        if (glx_info_ptr != nullptr) {
            glx_info_ptr->print_glx_info(hdc);
        }

        // SetAllocatorFunctions 
        // Initialize glew and imgui but only once
        if (!main_menu->initialized) {
            main_menu->initialize(hdc, WindowFromDC(hdc));
            glewInit();
            main_menu->set_context(ImGui::CreateContext());
            // Get the allocator functions, see imgui.cpp:1108 for more info
            // tl;dr for ImGui calls from outside the DLL boundaries
            ImGui::GetAllocatorFunctions(&main_menu->p_alloc_func, &main_menu->p_free_func, &main_menu->p_user_data);
            main_menu->CreateImGui(hdc);
            main_menu->initialized = true;
        }
        if (main_menu->visible) {
            main_menu->PreRender();
            main_menu->Render();
            main_menu->PostRender();
        }

        return pWglSwapBuffers(hdc);
    }
    
    void setup() {
        status_message(1, L"Hooking Manager initializing...");
        
        // Get a handle to opengl32.dlll
        opengl_handle = nullptr;
        do {
            opengl_handle = GetModuleHandleW(L"opengl32.dll");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (opengl_handle == nullptr);
        status_message(1, L"opengl32.dll found... ", opengl_handle);

        glx_info_ptr = new glx_info(opengl_handle);
        visuals_ptr = new visuals(opengl_handle);
        main_menu = new menu();
        main_menu->add_feature(std::shared_ptr<features>(glx_info_ptr));
        main_menu->add_feature(std::shared_ptr<features>(visuals_ptr));
        std::vector<myHook> kernel32Hooks = {
            { "GetProcAddress", &hook_GetProcAddress, reinterpret_cast<void**>(&pGetProcAddress), reinterpret_cast<void**>(&pGetProcAddressTarget) }
        };
        // user32 hooks
        std::vector<myHook> user32Hooks = {
            { "ClipCursor", &hook_ClipCursor, reinterpret_cast<void**>(&pClipCursor), reinterpret_cast<void**>(&pClipCursorTarget) },
            { "SetCursorPos", &hook_SetCursorPos, reinterpret_cast<void**>(&pSetCursorPos), reinterpret_cast<void**>(&pSetCursorPosTarget) }
        };
        // Opengl32 hooks
        std::vector<myHook> opengl32Hooks = {
            { "wglSwapBuffers", &hook_wglSwapBuffers, reinterpret_cast<void**>(&pWglSwapBuffers), reinterpret_cast<void**>(&pWglSwapBuffersTarget) },
            { "glDrawElements", &hook_glDrawElements, reinterpret_cast<void**>(&pGlDrawElements), reinterpret_cast<void**>(&pGlDrawElementsTarget) },
            { "glBindTexture", &hook_glBindTexture, reinterpret_cast<void**>(&pGlBindTexture), reinterpret_cast<void**>(&pGlBindTextureTarget) }
        };
        modulehookmap.insert(std::pair<const wchar_t*, std::vector<myHook> >(L"user32", user32Hooks));
        modulehookmap.insert(std::pair<const wchar_t*, std::vector<myHook> >(L"kernel32", kernel32Hooks));
        modulehookmap.insert(std::pair<const wchar_t*, std::vector<myHook> >(L"opengl32", opengl32Hooks));
        

        // Begin MinHook
        if (MH_Initialize() != MH_OK) {
            status_message(0, L"Failed to initialize MinHook");
            throw std::runtime_error("MH_Initialize() Failure.");
        }
        else {
            status_message(1, L"MinHook initialized...");
        }
        // Install the Hooks (Live)
        status_message(1, L"Installing Hooks...");
        for (const auto& pair : modulehookmap) {
            auto mod = pair.first;
            auto hooks = pair.second;
            status_message(1, L"Module: " + std::wstring(mod));
            for (const auto& hook : hooks) {
                status_message(1, L"Installing Hook: " + cchar_to_wstring(hook.functionName));
                MH_STATUS status = MH_CreateHookApiEx(mod, hook.functionName, hook.hook_routine, hook.original_function, hook.target_function);
                if (status != MH_OK) {
                    status_message(0, L"Failed to create hook for " + cchar_to_wstring(hook.functionName), status);
                }
            }
            status_message(1, L"Done Creating Hooks.");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        for (const auto& pair : modulehookmap) {
            auto mod = pair.first;
            auto hooks = pair.second;
            status_message(1, L"Enabling Hooks...");
            for (const auto& hook : hooks) {
                MH_STATUS status = MH_EnableHook(*hook.target_function);
                if (status != MH_OK) {
                    status_message(0, L"Failed to enable hook for " + cchar_to_wstring(hook.functionName), status);
                }
            }
            status_message(1, L"Done Enabling Hooks.");
        }

        status_message(1, L"Hooking Manager Done.");
    }

    void shutdown() {
        status_message(1, L"Hooking Manager shutting down...");
        for (const auto& pair : modulehookmap) {
            auto mod = pair.first;
            auto hooks = pair.second;
            status_message(0, L"Disabling Hooks...");
            for (const auto& hook : hooks) {
                MH_STATUS status = MH_DisableHook(*hook.target_function);
                if (status != MH_OK) {
                    status_message(0, L"Failed to disable hook for " + cchar_to_wstring(hook.functionName), status);
                }
            }
            status_message(1, L"Done Disabling Hooks.");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        main_menu->DestroyImGui();
        delete main_menu;

        MH_STATUS status = MH_Uninitialize();
        if (status != MH_OK) {
            status_message(0, L"Failed to uninitialize MinHook", status);
        }
        else {
            status_message(1, L"MinHook uninitialized...");
        }
    }
}