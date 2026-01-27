#pragma once
#include <d3d11.h>
#include <memory>
#include <windows.h>
#include <DirectXMath.h>
// Common
#include "PropertyHelper.h"


class Renderer;
class TexturesManager;
class ShaderManager;
class DefaultModel;
class Light;


class RenderingEngine {
public:
	RenderingEngine();
	RenderingEngine(const RenderingEngine& other) = delete;
	~RenderingEngine();

	bool Init(HWND);
	void Shutdown();

	void Draw(float,
		PropertyHelper::Property<DirectX::XMMATRIX>,
		PropertyHelper::Property<DirectX::XMMATRIX>,
		PropertyHelper::Property<DirectX::XMFLOAT3>);

	void BeginScene(float, float, float, float);
	void EndScene();

public:
	void SetMode(bool, bool);
	void SetDepthBuffer(bool);
	void SetWireframeEnable(bool val) { m_isWireframe = val; }
	void SetBackCullEnable(bool val) { m_backCullEnable = val; }
	void SetDepthEnable(bool val) { m_depthEnable = val; }

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	bool GetWireframeEnable() const { return m_isWireframe; }
	bool GetBackCullEnable() const { return m_backCullEnable; }
	bool GetDepthEnable() const { return m_depthEnable; }

private:
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<TexturesManager> m_TexturesManager;
	std::unique_ptr<ShaderManager> m_ShaderManager;
	std::unique_ptr<DefaultModel> m_SunModel;
	std::unique_ptr<DefaultModel> m_CloudArea;
	std::unique_ptr<DefaultModel> m_BicubicMesh;
	std::unique_ptr<DefaultModel> m_Sky;
	std::unique_ptr<Light> m_Light;

	bool m_isWireframe;
	bool m_backCullEnable;
	bool m_depthEnable;
	uint32_t m_frameCount;
}; // RenderingEngine