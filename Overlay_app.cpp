#include "Overlay_app.h"
#include <thread>


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define MINIMISE_MAX_FPS 60

namespace EpicOverlay {
    namespace Overlay {

        // Data
        static ID3D11Device* g_pd3dDevice = nullptr;
        static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
        static IDXGISwapChain* g_pSwapChain = nullptr;
        static ID3D11Texture2D* g_pBackBuffer = nullptr;
        static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
        static UINT                     g_windowWidth = 1280, g_windowHeight = 720;
        static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
        static HWND                     g_hwnd;
        static WNDCLASSEXW              g_wc;
        static BOOL                     s_windowVisible = true;
        static char                     s_textBuf[0x400];

        static double s_lastFrameStartTime = 0;

        // Forward declarations of helper functions
        bool CreateDeviceD3D(HWND hWnd);
        void CleanupDeviceD3D();
        void CreateRenderTarget();
        void CleanupRenderTarget();
        LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        double getSystemTimeSeconds() {
            LARGE_INTEGER t1 = {};
            LARGE_INTEGER freq = {};

            // High resolution windows only clocks
            QueryPerformanceCounter(&t1);
            QueryPerformanceFrequency(&freq);

            return static_cast<double>(t1.QuadPart) / static_cast<double>(freq.QuadPart);
        }

        bool StartWindow() {
            // Create application window
            //ImGui_ImplWin32_EnableDpiAwareness();
            g_wc = { sizeof(g_wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"VRCSkeletalHandsWindow", nullptr };
            ::RegisterClassExW(&g_wc);
            g_hwnd = ::CreateWindowW(g_wc.lpszClassName, L"Skeletal Hands Settings", WS_OVERLAPPEDWINDOW, 100, 100, g_windowWidth, g_windowHeight, nullptr, nullptr, g_wc.hInstance, nullptr);

            // Initialize Direct3D
            if (!CreateDeviceD3D(g_hwnd))
            {
                CleanupDeviceD3D();
                ::UnregisterClassW(g_wc.lpszClassName, g_wc.hInstance);
                return false;
            }

            // Show the window
            ::ShowWindow(g_hwnd, SW_SHOWDEFAULT);
            ::UpdateWindow(g_hwnd);

            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            //ImGui::StyleColorsLight();

            // Setup Platform/Renderer backends
            ImGui_ImplWin32_Init(g_hwnd);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

            // Load Fonts
            // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
            // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
            // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
            // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
            // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
            // - Read 'docs/FONTS.md' for more instructions and details.
            // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
            //io.Fonts->AddFontDefault();
            //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
            //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
            //IM_ASSERT(font != nullptr);

            SetupImgui();

            // Lock the main thread to the current core
            uint32_t desiredLogicalCore = 1; // @TODO: Change so that we can lock the thread onto a CPU core dynamically after querying system information
            // This is a bit mask where each bit corresponds to a logical core on the current system. We want to pin this thread onto a single core so that timings info is consistent
            // Set the bit at 'desiredLogicalCore' to 1
            uint64_t threadAffinityMask = 1ULL << desiredLogicalCore;
            SetThreadAffinityMask(GetCurrentThread(), threadAffinityMask);

            s_lastFrameStartTime = getSystemTimeSeconds();

            return true;
        }

        void DestroyWindow() {
            CleanupImgui();

            // Cleanup
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            CleanupDeviceD3D();
            ::DestroyWindow(g_hwnd);
            ::UnregisterClassW(g_wc.lpszClassName, g_wc.hInstance);

        }

        // Helper functions
        bool CreateDeviceD3D(HWND hWnd) {
            // Setup swap chain
            DXGI_SWAP_CHAIN_DESC sd;
            ZeroMemory(&sd, sizeof(sd));
            sd.BufferCount = 2;
            sd.BufferDesc.Width = 0;
            sd.BufferDesc.Height = 0;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hWnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;
            sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            UINT createDeviceFlags = 0;
            //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
            D3D_FEATURE_LEVEL featureLevel;
            const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
            HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
            if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
                res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
            if (res != S_OK)
                return false;

            CreateRenderTarget();
            return true;
        }

        void CleanupDeviceD3D()
        {
            CleanupRenderTarget();
            if (g_pBackBuffer) { g_pBackBuffer->Release(); g_pBackBuffer = nullptr; }
            if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
            if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
            if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
        }

        void CreateRenderTarget()
        {
            g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&g_pBackBuffer));
            if (g_pBackBuffer != nullptr) {
                g_pBackBuffer->Release();
            }
            g_pd3dDevice->CreateRenderTargetView(g_pBackBuffer, nullptr, &g_mainRenderTargetView);
        }

