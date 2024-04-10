#pragma once
#include <cinttypes>
#include <memory>
#include <hekky-osc.hpp>
#include <openvr.h>

#define SKELETAL_BONE_COUNT 31

//Bone Indexes for a Skeletal hand
enum HandSkeletonBone : vr::BoneIndex_t
{
	eBone_Root = 0,
	eBone_Wrist,
	eBone_Thumb0,
	eBone_Thumb1,
	eBone_Thumb2,
	eBone_Thumb3,
	eBone_IndexFinger0,
	eBone_IndexFinger1,
	eBone_IndexFinger2,
	eBone_IndexFinger3,
	eBone_IndexFinger4,
	eBone_MiddleFinger0,
	eBone_MiddleFinger1,
	eBone_MiddleFinger2,
	eBone_MiddleFinger3,
	eBone_MiddleFinger4,
	eBone_RingFinger0,
	eBone_RingFinger1,
	eBone_RingFinger2,
	eBone_RingFinger3,
	eBone_RingFinger4,
	eBone_PinkyFinger0,
	eBone_PinkyFinger1,
	eBone_PinkyFinger2,
	eBone_PinkyFinger3,
	eBone_PinkyFinger4,
	eBone_Aux_Thumb,
	eBone_Aux_IndexFinger,
	eBone_Aux_MiddleFinger,
	eBone_Aux_RingFinger,
	eBone_Aux_PinkyFinger,
	eBone_Count
};

struct controller_State {
	uint32_t deviceIndex;
	bool is_connected;

	vr::VRSkeletalSummaryData_t skeletonSumary;
	vr::InputSkeletalActionData_t skeletalData;
	vr::InputAnalogActionData_t force;
	vr::VRBoneTransform_t boneTransforms[SKELETAL_BONE_COUNT];

	struct {
		vr::VRInputValueHandle_t inputValueHandle;

		vr::VRActionHandle_t skeletal;
		vr::VRActionHandle_t force;

	}inputHandles;


	//Hand data
	struct Hands {
		struct FingerJointCurlable {
			float curl;
		};
		struct FingerJointSplayable {
			float splay, curl;
		};
		struct Finger {
			FingerJointSplayable proximal;
			FingerJointCurlable distal;
		};
		Finger thumb;
		Finger index;
		Finger middle;
		Finger ring;
		Finger pinky;
	}Hand;

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

