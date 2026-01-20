// UserInterface/Widget/MainSideBarWidget.h
#pragma once
#include <vector>
#include <memory>
// Framework
#include "Widget/IWidget.h"

class World;

class MainSideBarWidget : public IWidget {
public:
    MainSideBarWidget();
	MainSideBarWidget(const MainSideBarWidget&) = delete;
    MainSideBarWidget(const std::string&);
	virtual ~MainSideBarWidget() override;
    virtual void Frame() override;

    void AddComponent(std::unique_ptr<IWidget>);

public:
    template<typename T>
    T* GetComponent() {
        for (auto& comp : m_components) {
            T* target = dynamic_cast<T*>(comp.get());
            if (target) return target;
        }
        return nullptr;
    }



private:
    std::vector<std::unique_ptr<IWidget>> m_components;
}; // MainSideBarWidget
