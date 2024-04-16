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

		buildInternal(state.Hand.thumb.distal.curl, state.Hand.thumb.proximal.curl, "Thumb");
		buildInternal(state.Hand.index.distal.curl, state.Hand.index.proximal.curl, "Index");
		buildInternal(state.Hand.middle.distal.curl, state.Hand.middle.proximal.curl, "Middle");
		buildInternal(state.Hand.ring.distal.curl, state.Hand.ring.proximal.curl, "Ring");
		buildInternal(state.Hand.pinky.distal.curl, state.Hand.pinky.proximal.curl, "Pinky");

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

#if _DEBUG 
	ImGui::ShowDemoWindow();
#endif

	bool skeletalHandsActive = true;
	const ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

	ImGui::Begin("VRC Skeletal Hands", &skeletalHandsActive, 1);

	ImGui::Text("Binary[1]: %.1f", state.bool1);
	ImGui::Spacing();
	ImGui::Text("Binary[2]: %.1f", state.bool2);
	ImGui::Spacing();
	ImGui::Text("Binary[3]: %.1f", state.bool4);
	ImGui::Spacing();
	ImGui::Text("Binary[4]: %.1f", state.bool8);
	ImGui::Spacing();

	drawController(state.left, true);
	drawController(state.right, false);

	ImGui::End();
}