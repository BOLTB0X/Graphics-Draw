///////////////////////////////////////////////////////////////////////////////
// Filename: TerrainCell.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TerrainCell.h"


TerrainCell::TerrainCell()
	: m_vertexList(nullptr),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_lineVertexBuffer(nullptr),
	m_lineIndexBuffer(nullptr)
{
	m_vertexCount = 0;
	m_indexCount = 0;
	m_lineIndexCount = 0;
	m_maxWidth = 0;
	m_maxHeight = 0;
	m_maxDepth = 0;
	m_minWidth = 0;
	m_minHeight = 0;
	m_minDepth = 0;
	m_positionX = 0;
	m_positionY = 0;
	m_positionZ = 0;
} // TerrainCell


TerrainCell::TerrainCell(const TerrainCell& other)
	: m_vertexList(nullptr),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_lineVertexBuffer(nullptr),
	m_lineIndexBuffer(nullptr)
{
	m_vertexCount = 0;
	m_indexCount = 0;
	m_lineIndexCount = 0;
	m_maxWidth = 0;
	m_maxHeight = 0;
	m_maxDepth = 0;
	m_minWidth = 0;
	m_minHeight = 0;
	m_minDepth = 0;
	m_positionX = 0;
	m_positionY = 0;
	m_positionZ = 0;
} // TerrainCell


TerrainCell::~TerrainCell()
{
} // ~TerrainCell


bool TerrainCell::Init(ID3D11Device* device, 
	void* terrainModelPtr, int nodeIndexX, int nodeIndexY,
	int cellHeight, int cellWidth, int terrainWidth)
{
	ModelType* terrainModel;
	bool result;


	// 지형 모델을 가리키는 포인터를 모델 유형으로 강제 변환
	terrainModel = (ModelType*)terrainModelPtr;

	//해당 셀 인덱스에 대한 지형 데이터를 렌더링 버퍼에 로드
	result = InitBuffers(
		device, 
		nodeIndexX, nodeIndexY, 
		cellHeight, cellWidth, 
		terrainWidth, terrainModel);
	if (!result)
	{
		return false;
	}

	terrainModel = 0;

	// 셀 크기를 계산
	CalculateCellDimensions();

	// 이 셀 주변의 경계 상자를 생성하도록 디버그 라인 버퍼를 구성
	result = BuildLineBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
} // Init


void TerrainCell::Shutdown()
{
	ShutdownLineBuffers();
	ShutdownBuffers();
	return;
} // Shutdown


void TerrainCell::Render(ID3D11DeviceContext* deviceContext)
{
	// 버텍스 버퍼와 인덱스 버퍼를 그래픽 파이프라인에 배치하여 그리기 준비
	RenderBuffers(deviceContext);

	return;
} // Render


int TerrainCell::GetVertexCount()
{
	return m_vertexCount;
} // GetVertexCount


int TerrainCell::GetIndexCount()
{
	return m_indexCount;
} //  GetIndexCount


bool TerrainCell::InitBuffers(ID3D11Device* device, 
	int nodeIndexX, int nodeIndexY, 
	int cellHeight, int cellWidth,
	int terrainWidth, ModelType* terrainModel)
{
	VertexType* vertices;
	unsigned long* indices;
	int i, j, modelIndex, index;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// 지형 셀에 있는 꼭짓점의 개수를 계산
	m_vertexCount = (cellHeight - 1) * (cellWidth - 1) * 6;
	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 지형 모델 데이터와 로컬 버텍스/인덱스 배열에 인덱스를 설정
	modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;
	index = 0;

	// 버텍스 배열과 인덱스 배열에 데이터를 로드
	for (j = 0; j < (cellHeight - 1); j++)
	{
		for (i = 0; i < ((cellWidth - 1) * 6); i++)
		{
			vertices[index].position = XMFLOAT3(terrainModel[modelIndex].x, terrainModel[modelIndex].y, terrainModel[modelIndex].z);
			vertices[index].texture = XMFLOAT2(terrainModel[modelIndex].tu, terrainModel[modelIndex].tv);
			vertices[index].normal = XMFLOAT3(terrainModel[modelIndex].nx, terrainModel[modelIndex].ny, terrainModel[modelIndex].nz);
			vertices[index].tangent = XMFLOAT3(terrainModel[modelIndex].tx, terrainModel[modelIndex].ty, terrainModel[modelIndex].tz);
			vertices[index].binormal = XMFLOAT3(terrainModel[modelIndex].bx, terrainModel[modelIndex].by, terrainModel[modelIndex].bz);
			vertices[index].color = XMFLOAT3(terrainModel[modelIndex].r, terrainModel[modelIndex].g, terrainModel[modelIndex].b);
			indices[index] = index;
			modelIndex++;
			index++;
		}
		modelIndex += (terrainWidth * 6) - (cellWidth * 6);
	}

	// 정적 버텍스 버퍼에 대한 description 을 설정
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 정점 데이터에 대한 포인터를 제공
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 정적 인덱스 버퍼에 대한 description 을 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 이 셀에 대한 정점 정보에 접근하는 데 사용될 공개 정점 배열을 생성
	m_vertexList = new VectorType[m_vertexCount];
	if (!m_vertexList)
	{
		return false;
	}

	// 이 셀의 버텍스 위치 데이터의 로컬 복사본을 보관
	for (i = 0; i < m_vertexCount; i++)
	{
		m_vertexList[i].x = vertices[i].position.x;
		m_vertexList[i].y = vertices[i].position.y;
		m_vertexList[i].z = vertices[i].position.z;
	}

	// 버퍼가 생성되고 로드되었으므로 이제 배열을 해제
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
} // InitBuffers


