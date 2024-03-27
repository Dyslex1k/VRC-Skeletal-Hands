#include <iostream>
#include "hekky-osc.hpp"
#include "openvr.h"
#include "Overlay_app.h"

// Props for the overlay
#define OPENVR_APPLICATION_KEY "Dyslex1k.Epic"
static vr::VROverlayHandle_t s_overlayMainHandle;
static vr::VROverlayHandle_t s_overlayThumbnailHandle;

bool InitVR()
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
    return true;
}

void TryCreateVrOverlay() {
    if (s_overlayMainHandle || !vr::VROverlay()) {
        return;
    }

    vr::VROverlayError err = vr::VROverlay()->CreateDashboardOverlay(
        OPENVR_APPLICATION_KEY, "EpicOverlay",
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

int main() {
    App_State state;

    InitVR();

	if (EpicOverlay::Overlay::StartWindow()) {
		bool doExectute = true;
		while (doExectute) {
            TryCreateVrOverlay();

			doExectute = EpicOverlay::Overlay::UpdateNativeWindow(s_overlayMainHandle, state);
		}
		EpicOverlay::Overlay::DestroyWindow();
	}
}