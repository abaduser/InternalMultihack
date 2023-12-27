#include "../pch.h"
#include "../toolbox.h"
#include "visuals.h"

// model definition
const char* PLAYER_MODEL_PATTERNS[] = { "head_default", "lower_default", "upper_default", "models/players" , NULL };
const char* ITEM_PATTERNS[] = {
    "models/weapons2", "models/weaphits/proxmine.tga", "models/powerups/",
    "models/flags/", "textures/effects/envmapgold2.tga", "textures/effects/envmapblue.tga",
    NULL
};
const char* PROJECTILES[] = { "models/ammo" };
const char* BLUE_TEAM_PATTERNS[] = { "/sport_blue.tga", "/upper_blue.tga", "/blue.tga", "models/players/mynx/blue_s.tga", NULL };
const char* RED_TEAM_PATTERNS[] = { "/sport_red.tga", "/upper_blue.tga", "/red.tga", "models/players/mynx/red_s.tga", NULL };
const char* BRIGHT_TEAM_PATTERNS[] = { "/bright.tga", "models/players/mynx/bright_s.tga", NULL };

ModelTeamEnum visuals::getTeam(const char* modelPath) {
    for (const char** pattern = BLUE_TEAM_PATTERNS; *pattern; ++pattern) {
        if (stristr(modelPath, *pattern)) return BLUE_TEAM;
    }
    for (const char** pattern = RED_TEAM_PATTERNS; *pattern; ++pattern) {
        if (stristr(modelPath, *pattern)) return RED_TEAM;
    }
    for (const char** pattern = BRIGHT_TEAM_PATTERNS; *pattern; ++pattern) {
        if (stristr(modelPath, *pattern)) return BRIGHT_TEAM;
    }
    return NO_TEAM;
}

ModelTypeEnum visuals::getType(const char* modelPath) {
    if (!modelPath) {
        return OTHER;
    }
    for (const char** pattern = PLAYER_MODEL_PATTERNS; *pattern; ++pattern) {
        if (stristr(modelPath, *pattern)) return PLAYER;
    }
    for (const char** pattern = ITEM_PATTERNS; *pattern; ++pattern) {
        if (stristr(modelPath, *pattern)) return ITEM;
    }
    for (const char** patern = PROJECTILES; *patern; ++patern) {
        if (stristr(modelPath, *patern)) return PROJECTILE;
    }
    return OTHER;
}

