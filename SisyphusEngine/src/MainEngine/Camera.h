// Graphics/Camera/Camera.h
#pragma once
#include <memory>
#include <directxmath.h>

class Position;
class Frustum;

class Camera {
public:
    Camera();
	Camera(const Camera&) = delete;
    ~Camera();

    void InitView();
	void InitProjection(int, int, float, float);
    void Render();

public:
    void SetPosition(float, float, float);
    void SetRotation(float, float, float);

    Position* GetPosition() const { return m_Position.get(); }
    Frustum* GetFrustum() const { return m_Frustum.get(); }

    DirectX::XMMATRIX GetWorldMatrix();
    DirectX::XMMATRIX GetViewMatrix() const { return m_viewMatrix; }
    DirectX::XMMATRIX GetProjectionMatrix() const { return m_projectionMatrix; }

private:
    std::unique_ptr<Position> m_Position;
    std::unique_ptr<Frustum> m_Frustum;
    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_projectionMatrix;

    float m_fieldOfView;
    float m_screenNear;
    float m_screenDepth;
}; // Camera