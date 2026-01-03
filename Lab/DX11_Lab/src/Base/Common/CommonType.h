////////////////////////////////////////////////////////////////////////////////
// Filename: CommonType.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COMMONTYPE_H_
#define _COMMONTYPE_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <string>

#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Resources/Light/Light.h"
#include "World/Camera/Frustum.h"

using namespace DirectX;
using namespace std;

class ShaderManager;
class Light;
class Frustum;
struct ID3D11DeviceContext;

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
};

struct ModelType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
};

struct TempVertexType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
};

struct VectorType
{
	float x, y, z;
};

struct HeightMapType
{
	float x, y, z;
	float nx, ny, nz;
	float r, g, b;
};


struct MeshData {
	std::vector<VertexType> vertices;
	std::vector<unsigned long> indices;
	std::vector<std::string> texturePaths;
};

#endif