void visuals::draw_visuals(unsigned int &mode, int &count, unsigned int &type, const void* &indices, glDrawElements_t original) {
    if (boundModel.modelPath == nullptr) {
        return;
    }
    switch (boundModel.modelType) {
    case PLAYER:
        if (cham.get_enabled() && cham.get_chams_player()) {
            float* playerColor = cham.get_playercolor();
            // Player Cham Setup
            (*pGlPushAttrib)(GL_ALL_ATTRIB_BITS); // Pushes the current attribute stack for GL onto the stack
            (*pGlPushMatrix)(); // Pushes the current matrix onto the stack
            (*pGlEnable)(GL_COLOR_MATERIAL); // Enables coloring of material
            (*pGlDisableClientState)(GL_COLOR_ARRAY); // Disables arrays of colors
            // Draw Wireframe
            if (wallhack.get_wall_player()) {
                (*pGlDisable)(GL_DEPTH_TEST); // Disables depth testing
                (*pGlEnable)(GL_TEXTURE_2D); // Disables 2D textures
                (*pGlDisable)(GL_LINE_SMOOTH); // Make it Cheaper, who cares.
                switch (boundModel.modelTeam) {
                case BLUE_TEAM:
                    (*pGlColor4f)(0.0f, 0.0f, 0.75f, 1.0f);
                    break;
                case RED_TEAM:
                    (*pGlColor4f)(0.75f, 0.0f, 0.0f, 1.0f);
                    break;
                case BRIGHT_TEAM:

                    (*pGlColor4f)(playerColor[0], playerColor[1], playerColor[2], playerColor[3]);
                    break;
                case NO_TEAM:
                    (*pGlColor4f)(0.5f, 0.5f, 0.0f, 1.0f);
                    break;
                }
                (*pGlLineWidth)(2.0); // Sets line width to 2.0
                (*pGlPolygonMode)(GL_FRONT_AND_BACK, GL_LINE); // Sets polygon drawing mode to outline
                (*pGlBlendFunc)(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Sets blending function
                (*pGlEnable)(GL_BLEND);  // Disable blending
                original(mode, count, type, indices); // Draws
            }
            // Draw Cham Color by Team
            switch (boundModel.modelTeam) {
            case BLUE_TEAM:
                (*pGlColor4f)(0.0f, 0.0f, 1.0f, 1.0f);
                break;
            case RED_TEAM:
                (*pGlColor4f)(1.0f, 0.0f, 0.0f, 1.0f);
                break;
            case BRIGHT_TEAM:
                (*pGlColor4f)(playerColor[0], playerColor[1], playerColor[2], playerColor[3]);
                break;
            case NO_TEAM:
                (*pGlColor4f)(playerColor[0], playerColor[1], playerColor[2], playerColor[3]);
                break;
            }
            (*pGlEnable)(GL_DEPTH_TEST); // Disables depth testing
            (*pGlDisable)(GL_TEXTURE_2D); // Disables 2D textures
            (*pGlEnable)(GL_BLEND); // Enables blending
            (*pGlBlendFunc)(GL_ONE, GL_ZERO); // Sets blending function
            (*pGlEnable)(GL_LINE_SMOOTH); // Enables anti-aliasing for lines
            (*pGlPolygonMode)(GL_FRONT_AND_BACK, GL_FILL); // Sets polygon drawing mode to outline
            original(mode, count, type, indices); // Draws
            // Pop / Cleanup

            (*pGlEnable)(GL_LINE_SMOOTH); // Re-enables anti-aliasing for lines
            (*pGlEnable)(GL_TEXTURE_2D); // Re-enables 2D textures
            (*pGlPolygonMode)(GL_FRONT_AND_BACK, GL_FILL); // Sets polygon drawing mode to outline
            (*pGlEnable)(GL_COLOR_MATERIAL); // Re-enables coloring of material 
            (*pGlDisableClientState)(GL_COLOR_ARRAY); // Disables arrays of colors
            (*pGlDisable)(GL_BLEND); // Disables blending

            (*pGlPopMatrix)(); // Pops the matrix off the stack, restoring previous matrix
            (*pGlPopAttrib)(); // Pops the current attribute stack for GL off the stack    
        }
        else {
            if (wallhack.get_enabled() && wallhack.get_wall_player()) {
                (*pGlDisable)(GL_DEPTH_TEST); // Makes the object visible through walls
            }
            else {
                (*pGlEnable)(GL_DEPTH_TEST);
            }
        }
        break;
    case ITEM:
        if (cham.get_enabled() && cham.get_chams_item()) {
            float* itemColor = cham.get_itemcolor();
            (*pGlPushAttrib)(GL_ALL_ATTRIB_BITS);
            (*pGlPushMatrix)();
            (*pGlEnable)(GL_COLOR_MATERIAL);
            (*pGlDisableClientState)(GL_COLOR_ARRAY);
            // Enable blending
            (*pGlEnable)(GL_BLEND);
            // choose color
            (*pGlColor4f)(itemColor[0], itemColor[1], itemColor[2], itemColor[3]);
            if (wallhack.get_wall_item()) {
                (*pGlDisable)(GL_DEPTH_TEST); // Makes the object visible through walls
            }
            // Disable textures
            (*pGlDisable)(GL_TEXTURE_2D);
            // Set blending function
            (*pGlBlendFunc)(GL_ONE_MINUS_DST_COLOR, GL_ONE);
            // Disable line smoothing, we don't need an outline
            (*pGlDisable)(GL_LINE_SMOOTH);
            // Set the polygon mode to fill the object
            (*pGlPolygonMode)(GL_FRONT_AND_BACK, GL_FILL);
            // Draw the elements (this would be your item)
            original(mode, count, type, indices);
            // Re-enable depth test
            (*pGlEnable)(GL_DEPTH_TEST);
            // Re-enable textures
            (*pGlEnable)(GL_TEXTURE_2D);
            (*pGlDisable)(GL_BLEND);
            // Pop the current matrix off the stack
            (*pGlPopMatrix)();
            (*pGlPopAttrib)();
        }
        else {
            if (wallhack.get_enabled() && wallhack.get_wall_item()) {
                (*pGlDisable)(GL_DEPTH_TEST); // Makes the object visible through walls
            }
            else {
                (*pGlEnable)(GL_DEPTH_TEST);
            }
        }
        break;
    case PROJECTILE:
        if (cham.get_enabled() && cham.get_chams_projectile()) {
            float* projectileColor = cham.get_projectilecolor();
            (*pGlPushAttrib)(GL_ALL_ATTRIB_BITS);
            (*pGlPushMatrix)();
            (*pGlEnable)(GL_COLOR_MATERIAL);
            (*pGlDisableClientState)(GL_COLOR_ARRAY);
            (*pGlEnable)(GL_BLEND);
            (*pGlColor4f)(projectileColor[0], projectileColor[1], projectileColor[2], projectileColor[3]);

            if (wallhack.get_wall_projectile()) {
                (*pGlDisable)(GL_DEPTH_TEST); // Makes the object visible through walls
            }

            // Disable textures
            (*pGlDisable)(GL_TEXTURE_2D);

            // Set blending function
            (*pGlBlendFunc)(GL_ONE_MINUS_DST_COLOR, GL_ONE);

            // Disable line smoothing, we don't need an outline
            (*pGlDisable)(GL_LINE_SMOOTH);

            // Set the polygon mode to fill the object
            (*pGlPolygonMode)(GL_FRONT_AND_BACK, GL_FILL);

            // Draw the elements (this would be your item)
            original(mode, count, type, indices);

            // Re-enable depth test
            (*pGlEnable)(GL_DEPTH_TEST);

            // Re-enable textures
            (*pGlEnable)(GL_TEXTURE_2D);

            (*pGlDisable)(GL_BLEND);

            // Pop the current matrix off the stack
            (*pGlPopMatrix)();
            (*pGlPopAttrib)();
        }
        else {
            if (wallhack.get_enabled() && wallhack.get_wall_projectile()) {
                (*pGlDisable)(GL_DEPTH_TEST); // Makes the object visible through walls
            }
            else {
                (*pGlEnable)(GL_DEPTH_TEST);
            }
        }
    }
}

bool visuals::wallhack_should_draw() {
    if (wallhack.get_enabled()) {
        switch (boundModel.modelType) {
        case PLAYER:
            if (wallhack.get_wall_player()) {
                return true;
            }
            break;
        case ITEM:
            if (wallhack.get_wall_item()) {
                return true;
            }
            // Logic
            break;
        case PROJECTILE:
            if (wallhack.get_wall_projectile()) {
                return true;
            }
            // Do nothing
            break;
        case OTHER:
            if (wallhack.get_wall_other()) {
                return true;
            }
            // Do nothing
            break;
        default:
            // By default, even if wallhacks are enabled, if we didn't find a model type, don't draw
            return false;
        }
    }
    return false;
}