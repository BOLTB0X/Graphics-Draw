////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURESHADER_H_
#define _TEXTURESHADER_H_


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
// Class name: TextureShader
////////////////////////////////////////////////////////////////////////////////
class TextureShader: public BaseShader 
{
private:

public:
	TextureShader();
	TextureShader(const TextureShader&);
	virtual ~TextureShader();

	bool Init(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;

private:
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11SamplerState* m_sampleState;
}; // TextureShader

#endif