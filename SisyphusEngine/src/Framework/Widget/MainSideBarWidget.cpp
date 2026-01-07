// Framework/Widget/MainSideBarWidget.h
#include "MainSideBarWidget.h"
#include "imgui.h"

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
    if (m_isVisible == false) return;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 800), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(m_name.c_str(), &m_isVisible))
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