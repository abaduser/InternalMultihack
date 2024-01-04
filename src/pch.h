// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <map>
#include "GL/glew.h"
#include "../Include/QL_Unofficial_SDK/qcommon/q_platform.h"
#include "../Include/QL_Unofficial_SDK/qcommon/q_shared.h"
#include "../Include/QL_Unofficial_SDK/cgame/cg_public.h"
#include "../Include/QL_Unofficial_SDK/cgame/cg_local.h"
#include "../Include/QL_Unofficial_SDK/renderercommon/tr_types.h"

#endif //PCH_H
