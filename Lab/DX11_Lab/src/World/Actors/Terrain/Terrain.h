////////////////////////////////////////////////////////////////////////////////
// Filename: Terrain.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAIN_H_
#define _TERRAIN_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <stdio.h>

#include "TerrainCell.h"
#include "Camera/Frustum.h"

using namespace DirectX;
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: Terrain
////////////////////////////////////////////////////////////////////////////////
class Terrain
{
private:
	//struct VertexType
	//{
	//	XMFLOAT3 position;
	//	XMFLOAT2 texture;
	//	XMFLOAT3 normal;
	//	XMFLOAT3 tangent;
	//	XMFLOAT3 binormal;
	//	XMFLOAT3 color;
	//};

	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float r, g, b;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float r, g, b;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	Terrain();
	Terrain(const Terrain&);
	~Terrain();

	bool Init(ID3D11Device*, char*);
	void Shutdown();

	void Frame();
	bool Render(ID3D11DeviceContext*);

	int GetIndexCount();

	bool RenderCell(ID3D11DeviceContext*, int, Frustum*);
	bool RenderCell(ID3D11DeviceContext*, int);
	void RenderCellLines(ID3D11DeviceContext*, int);

	int GetCellIndexCount(int);
	int GetCellLinesIndexCount(int);
	int GetCellCount();

	int GetRenderCount();
	int GetCellsDrawn();
	int GetCellsCulled();

	bool GetHeightAtPosition(float, float, float&);

private:
	bool LoadSetupFile(char*);
	//bool LoadBitmapHeightMap();
	bool LoadRawHeightMap();

	void ShutdownHeightMap();
	void SetTerrainCoordinates();

	bool CalculateNormals();
	bool LoadColorMap();
	bool BuildTerrainModel();
	void ShutdownTerrainModel();

	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

	bool InitBuffers(ID3D11Device*);
	void ShutdownBuffers();
	//void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTerrainCells(ID3D11Device*);
	void ShutdownTerrainCells();
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	char* m_terrainFilename, *m_colorMapFilename;
	HeightMapType* m_heightMap;
	ModelType* m_terrainModel;

	TerrainCell* m_TerrainCells;
	int m_cellCount, m_renderCount, m_cellsDrawn, m_cellsCulled;
}; // Terrain

#endif