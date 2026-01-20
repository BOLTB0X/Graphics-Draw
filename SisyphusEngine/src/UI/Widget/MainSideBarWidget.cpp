// UserInterface/Widget/MainSideBarWidget.h
#include "MainSideBarWidget.h"
#include "imgui.h"

#include "World.h"

/* default */
////////////////////////////////////////////////////////////////////

MainSideBarWidget::MainSideBarWidget()
	: IWidget("F1:Sisyphus Editor")
{
} // MainSideBarWidget


MainSideBarWidget::MainSideBarWidget(const std::string& name)
    : IWidget(name)
{
} // MainSideBarWidget


MainSideBarWidget::~MainSideBarWidget()
{
} // ~MainSideBarWidget


void MainSideBarWidget::Frame()
{
    if (i_isVisible == false) return;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 800), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(i_name.c_str(), &i_isVisible))
    {
        for (auto& comp : m_components)
        {
            ImGui::PushID(comp->GetName().c_str());

            if (ImGui::CollapsingHeader(comp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                comp->Frame();
            }

            ImGui::Spacing();
            ImGui::PopID();
        }
    }
    ImGui::End();
} // Frame


void MainSideBarWidget::AddComponent(std::unique_ptr<IWidget> component)
{
    m_components.push_back(std::move(component));
} // AddComponent


//void MainSideBarWidget::RenderCameraSection(World* world)
//{
//    if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen))
//    {
//        // 현재 제어 상태 표시
//        bool isControlling = world->CanControlWorld();
//        ImGui::Text("Control State: %s", isControlling ? "WORLD (Active)" : "UI (Paused)");
//
//        ImGui::Separator();
//
//        // 원점 복귀 버튼
//        // 버튼의 가로 길이를 창 너비에 맞춤 (-1)
//        if (ImGui::Button("Reset Camera to Origin", ImVec2(-1, 30)))
//        {
//            auto* camPos = m_World->GetCamera()->GetPosition();
//
//            // 위치 초기화 (y를 약간 높여서 지형을 내려다보게 설정)
//            camPos->SetPosition(0.0f, 5.0f, -15.0f);
//
//            // 회전 초기화
//            camPos->SetRotation(0.0f, 0.0f, 0.0f);
//
//            // 가속도 시스템을 사용 중이므로 속도값들도 0으로 밀어주는 것이 좋습니다.
//            // camPos->ResetVelocities(); // 필요시 Position 클래스에 추가
//        }
//
//        ImGui::TextWrapped("Tip: Click on empty world space to toggle camera control.");
//    }
//}