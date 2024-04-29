#include <iostream>
#include <string>
#include <filesystem>
#include <math.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <hekky-osc.hpp>
#include <openvr.h>
#include "Overlay_app.h"
#include "Maths.h"

// Props for the overlay
#define OPENVR_APPLICATION_KEY "Dyslex1k.VRCSkeletalHands"
static vr::VROverlayHandle_t s_overlayMainHandle;
static vr::VROverlayHandle_t s_overlayThumbnailHandle;

bool loadInputs(App_State& state);

static std::string getExecutableDir() {
    //Get path to exe (includes the exe file name)
    //CHAR executablePath[MAX_PATH];
    std::string executablePath(MAX_PATH, '\0');
    GetModuleFileNameA(nullptr, &executablePath[0], MAX_PATH);

    //Get directory path
    std::string executableDir;
    const size_t lastSlashIndex = executablePath.rfind('\\');
    if (std::string::npos != lastSlashIndex) {
        executableDir = executablePath.substr(0, lastSlashIndex);
    }
    else {
        std::cerr << "Invalid Executable Path Aborting\n";
        return "";
    }
    return executableDir;
}

bool InitVR(App_State& state)
{
    auto initError = vr::VRInitError_None;
    vr::VR_Init(&initError, vr::VRApplication_Other);
    if (initError != vr::VRInitError_None)
    {
        auto error = vr::VR_GetVRInitErrorAsEnglishDescription(initError);
        std::cerr << std::string("OpenVR error:" + std::string(error));
        return false;
    }

    if (!vr::VR_IsInterfaceVersionValid(vr::IVRSystem_Version))
    {
        std::cerr << std::string("OpenVR error: Outdated IVRSystem_Version");
        return false;
    }
    else if (!vr::VR_IsInterfaceVersionValid(vr::IVRSettings_Version))
    {
        std::cerr << std::string("OpenVR error: Outdated IVRSettings_Version");
        return false;
    }
    else if (!vr::VR_IsInterfaceVersionValid(vr::IVROverlay_Version))
    {
        std::cerr << std::string("OpenVR error: Outdated IVROverlay_Version");
        return false;
    }

    return loadInputs(state);
}

bool loadInputs(App_State &state) {
    //Trys to open the Action manifest 
    std::string actionManifestPath = "action_manifest.json";

    actionManifestPath = getExecutableDir() + "\\" + actionManifestPath;

    auto error = vr::VRInput()->SetActionManifestPath(actionManifestPath.c_str());
    if (error != vr::EVRInputError::VRInputError_None)
    {
        std::cerr << error;
        return false;
    }

    //Asign Left Hand
    error = vr::VRInput()->GetActionHandle("/actions/main/in/LeftHand_Anim", &state.left.inputHandles.skeletal);
    if (error != vr::EVRInputError::VRInputError_None)
    {
        std::cerr << "Handle error.\n";
        return false;
    }

    error = vr::VRInput()->GetActionHandle("/actions/main/in/leftforce", &state.left.inputHandles.force);
    if (error != vr::EVRInputError::VRInputError_None)
    {
        std::cerr << "Handle error.\n";
        return false;
    }

    //Asign Right Hand
    error = vr::VRInput()->GetActionHandle("/actions/main/in/RightHand_Anim", &state.right.inputHandles.skeletal);
    if (error != vr::EVRInputError::VRInputError_None)
    {
        std::cerr << "Handle error.\n";
        return false;
    }

    error = vr::VRInput()->GetActionHandle("/actions/main/in/rightforce", &state.right.inputHandles.force);
    if (error != vr::EVRInputError::VRInputError_None)
    {
        std::cerr << "Handle error.\n";
        return false;
    }

    error = vr::VRInput()->GetActionSetHandle("/actions/main", &state.actionSetHandle);
    if (error != vr::EVRInputError::VRInputError_None)
    {
        std::cerr << "Handle error.\n";
        return false;
    }

    state.actionSet.ulActionSet = state.actionSetHandle;
    state.actionSet.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
    state.actionSet.nPriority = 0;

    return true;
}