        void CleanupRenderTarget()
        {
            if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
        }

        // Win32 message handler
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
                return true;

            switch (msg)
            {
            case WM_SIZE:
                if (wParam == SIZE_MINIMIZED)
                    return 0;
                g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
                g_ResizeHeight = (UINT)HIWORD(lParam);
                return 0;
            case WM_ACTIVATE:
                if (wParam == WA_INACTIVE) {
                    s_windowVisible = false;
                    return 0;
                }
                else {
                    s_windowVisible = true;
                    return 0;
                }
                break;
            case WM_SYSCOMMAND:
                if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                    return 0;
                if ((wParam & 0xfff0) == SC_MINIMIZE) {
                    // Handle minimise
                    s_windowVisible = false;
                    return 0;
                }
                if ((wParam & 0xfff0) == SC_RESTORE) {
                    // Handle restore
                    s_windowVisible = true;
                    return 0;
                }
                break;
            case WM_DESTROY:
                ::PostQuitMessage(0);
                return 0;
            }
            return ::DefWindowProcW(hWnd, msg, wParam, lParam);
        }

        bool UpdateNativeWindow(vr::VROverlayHandle_t overlayMainHandle, App_State& state) {

            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

            // Poll and handle messages (inputs, window resize, etc.)
            // See the WndProc() function below for our to dispatch events to the Win32 backend.
            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    return false;
            }

            // Handle window resize (we don't resize directly in the WM_SIZE handler)
            if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
            {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
                g_windowWidth = g_ResizeWidth;
                g_windowHeight = g_ResizeHeight;
                g_ResizeWidth = g_ResizeHeight = 0;
                CreateRenderTarget();
            }

            // Handle overlay inputs
            bool dashboardVisible = false;
            if (overlayMainHandle && vr::VROverlay()) {
                auto& io = ImGui::GetIO();
                dashboardVisible = vr::VROverlay()->IsActiveDashboardOverlay(overlayMainHandle);

                static bool keyboardOpen = false, keyboardJustClosed = false;

                // After closing the keyboard, this code waits one frame for ImGui to pick up the new text from SetActiveText
                // before clearing the active widget. Then it waits another frame before allowing the keyboard to open again,
                // otherwise it will do so instantly since WantTextInput is still true on the second frame.
                if (keyboardJustClosed && keyboardOpen)
                {
                    ImGui::ClearActiveID();
                    keyboardOpen = false;
                }
                else if (keyboardJustClosed)
                {
                    keyboardJustClosed = false;
                }
                else if (!io.WantTextInput)
                {
                    // User might close the keyboard without hitting Done, so we unset the flag to allow it to open again.
                    keyboardOpen = false;
                }
                else if (io.WantTextInput && !keyboardOpen && !keyboardJustClosed)
                {
                    int id = ImGui::GetActiveID();
                    auto textInfo = ImGui::GetInputTextState(id);

                    if (textInfo != nullptr) {
                        s_textBuf[0] = 0;
                        int len = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)textInfo->TextW.Data, textInfo->TextW.Size, s_textBuf, sizeof(s_textBuf), NULL, NULL);
                        s_textBuf[min(len, sizeof(s_textBuf) - 1)] = 0;

                        uint32_t unFlags = 0; // EKeyboardFlags 

                        vr::VROverlay()->ShowKeyboardForOverlay(
                            overlayMainHandle, vr::k_EGamepadTextInputModeNormal, vr::k_EGamepadTextInputLineModeSingleLine,
                            unFlags, "VRC Skeletal Hands", sizeof s_textBuf, s_textBuf, 0
                        );
                        keyboardOpen = true;
                    }
                }

                vr::VREvent_t vrEvent;
                while (vr::VROverlay()->PollNextOverlayEvent(overlayMainHandle, &vrEvent, sizeof(vrEvent)))
                {
                    switch (vrEvent.eventType) {
                    case vr::VREvent_MouseMove:
                        io.AddMousePosEvent(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
                        break;
                    case vr::VREvent_MouseButtonDown:
                        io.AddMouseButtonEvent((vrEvent.data.mouse.button & vr::VRMouseButton_Left) == vr::VRMouseButton_Left ? 0 : 1, true);
                        break;
                    case vr::VREvent_MouseButtonUp:
                        io.AddMouseButtonEvent((vrEvent.data.mouse.button & vr::VRMouseButton_Left) == vr::VRMouseButton_Left ? 0 : 1, false);
                        break;
                    case vr::VREvent_ScrollDiscrete:
                    {
                        float x = vrEvent.data.scroll.xdelta * 360.0f * 8.0f;
                        float y = vrEvent.data.scroll.ydelta * 360.0f * 8.0f;
                        io.AddMouseWheelEvent(x, y);
                        break;
                    }
                    case vr::VREvent_KeyboardDone: {
                        vr::VROverlay()->GetKeyboardText(s_textBuf, sizeof s_textBuf);

                        int id = ImGui::GetActiveID();
                        auto textInfo = ImGui::GetInputTextState(id);
                        int bufSize = MultiByteToWideChar(CP_ACP, 0, s_textBuf, -1, NULL, 0);
                        textInfo->TextW.resize(bufSize);
                        MultiByteToWideChar(CP_ACP, 0, s_textBuf, -1, (LPWSTR)textInfo->TextW.Data, bufSize);
                        textInfo->CurLenW = bufSize;
                        textInfo->CurLenA = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)textInfo->TextW.Data, textInfo->TextW.Size, NULL, 0, NULL, NULL);

                        keyboardJustClosed = true;
                        break;
                    }
                    case vr::VREvent_Quit:
                        return false;
                    }
                }
            }

            if (s_windowVisible || dashboardVisible) {

                // Window should follow overlay resolution
                auto& io = ImGui::GetIO();
                io.DisplaySize = ImVec2((float)g_windowWidth, (float)g_windowHeight);
                io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

                // Overlay should ignore mouse inputs from the hardware
                io.ConfigFlags = io.ConfigFlags & ~ImGuiConfigFlags_NoMouseCursorChange;
                if (dashboardVisible) {
                    io.ConfigFlags = io.ConfigFlags | ImGuiConfigFlags_NoMouseCursorChange;
                }

                // Start the Dear ImGui frame
                ImGui_ImplDX11_NewFrame();
                ImGui_ImplWin32_NewFrame();
                ImGui::NewFrame();

                bool isDashboardVisible = false;
                DrawUi(isDashboardVisible, state);

                // Rendering
                ImGui::Render();
                const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
                g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
                g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

                // Only present if the window is visible
                if (g_windowWidth != 0 && g_windowHeight != 0) {
                    g_pSwapChain->Present(1, 0); // Present with vsync
                    //g_pSwapChain->Present(0, 0); // Present without vsync
                }

                // Only present to SteamVR if the overlay is open
                if (dashboardVisible)
                {
                    vr::Texture_t vrTex;
                    vrTex.eType = vr::TextureType_DirectX;
                    vrTex.eColorSpace = vr::ColorSpace_Auto;
                    vrTex.handle = (void*)g_pBackBuffer;

                    vr::HmdVector2_t mouseScale = { (float)g_windowWidth, (float)g_windowHeight };

                    vr::VROverlay()->SetOverlayTexture(overlayMainHandle, &vrTex);
                    vr::VROverlay()->SetOverlayMouseScale(overlayMainHandle, &mouseScale);
                }
            } 
            else {
                double targetFrameTime = 1.0/MINIMISE_MAX_FPS;
                double waitTime = targetFrameTime - (getSystemTimeSeconds() - s_lastFrameStartTime);
                if (waitTime > 0) {
                    std::this_thread::sleep_for(std::chrono::duration<double>(waitTime));
                }
                s_lastFrameStartTime += targetFrameTime;
            }

            return true;
        }
    }
}