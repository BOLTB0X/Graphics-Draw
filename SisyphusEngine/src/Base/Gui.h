// Base/Gui.h
#pragma once

#include <d3d11.h>
#include <vector>
#include <memory>

#include "Framework/Widget/IWidget.h"

class Gui {
public:
    Gui();
	Gui(const Gui&) = delete;
    ~Gui();

    bool Init(HWND, ID3D11Device*, ID3D11DeviceContext*);
    void Shutdown();

    void Begin();
    void End();

public:
    void AddWidget(std::unique_ptr<IWidget>);
    void FrameWidgets();

    template<typename T>
    T* GetWidget(const std::string& name)
    {
        for (auto& widget : m_widgets) {
            if (widget->GetName() == name)
                return dynamic_cast<T*>(widget.get());
        }
        return nullptr;
    } // GetWidget

private:
    std::vector<std::unique_ptr<IWidget>> m_widgets;
}; // Gui