// UI/UI.cpp
#include "UI.h"
#include "StatsWidget.h"
#include "CameraWidget.h"
#include "RenderStateWidget.h"
// System
#include "Input.h"
// MainEngine
#include "Timer.h"
#include "Fps.h"
#include "Camera.h"
// Framework
#include "IWidget.h"
// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


UI::UI()
    : m_isCameraLocked(false)
{
    m_Gui = std::make_unique<IImGUI>();
} // UI


UI::~UI() 
{
    //Shutdown();
} // ~UI


bool UI::Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    return m_Gui->Init(hwnd, device, deviceContext);
} // Init


void UI::Shutdown()
{
    m_widgets.clear();
    if (m_Gui) m_Gui->Shutdown();
} // Shutdown


void UI::Render()
{
    if (m_Gui == nullptr) return;

    m_Gui->Begin();

    for (auto& widget : m_widgets)
    {
        if (widget->IsVisible())
        {
            widget->Frame();
        }
    }

    m_Gui->End();
    return;
} // Render


void UI::Begin()
{
    if (m_Gui) m_Gui->Begin();
} // Begin


void UI::End()
{
    if (m_Gui) m_Gui->End();
} // End

bool UI::CanControlWorld() const
{
    if (m_Gui == nullptr) { return false; }
    ImGuiIO& io = ImGui::GetIO();
    return !(io.WantCaptureMouse || io.WantCaptureKeyboard);
} // CanControlWorld


void UI::AddWidget(std::unique_ptr<IWidget> widget)
{
    m_widgets.push_back(std::move(widget));
    return;
} // AddWidget


void UI::CreateWidget(Timer* timer, Fps* fps, Camera* camera, bool* wire, bool* back, bool* depth)
{
    AddWidget(std::make_unique<StatsWidget>("Engine Stats", timer, fps));
    AddWidget(std::make_unique<CameraWidget>("Camera Controller", camera));
    AddWidget(std::make_unique<RenderStateWidget>("Render Settings", wire, back, depth));
}
 // CreateWidget

void UI::ToggleWidget()
{

    for (auto& widget : m_widgets)
    {
        widget->SetVisible(!widget->IsVisible());
    }
} // ToggleWidget


bool UI::IsWorldClicked(bool mousePressed) const
{
    if (m_Gui == nullptr) return false;
    return mousePressed && !ImGui::GetIO().WantCaptureMouse;
} // IsWorldClicked