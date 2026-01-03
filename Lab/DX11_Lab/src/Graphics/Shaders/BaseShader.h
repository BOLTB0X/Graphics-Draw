////////////////////////////////////////////////////////////////////////////////
// Filename: BaseShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BASESHADER_H_
#define _BASESHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>


using namespace DirectX;
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: BaseShader
////////////////////////////////////////////////////////////////////////////////
class BaseShader {
protected:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

protected:
	bool Init(ID3D11Device* device, HWND hwnd, const wchar_t* vsPath, const wchar_t* psPath);
	bool CompileShaderFromFile(ID3D11Device*, HWND, WCHAR*,LPCSTR, LPCSTR, ID3DBlob**);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool CreateMatrixBuffer(ID3D11Device*);
	bool CreateVertexInputLayout(ID3D11Device*, ID3DBlob*, const char**, DXGI_FORMAT*, unsigned int);
	void ShutdownShader();

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

public:	
	BaseShader();

public:
	virtual ~BaseShader() = 0;
	virtual bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) = 0;
};


#endif