void TryCreateVrOverlay() {
    if (s_overlayMainHandle || !vr::VROverlay()) {
        return;
    }

    vr::VROverlayError err = vr::VROverlay()->CreateDashboardOverlay(
        OPENVR_APPLICATION_KEY, "VRCSkeletalHands",
        &s_overlayMainHandle, &s_overlayThumbnailHandle
    );

    if (err == vr::VROverlayError_KeyInUse) {
        throw std::runtime_error("Another instance of VRCSkeletalHands is already running!");
    }
    else if (err != vr::VROverlayError_None) {
        throw std::runtime_error("Failed to create SteamVR Overlay with error " + std::string(vr::VROverlay()->GetOverlayErrorNameFromEnum(err)));
    }

    vr::VROverlay()->SetOverlayWidthInMeters(s_overlayMainHandle, 3.0f);
    vr::VROverlay()->SetOverlayInputMethod(s_overlayMainHandle, vr::VROverlayInputMethod_Mouse);
    vr::VROverlay()->SetOverlayFlag(s_overlayMainHandle, vr::VROverlayFlags_SendVRDiscreteScrollEvents, true);

    // @TODO: Icon


    //setup application manifest
    vr::VRApplications()->AddApplicationManifest((getExecutableDir() + "\\manifest.vrmanifest").c_str(), false);
    vr::VRApplications()->SetApplicationAutoLaunch(OPENVR_APPLICATION_KEY, true);

}


enum class BindingState {
    Okay,
    UpdateTriggered,
    Reloaded,
    Broken
};


void updateSteamVrState(App_State &state) {
    
    static BindingState bindingState = BindingState::Okay;

    vr::VREvent_t pEvent = {};
    while (vr::VRSystem()->PollNextEvent(&pEvent, sizeof(pEvent))) {
        switch (pEvent.eventType) {

        case vr::EVREventType::VREvent_Input_BindingLoadFailed:
            std::cout << "Binding load Failed\n";
            break;

        case vr::EVREventType::VREvent_ActionBindingReloaded:
            if (bindingState == BindingState::UpdateTriggered) {
                bindingState = BindingState::Broken;
            }
            std::cout << "Action Binding Reloaded\n";
            break;

        case vr::EVREventType::VREvent_Input_BindingsUpdated:
            if (bindingState == BindingState::Okay) {
                bindingState = BindingState::UpdateTriggered;
            }
            std::cout << "Binding Updated\n";
            break;

        case vr::EVREventType::VREvent_Input_BindingLoadSuccessful:
            std::cout << "Binding load Successful\n";
            break;

        default:
            break;
        }
    }

    if (bindingState == BindingState::Broken) {
        //If retricted bindings need reloaded
        std::cout << "Bindings Broken Attempting Fix\n";
        if (loadInputs(state)) {
            bindingState = BindingState::Okay;
        }
    }
}

void TryFindController(App_State& state) {

    uint32_t trackerIdLeft = vr::k_unTrackedDeviceIndexInvalid;
    uint32_t trackerIdRight = vr::k_unTrackedDeviceIndexInvalid;

    for (int i = 1; i < vr::k_unMaxTrackedDeviceCount; i++) {
        if (vr::VRSystem()->IsTrackedDeviceConnected(i)) {
            //feches the device property
            uint32_t deviceClass = vr::VRSystem()->GetInt32TrackedDeviceProperty(i, vr::Prop_DeviceClass_Int32);

            //checks device property and checks if controller
            if (deviceClass == vr::TrackedDeviceClass_Controller) {
                vr::ETrackedControllerRole controllerHand = static_cast<vr::ETrackedControllerRole>(vr::VRSystem()->GetInt32TrackedDeviceProperty(i, vr::Prop_ControllerRoleHint_Int32));

                //Check for left or right
                if (controllerHand == vr::TrackedControllerRole_RightHand) {
                    //found left hand
                    if (trackerIdRight == vr::k_unTrackedDeviceIndexInvalid) {
                        trackerIdRight = i;
                    }
                    if (trackerIdLeft != vr::k_unTrackedDeviceIndexInvalid) {
                        break;
                    }
                }
                else if (controllerHand == vr::TrackedControllerRole_LeftHand) {
                    // Found left tracker
                    if (trackerIdLeft == vr::k_unTrackedDeviceIndexInvalid) {
                        trackerIdLeft = i;
                    }
                    if (trackerIdRight != vr::k_unTrackedDeviceIndexInvalid) {
                        break;
                    }
                }
            }
        }
    }
    //end of loop
    state.left.deviceIndex = trackerIdLeft;
    state.right.deviceIndex = trackerIdRight;
}

