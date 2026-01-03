////////////////////////////////////////////////////////////////////////////////
// Filename: NormalMapShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _NORMALMAPSHADER_H_
#define _NORMALMAPSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "BaseShader.h"

using namespace DirectX;
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: NormalMapShader
////////////////////////////////////////////////////////////////////////////////
class NormalMapShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

public:
	NormalMapShader();
	NormalMapShader(const NormalMapShader&);
	virtual ~NormalMapShader();

	bool Init(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;

private:
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
}; // NormalMapShader

#endif