#include "User_Interface.h"

void SetupImgui() {
	
}
void CleanupImgui() {

}

void buildInternal(float valueCurl, float valueSplay, std::string Finger) {
	std::string text = Finger + ": %.2f";
	ImGui::Text(text.c_str(), valueCurl);
	ImGui::ProgressBar(valueCurl);
	ImGui::TableNextColumn();
	ImGui::Text(text.c_str(), valueSplay);
	ImGui::ProgressBar(valueSplay);
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
}

void drawDataTable(controller_State state) {
	if (ImGui::BeginTable("Curl Table", 2)) {
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		buildInternal(state.Curl.thumb, state.Splay.thumb, "Thumb");
		buildInternal(state.Curl.index, state.Splay.index, "Index");
		buildInternal(state.Curl.middle, state.Splay.middle, "Middle");
		buildInternal(state.Curl.ring, state.Splay.ring, "Ring");
		buildInternal(state.Curl.pinky, state.Splay.pinky, "Pinky");

	}
	ImGui::EndTable();
}

void drawController(controller_State state, bool isLeft) {
	std::string hand = "";
	if (isLeft) {
		hand = "Left hand Connected";
	}
	else {
		hand = "Right hand Connected";
	}
	ImGui::Spacing();
	if (state.deviceIndex == vr::k_unTrackedDeviceIndexInvalid) {
		ImGui::Text("Not Connected");
	}
	else {
		ImGui::Text(hand.c_str());
		drawDataTable(state);
	}
}

void DrawUi(bool isOverlay, App_State& state){
	ImGui::ShowDemoWindow();
	bool skeletalHandsActive = true;
	const ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

	ImGui::Begin("VRC Skeletal Hands", &skeletalHandsActive, 1);

	drawController(state.left, true);
	drawController(state.right, false);

	ImGui::End();
}