////////////////////////////////////////////////////////////////////////////////
// Filename: Application.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATION_H_
#define _APPLICATION_H_


//////////////
// INCLUDES //
//////////////
#include "Base/Input/Input.h"
#include "Base/Timer/Timer.h"
#include "Base/Fps/Fps.h"

#include "Graphics/Renderer/D3DRenderer.h"
#include "Graphics/Resources/Model/Model.h"
#include "Graphics/Resources/Texture/TextureManager.h"
#include "Graphics/Shaders/ShaderManager.h"

#include "World/Actors/Stone/Stone.h"
#include "World/Zone/Zone.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

////////////////////////////////////////////////////////////////////////////////
// Class name: Application
////////////////////////////////////////////////////////////////////////////////
class Application
{
public:
    Application();
    Application(const Application&);
    ~Application();

    bool Init(int, int, HWND);
    void Shutdown();
    bool Frame();
    Input* GetInput();

private:
    bool Render();

private:
    D3DRenderer* m_Direct3D;
    //Camera* m_Camera;
    Model* m_Model;
    //Light* m_Light;
    //Stone* m_PlayerStone;
    //Stone* m_WallStone;
    TextureManager* m_TextureManager;
    ShaderManager* m_ShaderManager;
    Input* m_Input;

    Timer* m_Timer;
    Fps* m_Fps;
    Zone* m_Zone;
}; // Application

#endif