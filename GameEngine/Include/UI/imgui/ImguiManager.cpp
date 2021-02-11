#include "ImguiManager.h"
#include "imgui.h"

CImguiManager::CImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

CImguiManager::~CImguiManager()
{
	ImGui::DestroyContext();
}