void TerrainCell::ShutdownBuffers()
{
	if (m_vertexList)
	{
		delete[] m_vertexList;
		m_vertexList = 0;
	}


	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}


	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
} // ShutdownBuffers

void TerrainCell::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
} // RenderBuffers

void TerrainCell::CalculateCellDimensions()
{
	int i;
	float width, height, depth;

	m_maxWidth = -1000000.0f;
	m_maxHeight = -1000000.0f;
	m_maxDepth = -1000000.0f;

	m_minWidth = 1000000.0f;
	m_minHeight = 1000000.0f;
	m_minDepth = 1000000.0f;

	for (i = 0; i < m_vertexCount; i++)
	{
		width = m_vertexList[i].x;
		height = m_vertexList[i].y;
		depth = m_vertexList[i].z;

		// 너비가 최소값 또는 최대값을 초과하는지 확인
		if (width > m_maxWidth)
		{
			m_maxWidth = width;
		}
		if (width < m_minWidth)
		{
			m_minWidth = width;
		}

		// 높이가 최소값 또는 최대값을 초과하는지 확인
		if (height > m_maxHeight)
		{
			m_maxHeight = height;
		}
		if (height < m_minHeight)
		{
			m_minHeight = height;
		}

		// depth 최소 또는 최대값을 초과하는지 확인
		if (depth > m_maxDepth)
		{
			m_maxDepth = depth;
		}
		if (depth < m_minDepth)
		{
			m_minDepth = depth;
		}
	}

	// 중심 위치를 계산
	m_positionX = (m_maxWidth - m_minWidth) + m_minWidth;
	m_positionY = (m_maxHeight - m_minHeight) + m_minHeight;
	m_positionZ = (m_maxDepth - m_minDepth) + m_minDepth;

	return;
} // CalculateCellDimensions


bool TerrainCell::BuildLineBuffers(ID3D11Device* device)
{
	ColorVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	XMFLOAT4 lineColor;
	int index, vertexCount, indexCount;


	lineColor = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

	vertexCount = 24;
	indexCount = vertexCount;

	vertices = new ColorVertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// 버텍스 버퍼에 대한 설명을 설정
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ColorVertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 정점 데이터에 대한 포인터를 제공
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼의 설명을 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	index = 0;

	// 8개의 가로선
	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	// 4 세로선
	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_lineVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_lineIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더링을 위한 인덱스 카운트를 저장
	m_lineIndexCount = indexCount;

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
} // BuildLineBuffers


void TerrainCell::ShutdownLineBuffers()
{
	if (m_lineIndexBuffer)
	{
		m_lineIndexBuffer->Release();
		m_lineIndexBuffer = 0;
	}

	if (m_lineVertexBuffer)
	{
		m_lineVertexBuffer->Release();
		m_lineVertexBuffer = 0;
	}

	return;
} // ShutdownLineBuffers


void TerrainCell::RenderLineBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(ColorVertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_lineVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_lineIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
} // RenderLineBuffers


int TerrainCell::GetLineBuffersIndexCount()
{
	return m_lineIndexCount;
} // GetLineBuffersIndexCount


void TerrainCell::GetCellDimensions(
	float& maxWidth, float& maxHeight, float& maxDepth,
	float& minWidth, float& minHeight, float& minDepth)
{
	maxWidth = m_maxWidth;
	maxHeight = m_maxHeight;
	maxDepth = m_maxDepth;
	minWidth = m_minWidth;
	minHeight = m_minHeight;
	minDepth = m_minDepth;
	return;
} // GetCellDimensions