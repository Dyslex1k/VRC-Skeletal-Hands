#pragma once
#include <imgui.h>

#include "App_State.h"

void SetupImgui();
void CleanupImgui();
void DrawUi(bool isOverlay, App_State& state);