void updateControllerState(controller_State& state, App_State& appState) {
    if (state.deviceIndex == vr::k_unTrackedDeviceIndexInvalid) {
        return;
    }
    
    vr::VRInput()->UpdateActionState(&appState.actionSet, sizeof(appState.actionSet), 1);

    //Get forceValue
    if (state.inputHandles.force != vr::k_ulInvalidInputValueHandle) {
        vr::VRInput()->GetAnalogActionData(state.inputHandles.force, &state.force, sizeof(state.force), vr::k_ulInvalidInputValueHandle);
    }
    //Get Skeletal DataInfo
    vr::VRInput()->GetSkeletalActionData(state.inputHandles.skeletal, &state.skeletalData, 1);

    //Get Skeletal summary for splay
    if (state.inputHandles.skeletal != vr::k_ulInvalidInputValueHandle) {
        vr::VRInput()->GetSkeletalSummaryData(state.inputHandles.skeletal, vr::VRSummaryType_FromDevice, &state.skeletonSumary);
    }

    //GetBoneData saves to ControllerState.boneTransforms
    vr::VRInput()->GetSkeletalBoneData(state.inputHandles.skeletal,vr::EVRSkeletalTransformSpace::VRSkeletalTransformSpace_Model, vr::EVRSkeletalMotionRange::VRSkeletalMotionRange_WithoutController, state.boneTransforms, SKELETAL_BONE_COUNT);
}


//Thank you so much Lux/Hekky/Hyblocker (They did the maths) 
/*@TODO Add in coming updates
double getBoneCurl(HandSkeletonBone boneA, HandSkeletonBone boneB, controller_State &state){
    static const vr::HmdVector3d_t UNIT_FORWARD = { {0.0,0.0,1.0} };
    static const vr::HmdVector3d_t CLOSE_AXIS = pow(vr::HmdVector3d_t { -0.00620213, -0.66979522, 0.74251997 }, 1.0);

    auto boneAForward = normalize(UNIT_FORWARD * state.boneTransforms[boneA].orientation);
    auto boneBForward = normalize((UNIT_FORWARD * state.boneTransforms[boneB].orientation) * CLOSE_AXIS);

    //@TODO This aproximates curvature not curl!
    //Fix . Want to 0 out axis that are not curl Same goes for splay
    auto dotProd = dot(boneAForward, boneBForward);
    double angle = radToDeg(acos(dotProd));
    return angle;
}

float getBoneSplay(HandSkeletonBone bone, controller_State &state) {
    return 1.0f;
}

//Function that transforms the bone rotations into curl values that are used in Vrchat
void transformBoneData(controller_State &state) {
    //Save data to floats

    //Calculate thumb (Humans are annoying)
    //state.Hand.thumb.distal.curl = getBoneCurl();
    //state.Hand.thumb.proximal.curl = getBoneCurl();
    //state.Hand.index.proximal.splay = getBoneSplay();


    //Calculate fingers from index -> pinky

    //Index
    ///state.Hand.index.distal.curl = getBoneCurl(HandSkeletonBone::eBone_IndexFinger3, HandSkeletonBone::eBone_IndexFinger1, state);
    ///state.Hand.index.proximal.curl = getBoneCurl(HandSkeletonBone::eBone_IndexFinger3, HandSkeletonBone::eBone_Wrist, state);
    ///state.Hand.index.proximal.splay = getBoneSplay(HandSkeletonBone::eBone_IndexFinger1, state);
    
    //Middle
    ///state.Hand.middle.distal.curl = getBoneCurl(HandSkeletonBone::eBone_MiddleFinger3, HandSkeletonBone::eBone_MiddleFinger1, state);
    ///state.Hand.middle.proximal.curl = getBoneCurl(HandSkeletonBone::eBone_MiddleFinger3, HandSkeletonBone::eBone_Wrist, state);
    ///state.Hand.middle.proximal.splay = getBoneSplay(HandSkeletonBone::eBone_MiddleFinger1, state);
   
    //Ring
    ///state.Hand.ring.distal.curl = getBoneCurl(HandSkeletonBone::eBone_RingFinger3, HandSkeletonBone::eBone_RingFinger1, state);
    ///state.Hand.ring.proximal.curl = getBoneCurl(HandSkeletonBone::eBone_RingFinger3, HandSkeletonBone::eBone_Wrist, state);
    ///state.Hand.ring.proximal.splay = getBoneSplay(HandSkeletonBone::eBone_RingFinger1, state);
    
    //Pinky
    ///state.Hand.pinky.distal.curl = getBoneCurl(HandSkeletonBone::eBone_PinkyFinger3, HandSkeletonBone::eBone_PinkyFinger1, state);
    ///state.Hand.pinky.proximal.curl = getBoneCurl(HandSkeletonBone::eBone_PinkyFinger3, HandSkeletonBone::eBone_Wrist, state);
    ///state.Hand.pinky.proximal.splay = getBoneSplay(HandSkeletonBone::eBone_PinkyFinger1, state);
  
    double test = getBoneCurl(HandSkeletonBone::eBone_IndexFinger0, HandSkeletonBone::eBone_IndexFinger1, state);
    //state.fingerRotationB = getBoneCurl(HandSkeletonBone::eBone_MiddleFinger3, HandSkeletonBone::eBone_MiddleFinger0, state);
    //state.fingerRotationC = getBoneCurl(HandSkeletonBone::eBone_RingFinger3, HandSkeletonBone::eBone_RingFinger0, state);
    //state.fingerRotationD = getBoneCurl(HandSkeletonBone::eBone_PinkyFinger3, HandSkeletonBone::eBone_PinkyFinger0, state);
} */

