////////////////////////////////////////////////////////////////////////////////
// Filename: App.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APP_H_
#define _APP_H_


//////////////
// INCLUDES //
//////////////
#include <windows.h>
//#include "Application/Renderer/Resources/Model.h"
//#include "Application/Renderer/Shaders/TextureShader.h"
#include "Application/D3DRenderer/D3DRenderer.h"
#include "Application/Camera/Camera.h"
#include "Application/Model/Model.h"
#include "Application/Shaders/Shader.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


//class Win32Window;
//class DX11Renderer;

class App
{
public:
    App();
    App(const App&);
    ~App();

    bool Init(int, int, HWND);
    //App(HINSTANCE hInstance);
    //~App();

   // void Run();
    void Shutdown();
    bool Frame();

private:
    bool Render();

private:
    D3DRenderer* m_Direct3D;
    Camera* m_Camera;
    Model* m_Model;
    Shader* m_TextureShader;
private:
    //HINSTANCE m_hInstance;

    //Model* m_RockModel = nullptr;
    //TextureShader* m_TextureShader = nullptr;

    //DirectX::XMMATRIX m_World;
    //DirectX::XMMATRIX m_View;
    //DirectX::XMMATRIX m_Proj;
}; // App

#endif