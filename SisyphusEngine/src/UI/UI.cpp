// UI/UI.cpp
#include "UI.h"

// System
#include "Input.h"
#include "Gui.h"
// Graphics
//#include "Renderer/Renderer.h"
// Engine
#include "Timer.h"
#include "Fps.h"
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
    m_MainSideBar(nullptr) {}


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


void UI::Frame()
{
    //for (auto& widget : m_widgets)
    //{
    //    widget->Frame(); // 필요 시
    //}

    return;
} // Frame


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

//////////////////////////////////////////////////////////////////

/* public */
/////////////////////////////////////////////////////////////////


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
    //if (widget->GetName().find("MainSideBar") != std::string::npos)
    //    m_MainSideBar = static_cast<MainSideBarWidget*>(widget.get());
    m_widgets.push_back(std::move(widget));
    return;
} // AddWidget


void UI::ToggleMainSideBar()
{
    /*if (m_MainSideBar) m_MainSideBar->SetVisible(!m_MainSideBar->IsVisible());*/
} // ToggleMainSideBar


void UI::CreateSideBar(std::string name, Timer* timer, Fps* fps, Renderer* renderer, World* world)
{
    return;
} // CreateSideBar


void UI::ApplyRenderStates(Renderer* renderer)
{
   
    return;
} // ApplyRenderStates

bool UI::IsWorldClicked(bool mousePressed) const
{
    return mousePressed && !ImGui::GetIO().WantCaptureMouse;
} // IsWorldClicked