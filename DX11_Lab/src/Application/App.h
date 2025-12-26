////////////////////////////////////////////////////////////////////////////////
// Filename: App.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APP_H_
#define _APP_H_


//////////////
// INCLUDES //
//////////////
#include "../Framework/Input/Input.h"
#include "../Framework/Camera/Camera.h"
#include "./Framework/Renderer/D3DRenderer.h"

#include "../Resources/Model/Model.h"
#include "../Resources/Light/Light.h"
#include "../Resources/Shaders/ShaderManager.h"

#include "../Actors/Stone.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

////////////////////////////////////////////////////////////////////////////////
// Class name: App
////////////////////////////////////////////////////////////////////////////////
class App
{
public:
    App();
    App(const App&);
    ~App();

    bool Init(int, int, HWND);
    void Shutdown();
    bool Frame(Input*);

private:
    bool Render();

private:
    D3DRenderer* m_Direct3D;
    Camera* m_Camera;
    Model* m_Model;
    Light* m_Light;
    Stone* m_PlayerStone;     // 조종할 돌
    Stone* m_WallStone;       // 벽 역할을 할 돌
    ShaderManager* m_ShaderManager;
	Input* m_Input;
}; // App

#endif