// Framework/Widget/IWidget.h
#pragma once
#include <string>

class IWidget {
public:
    IWidget();
	IWidget(const IWidget&) = delete;
    IWidget(const std::string& name) : m_name(name), m_isVisible(true) {}
    virtual ~IWidget() = default;
    virtual void Frame() = 0;

    void SetVisible(bool visible) { m_isVisible = visible; }
    bool IsVisible() const { return m_isVisible; }
    const std::string& GetName() const { return m_name; }

protected:
    std::string m_name;
    bool m_isVisible;
}; // IWidget
