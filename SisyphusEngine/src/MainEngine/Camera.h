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

    void BuildFrustum();

public:
    void SetPosition(DirectX::XMFLOAT3);
    void SetPosition(float, float, float);
    void SetRotation(DirectX::XMFLOAT3);
    void SetRotation(float, float, float);
    void SetLookAt(DirectX::XMFLOAT3);
    void SetLookAt(float, float, float);
    void SetFOV(float, int, int);

    DirectX::XMFLOAT3 GetPosition() const;
    DirectX::XMFLOAT3 GetRotation() const;
    DirectX::XMVECTOR GetForwardVector() const;
    DirectX::XMVECTOR GetRightVector() const;
    DirectX::XMMATRIX GetWorldMatrix() const;
    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;
    Position* GetPositionPtr() const { return m_Position.get(); }
    Frustum* GetFrustum() const { return m_Frustum.get(); }

    void AddRotation(float, float);
    void AddPitch(float);
    void AddYaw(float);
    void AddFOV(float);

private:
    std::unique_ptr<Position> m_Position;
    std::unique_ptr<Frustum> m_Frustum;
    DirectX::XMFLOAT3 m_up;

    float m_fov, m_near, m_far, m_aspect;
    const float m_maxPitch;
    const float m_minPitch;
}; // Camera