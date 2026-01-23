#pragma once
#include "IWidget.h"


class RenderStateWidget : public IWidget {
public:
    RenderStateWidget(const std::string&, bool*, bool*, bool*);
    RenderStateWidget(const RenderStateWidget& other) = delete;
    
    virtual ~RenderStateWidget() = default;
    virtual void Frame() override;

private:
    bool* m_isWireframe;
    bool* m_isBack;
    bool* m_isDepth;
}; // RenderStateWidget