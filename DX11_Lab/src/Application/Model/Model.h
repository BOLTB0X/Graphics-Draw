////////////////////////////////////////////////////////////////////////////////
// Filename: Model.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODEL_H_
#define _MODEL_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Texture.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: Model
////////////////////////////////////////////////////////////////////////////////
class Model
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	Model();
	Model(const Model&);
	~Model();

	bool Init(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	Texture* m_Texture;
}; // Model

#endif