// UI/UI.cpp
#include "UI.h"
#include "StatsWidget.h"
#include "CameraWidget.h"
#include "RenderStateWidget.h"
// System
#include "Input.h"
#include "Gui.h"
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

/* default */
//////////////////////////////////////////////////////////

UI::UI()
    : m_Gui(nullptr),
    m_isCameraLocked(false)
{}


UI::~UI() 
{
    m_widgets.clear();
} // ~UI


bool UI::Init(std::shared_ptr<Gui> gui)
{
    if (gui == nullptr) return false;

    m_Gui = gui; // 참조 cnt + 1
    return true;
} // Init


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
    return mousePressed && !ImGui::GetIO().WantCaptureMouse;
} // IsWorldClicked