void transformData(controller_State &state) {
    //@TODO Set all hand values Distal & Proximal Curl & Splay
    //transformBoneData(state);


    //Old method replace in future updates
    state.Hand.thumb.distal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Thumb];
    state.Hand.thumb.proximal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Thumb];
    state.Hand.thumb.proximal.splay = state.skeletonSumary.flFingerSplay[vr::VRFingerSplay_Thumb_Index];

    state.Hand.index.distal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Index];
    state.Hand.index.proximal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Index];
    state.Hand.index.proximal.splay = state.skeletonSumary.flFingerSplay[vr::VRFingerSplay_Index_Middle];

    state.Hand.middle.distal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Middle];
    state.Hand.middle.proximal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Middle];

    state.Hand.ring.distal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Ring];
    state.Hand.ring.proximal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Ring];
    state.Hand.ring.proximal.splay = state.skeletonSumary.flFingerSplay[vr::VRFingerSplay_Middle_Ring];

    state.Hand.pinky.distal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Pinky];
    state.Hand.pinky.proximal.curl = state.skeletonSumary.flFingerCurl[vr::VRFinger_Pinky];
    state.Hand.pinky.proximal.splay = state.skeletonSumary.flFingerSplay[vr::VRFingerSplay_Ring_Pinky];
}

void compressRemote(App_State& state, float rawValue) {
    //int n = lround(remap(rawValue, 0.0, 1.0, 0, 15));
    int n = lround(rawValue * 15.0);

    //Convert claped values to bools that represent bianary
    int binaryNum[4] = {};
    int i = 0;

    binaryNum[0] = (n & (1 << 0)) >> 0;
    binaryNum[1] = (n & (1 << 1)) >> 1;
    binaryNum[2] = (n & (1 << 2)) >> 2;
    binaryNum[3] = (n & (1 << 3)) >> 3;

    //Store number in bianary array
    state.bool1 = binaryNum[0];
    state.bool2 = binaryNum[1];
    state.bool4 = binaryNum[2];
    state.bool8 = binaryNum[3];
}

void pushRemote(App_State& state, const std::string& address) {
    auto bool1 = hekky::osc::OscMessage(("/avatar/parameters/SH/Interlace/bool" + address + "1")).PushBool(state.bool1);
    state.oscSender->Send(bool1);

    auto bool2 = hekky::osc::OscMessage(("/avatar/parameters/SH/Interlace/bool" + address + "2")).PushBool(state.bool2);
    state.oscSender->Send(bool2);

    auto bool4 = hekky::osc::OscMessage(("/avatar/parameters/SH/Interlace/bool" + address + "4")).PushBool(state.bool4);
    state.oscSender->Send(bool4);

    auto bool8 = hekky::osc::OscMessage(("/avatar/parameters/SH/Interlace/bool" + address + "8")).PushBool(state.bool8);
    state.oscSender->Send(bool8);
}

//Address needs to be boneType/Finger
void pushData(App_State& state, controller_State& controller, float raw, const std::string& address, const std::string& hand, bool sendRemote) {

    auto localData = hekky::osc::OscMessage(("/avatar/parameters/SH/Local/" + hand+ address)).PushFloat(raw);
    state.oscSender->Send(localData);

    if (sendRemote) {
        compressRemote(state, raw);
        pushRemote(state, address);
    }
}

