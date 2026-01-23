#include "RenderStateWidget.h"
#include "imgui.h"

using namespace ImGui;


RenderStateWidget::RenderStateWidget(
    const std::string& name,
    bool* wire, bool* back, bool* depth)
    : IWidget(name)
{
    m_isWireframe = wire;
    m_isBack = back;
    m_isDepth = depth;
} // RenderStateWidget


void RenderStateWidget::Frame()
{
    Begin(i_name.c_str());

    if (m_isWireframe == nullptr || m_isBack == nullptr || m_isDepth == nullptr)
    {
        End();
        return;
    }

    Checkbox("Wireframe Mode", m_isWireframe);
    Checkbox("Backface Culling", m_isBack);
    Checkbox("Depth Test", m_isDepth);

    Separator();
    if (ImGui::Button("Reset"))
    {
        *m_isWireframe = false;
        *m_isBack = false;
        *m_isDepth = true;
    }

    End();
} // Frame