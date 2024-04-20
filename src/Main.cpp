#include <iostream>
#include <string>
#include <filesystem>

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

#include <shellapi.h>

// Props for the overlay
#define OPENVR_APPLICATION_KEY "Dyslex1k.VRCSkeletalHands"
static vr::VROverlayHandle_t s_overlayMainHandle;
static vr::VROverlayHandle_t s_overlayThumbnailHandle;

bool loadInputs(App_State& state);

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

    //Get path to exe (includes the exe file name)
    //CHAR executablePath[MAX_PATH];
    std::string executablePath(MAX_PATH, '\0');
    GetModuleFileNameA(nullptr, &executablePath[0], MAX_PATH);

    //Get directory path
    std::string executableDir;
    const size_t lastSlashIndex = executablePath.rfind('\\');
    if (std::string::npos != lastSlashIndex) {
        executableDir = executablePath.substr(0, lastSlashIndex);
        actionManifestPath = executableDir + "\\" + actionManifestPath;
    }
    else {
        std::cerr << "Invalid Executable Path Aborting\n";
        return false;
    }

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
        throw std::runtime_error("Another instance of EpicOverlay is already running!");
    }
    else if (err != vr::VROverlayError_None) {
        throw std::runtime_error("Failed to create SteamVR Overlay with error " + std::string(vr::VROverlay()->GetOverlayErrorNameFromEnum(err)));
    }

    vr::VROverlay()->SetOverlayWidthInMeters(s_overlayMainHandle, 3.0f);
    vr::VROverlay()->SetOverlayInputMethod(s_overlayMainHandle, vr::VROverlayInputMethod_Mouse);
    vr::VROverlay()->SetOverlayFlag(s_overlayMainHandle, vr::VROverlayFlags_SendVRDiscreteScrollEvents, true);

    // @TODO: Icon
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

    if (state.inputHandles.skeletal != vr::k_ulInvalidInputValueHandle) {
        vr::VRInput()->GetSkeletalSummaryData(state.inputHandles.skeletal, vr::VRSummaryType_FromDevice, &state.skeletonSumary);
    }
}

float calcFingerCurl(controller_State &state, vr::EVRFinger finger){
    if (state.skeletonSumary.flFingerCurl[finger] > 0.75) {
        return state.skeletonSumary.flFingerCurl[finger] + state.force.x;
    }
    return state.skeletonSumary.flFingerCurl[finger];
}

void transformData(controller_State &state) {
    state.Curl.thumb = calcFingerCurl(state, vr::VRFinger_Thumb);
    state.Curl.index = calcFingerCurl(state, vr::VRFinger_Index);
    state.Curl.middle = calcFingerCurl(state, vr::VRFinger_Middle);
    state.Curl.ring = calcFingerCurl(state, vr::VRFinger_Ring);
    state.Curl.pinky = calcFingerCurl(state, vr::VRFinger_Pinky);

    state.Splay.thumb = state.skeletonSumary.flFingerSplay[vr::VRFingerSplay_Thumb_Index];
    state.Splay.index = state.skeletonSumary.flFingerSplay[vr::VRFingerSplay_Index_Middle];
    state.Splay.ring = state.skeletonSumary.flFingerSplay[vr::VRFingerSplay_Middle_Ring];
    state.Splay.pinky = state.skeletonSumary.flFingerSplay[vr::VRFingerSplay_Ring_Pinky];

    //@TODO Change the splay to use propper splay values from the inbetween values
}

void sendOSCData(App_State &state) {

    //Hand Curl data send
    auto thumbLeftCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Thumb_Curl");
    thumbLeftCurl.PushFloat(state.left.Curl.thumb);
    state.oscSender->Send(thumbLeftCurl);

    auto indexLeftCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Index_Curl");
    indexLeftCurl.PushFloat(state.left.Curl.index);
    state.oscSender->Send(indexLeftCurl);

    auto middleLeftCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Middle_Curl");
    middleLeftCurl.PushFloat(state.left.Curl.middle);
    state.oscSender->Send(middleLeftCurl);

    auto ringLeftCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Ring_Curl");
    ringLeftCurl.PushFloat(state.left.Curl.ring);
    state.oscSender->Send(ringLeftCurl);

    auto pinkyLeftCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Pinky_Curl");
    pinkyLeftCurl.PushFloat(state.left.Curl.pinky);
    state.oscSender->Send(pinkyLeftCurl);

    //hand Splay Data
    auto thumbLeftSplay = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Thumb_Splay");
    thumbLeftSplay.PushFloat(state.left.Splay.thumb);
    state.oscSender->Send(thumbLeftSplay);

    auto indexLeftSplay = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Index_Splay");
    indexLeftSplay.PushFloat(state.left.Splay.index);
    state.oscSender->Send(indexLeftSplay);

    auto ringLeftSplay = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Ring_Splay");
    ringLeftSplay.PushFloat(state.left.Splay.ring);
    state.oscSender->Send(ringLeftSplay);

    auto pinkyLeftSplay = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Left/Pinky_Splay");
    pinkyLeftSplay.PushFloat(state.left.Splay.pinky);
    state.oscSender->Send(pinkyLeftSplay);


    //Right Hand
    auto thumbRightCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Thumb_Curl");
    thumbRightCurl.PushFloat(state.right.Curl.thumb);
    state.oscSender->Send(thumbRightCurl);

    auto indexRightCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Index_Curl");
    indexRightCurl.PushFloat(state.right.Curl.index);
    state.oscSender->Send(indexRightCurl);

    auto middleRightCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Middle_Curl");
    middleRightCurl.PushFloat(state.right.Curl.middle);
    state.oscSender->Send(middleRightCurl);

    auto ringRightCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Ring_Curl");
    ringRightCurl.PushFloat(state.right.Curl.ring);
    state.oscSender->Send(ringRightCurl);

    auto pinkyRightCurl = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Pinky_Curl");
    pinkyRightCurl.PushFloat(state.right.Curl.pinky);
    state.oscSender->Send(pinkyRightCurl);

    //hand Splay Data
    auto thumbRightSplay = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Thumb_Splay");
    thumbRightSplay.PushFloat(state.right.Splay.thumb);
    state.oscSender->Send(thumbRightSplay);

    auto indexRightSplay = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Index_Splay");
    indexRightSplay.PushFloat(state.right.Splay.index);
    state.oscSender->Send(indexRightSplay);

    auto ringRightSplay = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Ring_Splay");
    ringRightSplay.PushFloat(state.right.Splay.ring);
    state.oscSender->Send(ringRightSplay);

    auto pinkyRightSplay = hekky::osc::OscMessage("/avatar/parameters/SH/Tracking/Right/Pinky_Splay");
    pinkyRightSplay.PushFloat(state.right.Splay.pinky);
    state.oscSender->Send(pinkyRightSplay);
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
		while (doExectute) {
            TryCreateVrOverlay();
            updateSteamVrState(state);
            TryFindController(state);
            updateControllerState(state.left, state);
            updateControllerState(state.right, state);
            transformData(state.left);
            transformData(state.right);
            sendOSCData(state);

			doExectute = EpicOverlay::Overlay::UpdateNativeWindow(s_overlayMainHandle, state);
            Sleep(8);
		}
		EpicOverlay::Overlay::DestroyWindow();
	}
}