void sendOSCData(App_State& state, bool is_Left, bool is_Right) {
    //@TODO Send data to the send parameters (Raw and Compressed) NOTE:Needs to be interlaced

    auto interlaceLeft = hekky::osc::OscMessage("/avatar/parameters/SH/Control/Interlace/isLeft");
    interlaceLeft.PushBool(is_Left);
    state.oscSender->Send(interlaceLeft);

    auto interlaceRight = hekky::osc::OscMessage("/avatar/parameters/SH/Control/Interlace/isRight");
    interlaceRight.PushBool(is_Right);
    state.oscSender->Send(interlaceRight);

    //Left Hand data send DISTAL
    pushData(state, state.left, state.left.Hand.thumb.distal.curl, "/Distal/Thumb", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.index.distal.curl, "/Distal/Index", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.middle.distal.curl, "/Distal/Middle", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.ring.distal.curl, "/Distal/Ring", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.pinky.distal.curl, "/Distal/Pinky", "Left", is_Left);

    //PROXIMAL LEFT
    pushData(state, state.left, state.left.Hand.thumb.proximal.curl, "/Proximal/Thumb", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.index.proximal.curl, "/Proximal/Index", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.middle.proximal.curl, "/Proximal/Middle", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.ring.proximal.curl, "/Proximal/Ring", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.pinky.proximal.curl, "/Proximal/Pinky", "Left", is_Left);

    //SPLAY LEFT
    pushData(state, state.left, state.left.Hand.thumb.proximal.splay, "/Splay/Thumb", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.index.proximal.splay, "/Splay/Index", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.middle.proximal.splay, "/Splay/Middle", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.ring.proximal.splay, "/Splay/Ring", "Left", is_Left);
    pushData(state, state.left, state.left.Hand.pinky.proximal.splay, "/Splay/Pinky", "Left", is_Left);


    //Right Hand
    //DISTAL
    pushData(state, state.right, state.right.Hand.thumb.distal.curl, "/Distal/Thumb", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.index.distal.curl, "/Distal/Index", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.middle.distal.curl, "/Distal/Middle", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.ring.distal.curl, "/Distal/Ring", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.pinky.distal.curl, "/Distal/Pinky", "Right", !is_Left);
    
    //PROXIMAL RIGHT
    pushData(state, state.right, state.right.Hand.thumb.proximal.curl, "/Proximal/Thumb", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.index.proximal.curl, "/Proximal/Index", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.middle.proximal.curl, "/Proximal/Middle", "Right",!is_Left);
    pushData(state, state.right, state.right.Hand.ring.proximal.curl, "/Proximal/Ring", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.pinky.proximal.curl, "/Proximal/Pinky", "Right", !is_Left);

    //rightY RIGHT
    pushData(state, state.right, state.right.Hand.thumb.proximal.splay, "/Splay/Thumb", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.index.proximal.splay, "/Splay/Index", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.middle.proximal.splay, "/Splay/Middle", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.ring.proximal.splay, "/Splay/Ring", "Right", !is_Left);
    pushData(state, state.right, state.right.Hand.pinky.proximal.splay, "/Splay/Pinky", "Right", !is_Left);
}

#ifdef _DEBUG
int main() {
#else
int APIENTRY wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow){
#endif
    App_State state;

    InitVR(state);


	if (EpicOverlay::Overlay::StartWindow()) {
		bool doExectute = true;

        //Here so that the animator can function without any mojor latency
        bool interlaceLeft = true;
        bool interlaceRight = false;

		while (doExectute) {
            TryCreateVrOverlay();
            updateSteamVrState(state);
            TryFindController(state);

            //Update controller states for openVr
            updateControllerState(state.left, state);
            updateControllerState(state.right, state);

            //Saves hand trackingData
            transformData(state.left);
            transformData(state.right);
            sendOSCData(state, interlaceLeft, interlaceRight);

			doExectute = EpicOverlay::Overlay::UpdateNativeWindow(s_overlayMainHandle, state);

            //Inverts interlace so other hand syncs next frame
            interlaceLeft = !interlaceLeft;
            interlaceRight = !interlaceRight;

            //Add sleep to slow refresh rate
            Sleep(8);
		}
		EpicOverlay::Overlay::DestroyWindow();
	}
}