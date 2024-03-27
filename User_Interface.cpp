#include "User_Interface.h"

void SetupImgui() {

}
void CleanupImgui() {

}
void DrawUi(bool isOverlay, App_State& state){
	ImGui::ShowDemoWindow();

	const ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

	ImGui::Begin("EpicOverlay");

	ImGui::Text("Sigma Male Gamer");

	ImGui::End();
}