////////////////////////////////////////////////////////////////////////////////
// Filename: ColorShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_


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
// Class name: ColorShader
////////////////////////////////////////////////////////////////////////////////
class ColorShader : public BaseShader
{
public:
	ColorShader();
	ColorShader(const ColorShader&);
	virtual ~ColorShader();

	bool Init(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;

private:
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);
}; // ColorShader

#endif