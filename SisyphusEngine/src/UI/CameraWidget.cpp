#include "CameraWidget.h"
#include <DirectXMath.h>
// 
#include "Camera.h"
// Imgui
#include "imgui.h"

using namespace DirectX;
using namespace ImGui;


CameraWidget::CameraWidget(const std::string& name, Camera* camera)
    : IWidget(name),
	m_Camera(camera)
{
}

void CameraWidget::Frame()
{
    Begin(i_name.c_str());

	if (m_Camera == nullptr)
	{
		End();
        return;
	}

    XMFLOAT3 pos = m_Camera->GetPosition();
    XMFLOAT3 rot = m_Camera->GetRotation();

    Text("Camera Location");

    bool isChanged = false;
    if (DragFloat3("Pos", &pos.x, 0.1f)) isChanged = true;
    if (DragFloat3("Rot", &rot.x, 0.1f)) isChanged = true;

    if (isChanged)
    {
        m_Camera->SetPosition(pos);
        m_Camera->SetRotation(rot);
    }

    Separator();

    if (ImGui::Button("Reset Camera"))
    {
        m_Camera->SetPosition(0.0f, 0.0f, -6.0f);
        m_Camera->SetRotation(0.0f, 0.0f, 0.0f);
    }
    End();
} // Frame