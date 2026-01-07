// Framework/Widget/ControlWidget.cpp
#include "ControlWidget.h"
#include "imgui.h"

/* default */
/////////////////////////////////////////////////////////////////////

ControlWidget::ControlWidget(Renderer* renderer)
	: IWidget("Control"),
	m_renderer(renderer),
	m_isWireframe(false),
	m_isCullNone(false)
{
} // ControlWidget


void ControlWidget::Frame()
{
    ImGui::TextDisabled("Rasterizer Settings");

    // 1. 와이어프레임 체크박스
    if (ImGui::Checkbox("Wireframe Mode", &m_isWireframe))
    {
    }

    const char* cullingLabel = m_isCullNone ? "Both Sides (Cull None)" : "Back-face Culling";
    if (ImGui::Checkbox(cullingLabel, &m_isCullNone))
    {
    }

    ImGui::Separator();
} // Frame