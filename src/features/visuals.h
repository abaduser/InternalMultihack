#pragma once
#include "opengl_definitions.h"

typedef enum {
    PLAYER,
    ITEM,
    PROJECTILE,
    OTHER
} ModelTypeEnum;

typedef enum {
    BLUE_TEAM,
    RED_TEAM,
    BRIGHT_TEAM,
    NO_TEAM
} ModelTeamEnum;

struct BoundModel {
    const unsigned char* modelPath;
    ModelTypeEnum modelType;
    ModelTeamEnum modelTeam;
};

// Object representing the cham feature, its state and some methods to interact with it
class chams {
    bool bEnabled = true;
    bool bPlayer = true;
    bool bItem = false;
    bool bProjectile = false;

    float playerColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    float itemColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
    float projectileColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
public:
    bool* pEnabled = &bEnabled;
    bool* pPlayer = &bPlayer;
    bool* pItem = &bItem;
    bool* pProjectile = &bProjectile;

    float* pPlayerColor = playerColor;
    float* pItemColor = itemColor;
    float* pProjectileColor = projectileColor;
    chams() {}
    ~chams() = default;

    float* get_playercolor() {
        return playerColor;
    }

    float* get_itemcolor() {
        return itemColor;
    }
    
    float* get_projectilecolor() {
        return projectileColor;
    }

    bool get_enabled() {
        return bEnabled;
    }
    void toggle_chams() {
        bEnabled = !bEnabled;
    }

    bool get_chams_player() {
        return bPlayer;
    }
    void toggle_chams_player() {
        bPlayer = !bPlayer;
    }

    bool get_chams_item() {
        return bItem;
    }
    void toggle_chams_item() {
        bItem = !bItem;
    }

    bool get_chams_projectile() {
        return bProjectile;
    }
    void toggle_chams_projectile() {
        bProjectile = !bProjectile;
    }
};

class wallhacks {
    bool bEnabled = true;
    bool bPlayer = false;
    bool bItem = true;
    bool bProjectile = false;
    bool bOther = false;
public:
    bool* pEnabled = &bEnabled;
    bool* pPlayer = &bPlayer;
    bool* pItem = &bItem;
    bool* pProjectile = &bProjectile;
    wallhacks() {}
    ~wallhacks() = default;

    bool get_enabled() {
        return bEnabled;
    }
    void toggle_wallhack() {
        bEnabled = !bEnabled;
    }

    bool get_wall_player() {
        return bPlayer;
    }
    void toggle_wall_player() {
        bPlayer = !bPlayer;
    }

    bool get_wall_item() {
        return bItem;
    }
    void toggle_wall_item() {
        bItem = !bItem;
    }

    bool get_wall_projectile() {
        return bProjectile;
    }
    void toggle_wall_projectile() {
        bProjectile = !bProjectile;
    }

    bool get_wall_other() {
        return bOther;
    }
    void toggle_wall_other() {
        bOther = !bOther;
    }


};

class visuals : public features {
    ModelTeamEnum getTeam(const char* modelPath);
    ModelTypeEnum getType(const char* modelPath);
    void(__stdcall* pGlBlendFunc)(unsigned int, unsigned int) = nullptr;
    void(__stdcall* pGlPushMatrix)() = nullptr;
    void(__stdcall* pGlColor4f)(float, float, float, float) = nullptr;
    void(__stdcall* pGlPopMatrix)() = nullptr;
    void(__stdcall* pGlDisableClientState)(unsigned int) = nullptr;
    void(__stdcall* pGlLineWidth)(float) = nullptr;
    void(__stdcall* pGlPolygonMode)(unsigned int, unsigned int) = nullptr;
    void(__stdcall* pGlPushAttrib)(unsigned int) = nullptr;
    void(__stdcall* pGlPopAttrib)() = nullptr;
    void(__stdcall* pGlEnable)(unsigned int) = nullptr;
    void(__stdcall* pGlDisable)(unsigned int) = nullptr;
public:
    wallhacks wallhack;
    chams cham;
    BoundModel boundModel = { nullptr, OTHER, NO_TEAM };

