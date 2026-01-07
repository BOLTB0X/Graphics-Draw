// Framework/Widget/ControlWidget.h
#pragma once
#include "IWidget.h"
#include "Graphics/Renderer/Renderer.h"


class ControlWidget : public IWidget {
public:
    ControlWidget(Renderer* renderer);
	ControlWidget(const ControlWidget&) = delete;
	virtual ~ControlWidget() = default;

    virtual void Frame() override;

public:
    bool IsWireframe() const { return m_isWireframe; }
	bool IsCullNone() const { return m_isCullNone; }

private:
    Renderer* m_renderer;
    bool m_isWireframe;
    bool m_isCullNone;
}; // ControlWidget