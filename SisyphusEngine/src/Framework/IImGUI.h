#pragma once

#include <d3d11.h>
#include <vector>
#include <memory>

class IImGUI {
public:
    IImGUI();
    IImGUI(const IImGUI&) = delete;
    ~IImGUI();

    bool Init(HWND, ID3D11Device*, ID3D11DeviceContext*);
    void Shutdown();

    void Begin();
    void End();

private:
    bool m_isInitialized;
}; // ImGUI