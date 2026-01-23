// UI/UI.h
#pragma once
#include <memory>
#include <vector>
#include <string>


class IWidget;
class Gui;
class Fps;
class Timer;
class Renderer;
class Camera;


class UI {
public:
    UI();
    UI(const UI& other) = delete;
    ~UI();

    bool Init(std::shared_ptr<Gui> gui);
    void Render();

    void Begin();
    void End();
    bool CanControlWorld() const;


public:
    void AddWidget(std::unique_ptr<IWidget>);
    void ToggleWidget();
    void CreateWidget(Timer*, Fps*, Camera*, bool*, bool*, bool*);
    bool IsWorldClicked(bool mousePressed) const;

public:
    template<typename T>
    T* GetWidget(const std::string& name)
    {
        for (auto& widget : m_widgets) {
            if (widget->GetName() == name)
                return dynamic_cast<T*>(widget.get());
        }
        return nullptr;
    }

    bool IsCameraLocked() const { return m_isCameraLocked; }
    void SetCameraLocked(bool lock) { m_isCameraLocked = lock; }

private:
    std::shared_ptr<Gui> m_Gui;
    std::vector<std::unique_ptr<IWidget>> m_widgets;
    //MainSideBarWidget* m_MainSideBar;
    bool m_isCameraLocked;
}; // UI