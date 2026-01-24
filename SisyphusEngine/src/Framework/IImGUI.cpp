#include "IImGUI.h"
// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
// Common
#include "EngineHelper.h"


IImGUI::IImGUI()
: m_isInitialized(false)
{
} // ImGui


IImGUI::~IImGUI()
{
} // ~ImGui


bool IImGUI::Init(HWND hwnd,
    ID3D11Device* device,
    ID3D11DeviceContext* deviceContext)
{
    if (m_isInitialized) return true;

    // ImGui 컨텍스트 생성 확인
    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == false) {
        EngineHelper::ErrorBox(hwnd, L"ImGui Context 생성 실패");
        return false;
    }

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (EngineHelper::SuccessCheck(ImGui_ImplWin32_Init(hwnd),
        "ImGui Win32 백엔드 초기화 실패") == false) return false;

    if (EngineHelper::SuccessCheck(ImGui_ImplDX11_Init(device, deviceContext),
        "ImGui DX11 백엔드 초기화 실패") == false) return false;

    ImGui::StyleColorsDark();

    m_isInitialized = true;
    return true;
} // Init


void IImGUI::Shutdown()
{
    if (m_isInitialized == false) return;
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    m_isInitialized = false;
} // Shutdown


void IImGUI::Begin()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
} // Begin


void IImGUI::End()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
} // End
