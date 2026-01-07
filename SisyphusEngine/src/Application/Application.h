// Application/Application.h
#pragma once

#include "Base/Input.h"
#include "Base/Timer.h"
#include "Base/Fps.h"
#include "Base/Gui.h"
#include "Graphics/Renderer/Renderer.h"
#include "World/World.h"
#include "Framework/Widget/MainSideBarWidget.h"
#include "ShaderManager/ShaderManager.h"
#include "ModelManager/ModelManager.h"
#include "TexturesManager/TexturesManager.h"

class Application
{
public:
    Application();
    Application(const Application&) = delete;
    ~Application();

    bool Init(HWND, int, int);
    void Shutdown();
    bool Frame();

public:
    Input* GetInput() { return m_Input; };

private:
    bool Render();
    void InitGui();
	void RenderGui();
    void HandleEditorMode();

private:
    Input* m_Input;
    Timer* m_Timer;
    Fps* m_Fps;
    Gui* m_Gui;
	MainSideBarWidget* m_mainSideBarWidget;

private:
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<ShaderManager> m_ShaderManager;
    std::unique_ptr<ModelManager> m_ModelManager;
    std::unique_ptr<TexturesManager> m_TextureManager;
    std::unique_ptr<World> m_World;

}; // Application