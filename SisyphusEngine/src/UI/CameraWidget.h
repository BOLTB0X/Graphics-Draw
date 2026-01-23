#pragma once
#include "IWidget.h"

class Camera;

class CameraWidget : public IWidget {
public:
    CameraWidget(const std::string&, Camera*);
    CameraWidget(const CameraWidget& other) = delete;

    virtual ~CameraWidget() = default;
    virtual void Frame() override;

private:
    Camera* m_Camera;
}; // CameraWidget