    visuals(HMODULE opengl_handle) {
        if (pGlEnable == nullptr) {
            pGlEnable = reinterpret_cast<void(__stdcall*)(unsigned int)>(GetProcAddress(opengl_handle, "glEnable"));
        }
        if (pGlDisable == nullptr) {
            pGlDisable = reinterpret_cast<void(__stdcall*)(unsigned int)>(GetProcAddress(opengl_handle, "glDisable"));
        }
        // Add the other functions here
        if (pGlBlendFunc == nullptr) {
            pGlBlendFunc = reinterpret_cast<void(__stdcall*)(unsigned int, unsigned int)>(GetProcAddress(opengl_handle, "glBlendFunc"));
        }
        if (pGlPushMatrix == nullptr) {
            pGlPushMatrix = reinterpret_cast<void(__stdcall*)()>(GetProcAddress(opengl_handle, "glPushMatrix"));
        }
        if (pGlColor4f == nullptr) {
            pGlColor4f = reinterpret_cast<void(__stdcall*)(float, float, float, float)>(GetProcAddress(opengl_handle, "glColor4f"));
        }
        if (pGlPopMatrix == nullptr) {
            pGlPopMatrix = reinterpret_cast<void(__stdcall*)()>(GetProcAddress(opengl_handle, "glPopMatrix"));
        }
        if (pGlDisableClientState == nullptr) {
            pGlDisableClientState = reinterpret_cast<void(__stdcall*)(unsigned int)>(GetProcAddress(opengl_handle, "glDisableClientState"));
        }
        if (pGlLineWidth == nullptr) {
            pGlLineWidth = reinterpret_cast<void(__stdcall*)(float)>(GetProcAddress(opengl_handle, "glLineWidth"));
        }
        if (pGlPolygonMode == nullptr) {
            pGlPolygonMode = reinterpret_cast<void(__stdcall*)(unsigned int, unsigned int)>(GetProcAddress(opengl_handle, "glPolygonMode"));
        }
        if (pGlPushAttrib == nullptr) {
            pGlPushAttrib = reinterpret_cast<void(__stdcall*)(unsigned int)>(GetProcAddress(opengl_handle, "glPushAttrib"));
        }
        if (pGlPopAttrib == nullptr) {
            pGlPopAttrib = reinterpret_cast<void(__stdcall*)()>(GetProcAddress(opengl_handle, "glPopAttrib"));
        }
    }
    ~visuals() {};

    void setCurrentmodel(const unsigned char* modelPath) {
        boundModel.modelPath = modelPath;
        boundModel.modelTeam = getTeam((const char*)modelPath);
        boundModel.modelType = getType((const char*)modelPath);
    }

    // Not sure if the parameters are passed correctly
    void draw_visuals(unsigned int& mode, int& count, unsigned int& type, const void*& indices, glDrawElements_t original);
    bool wallhack_should_draw();
    void draw_menu() override {
        ImGui::Text("Visuals");
        // Wallhack
        if (ImGui::CollapsingHeader("Wallhacks", ImGuiTreeNodeFlags_None)) {
            // Checkbox
            // TODO: Figure out why a static bool fixed all my problems lol
            // original issue: wallhacks would not inable, probably due to the checkbox being locked by wglswapbuffers
            ImGui::Checkbox("Enable Wallhacks", this->wallhack.pEnabled);
            ImGui::SetItemTooltip("Disables Depth Testing");
            if (this->wallhack.get_enabled()) {
                ImGui::BeginChild("Wallhack Settings", ImVec2(0, 0), true);
                ImGui::Checkbox("Players", this->wallhack.pPlayer); ImGui::SameLine();
                ImGui::SetItemTooltip("Draw players through walls");
                ImGui::Checkbox("Items", this->wallhack.pItem); ImGui::SameLine();
                ImGui::SetItemTooltip("Draw items through walls");
                ImGui::Checkbox("Projectiles", this->wallhack.pProjectile); ImGui::SameLine();
                ImGui::SetItemTooltip("Draw projectiles (rocket etc.) through walls");
                ImGui::EndChild();
            }
        }
        if (ImGui::CollapsingHeader("ESP", ImGuiTreeNodeFlags_None)) {
            // Checkbox
            ImGui::Checkbox("Chams", this->cham.pEnabled);
            ImGui::SetItemTooltip("Draw chams");
            if (this->cham.get_enabled()) {
                ImGui::BeginChild("Chams Settings", ImVec2(0, 0), true);
                ImGui::Checkbox("Players", this->cham.pPlayer);
                ImGui::SetItemTooltip("Draw player chams");
                if (this->cham.get_chams_player()) {
                    ImGui::ColorEdit4("Playercolor", this->cham.pPlayerColor);
                }
                ImGui::Checkbox("Items", this->cham.pItem);
                ImGui::SetItemTooltip("Draw item chams");
                if (this->cham.get_chams_item()) {
                    ImGui::ColorEdit4("Itemcolor", this->cham.pItemColor);
                }
                ImGui::Checkbox("Projectiles", this->cham.pProjectile);
                if (this->cham.get_chams_projectile()) {
                    ImGui::ColorEdit4("Projectile color", this->cham.pProjectileColor);
                }

                ImGui::EndChild();
            }
        }
    };
};