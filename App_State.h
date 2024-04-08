#pragma once
#include <cinttypes>
#include <memory>
#include <hekky-osc.hpp>
#include <openvr.h>


struct controller_State {
	uint32_t deviceIndex;
	bool is_connected;

	vr::VRSkeletalSummaryData_t skeletonSumary;
	vr::InputSkeletalActionData_t skeletalData;
	vr::InputAnalogActionData_t force;

	struct {
		float thumb, index, middle, ring, pinky;
	}Curl;

	struct {
		float thumb, index, middle, ring, pinky;
	}Splay;

	struct {
		vr::VRInputValueHandle_t inputValueHandle;

		vr::VRActionHandle_t skeletal;
		vr::VRActionHandle_t a;
		vr::VRActionHandle_t b;
		vr::VRActionHandle_t joystick;
		vr::VRActionHandle_t touchpad;
		vr::VRActionHandle_t force;

	}inputHandles;
};

struct Curl{
	float thumb, index, middle, ring, pinky;
};

struct App_State
{
public:
	App_State();
	vr::VRActionSetHandle_t actionSetHandle;
	vr::VRActiveActionSet_t actionSet;

	std::unique_ptr<hekky::osc::UdpSender> oscSender;
	controller_State left;
	controller_State right;
};

