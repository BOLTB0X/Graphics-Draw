////////////////////////////////////////////////////////////////////////////////
// Filename: Zone.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Zone.h"


Zone::Zone()
	: m_Camera(nullptr),
	m_Position(nullptr),
	m_Terrain(nullptr),
	m_Light(nullptr),
	m_Frustum(nullptr),
	m_PlayerStone(nullptr),
	m_WallStone(nullptr),
	m_wireFrame(false),
	m_cellLines(false),
	m_heightLocked(false)
{
} // Zone


Zone::Zone(const Zone& other)
	: m_Camera(nullptr),
	m_Position(nullptr),
	m_Terrain(nullptr),
	m_Light(nullptr),
	m_Frustum(nullptr),
	m_PlayerStone(nullptr),
	m_WallStone(nullptr),
	m_wireFrame(false),
	m_cellLines(false),
	m_heightLocked(false)
{
} // Zone


Zone::~Zone()
{
} // ~Zone


bool Zone::Init(D3DRenderer* Direct3D, HWND hwnd,
	int screenWidth, int screenHeight, float screenDepth,
	Model* sharedModel)
{
	bool result;

	m_Camera = new Camera;
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	m_Light = new Light;
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-0.5f, -1.0f, -0.5f);

	m_Position = new Position;
	m_Position->SetPosition(128.0f, 10.0f, -10.0f);
	m_Position->SetRotation(0.0f, 0.0f, 0.0f);

	m_Frustum = new Frustum;
	// Initialize the frustum object.
	m_Frustum->Init(screenDepth);

	m_Terrain = new Terrain;
	if (!m_Terrain)
	{
		return false;
	}
	result = m_Terrain->Init(Direct3D->GetDevice(), "assets/Terrain/setup.txt");
	if (!result)
	{
		MessageBoxW(hwnd, L"terrain 초기화 실패", L"Error", MB_OK);
		return false;
	}

	// 조종할 돌
	m_PlayerStone = new Stone;
	m_PlayerStone->Init(sharedModel, 128.0f, 5.0f, 128.0f, 1.0f);

	// 벽이 될 돌
	m_WallStone = new Stone;
	m_WallStone->Init(sharedModel, 132.0f, 5.0f, 128.0f, 1.0f);

	m_wireFrame = false;

	m_cellLines = false;

	m_heightLocked = true;

	return true;
} // init


void Zone::Shutdown()
{
	if (m_WallStone) {
		delete m_WallStone;
		m_WallStone = 0;
	}
	if (m_PlayerStone) {
		delete m_PlayerStone;
		m_PlayerStone = 0;
	}

	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	return;
} // Shutdown


bool Zone::Frame(D3DRenderer* Direct3D, Input* input,
	ShaderManager* shaderManager, TextureManager* textureManager,
	float frameTime, int fps)
{
	bool result, foundHeight;
	//float posX, posY, posZ, rotX, rotY, rotZ, height;
	float height;

	XMFLOAT3 pos, rot;

	// 프레임 입력 처리를 수행
	HandleMovementInput(input, frameTime);

	pos = m_Position->GetPosition();
	rot = m_Position->GetRotation();

	m_Terrain->Frame();

	if (m_heightLocked)
	{
		// 주어진 카메라 위치 바로 아래에 있는 삼각형의 높이를 구함
		foundHeight = m_Terrain->GetHeightAtPosition(pos.x, pos.z, height);
		if (foundHeight)
		{
			// 카메라 아래에 삼각형이 있다면 카메라를 그 삼각형 바로 위 1미터 위치에 놓음
			m_Position->SetPosition(pos.x, height + 1.0f, pos.x);
			m_Camera->SetPosition(pos.x, height + 1.0f, pos.z);
		}
	}

	result = Render(Direct3D, shaderManager, textureManager);
	if (!result)
	{
		return false;
	}

	return true;
} // Frame


bool Zone::Render(D3DRenderer* renderer,
	ShaderManager* shaderManager, TextureManager* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	XMFLOAT3 cameraPosition;
	bool result;


	//  카메라 위치를 기반으로 뷰 행렬을 생성
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드 행렬, 뷰 행렬, 프로젝션 행렬을 가져옴
	renderer->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	renderer->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	renderer->GetOrthoMatrix(orthoMatrix);

	cameraPosition = m_Camera->GetPosition();
	m_Frustum->ConstructFrustum(projectionMatrix, viewMatrix);

	renderer->TurnOffCulling();
	renderer->TurnZBufferOff();

	worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	renderer->GetWorldMatrix(worldMatrix);
	renderer->TurnZBufferOn();
	renderer->TurnOnCulling();


	if (m_wireFrame)
	{
		renderer->EnableWireframe();
	}

	for (unsigned int i = 0; i < m_Terrain->GetCellCount(); i++)
	{
		// Render each terrain cell if it is visible only.
		result = m_Terrain->RenderCell(renderer->GetDeviceContext(), i, m_Frustum);
		if (result)
		{
			// Render the cell buffers using the terrain shader.
			result = shaderManager->RenderTerrainShader(
				renderer->GetDeviceContext(),
				m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix,
				projectionMatrix, textureManager->GetTexture(0), textureManager->GetTexture(1),
				m_Light->GetDirection(), m_Light->GetDiffuseColor());
			if (!result)
			{
				return false;
			}

			// If needed then render the bounding box around this terrain cell using the color shader. 
			if (m_cellLines)
			{
				m_Terrain->RenderCellLines(renderer->GetDeviceContext(), i);
				shaderManager->RenderColorShader(renderer->GetDeviceContext(), m_Terrain->GetCellLinesIndexCount(i), worldMatrix,
					viewMatrix, projectionMatrix);
				if (!result)
				{
					return false;
				}
			}
		}
	}

	/*m_Terrain->Render(renderer->GetDeviceContext());
	result = shaderManager->RenderTerrainShader(
		renderer->GetDeviceContext(),
		m_Terrain->GetIndexCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		textureManager->GetTexture(0),
		textureManager->GetTexture(1),
		m_Light->GetDirection(),
		m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}*/

	//result = m_PlayerStone->Render(renderer->GetDeviceContext(), shaderManager,
	//	viewMatrix, projectionMatrix, m_Light);
	//if (!result)
	//	return false;

	//result = m_WallStone->Render(renderer->GetDeviceContext(), shaderManager,
	//	viewMatrix, projectionMatrix, m_Light);
	//if (!result)
	//	return false;

	if (m_wireFrame)
	{
		renderer->DisableWireframe();
	}

	return true;
} // Render


void Zone::HandleMovementInput(Input* input, float frameTime)
{
	bool keyDown;
	XMFLOAT3 pos, rot;
	//float posX, posY, posZ, rotX, rotY, rotZ;

	float moveSpeed = 0.01f;
	float rotateSpeed = 0.1f;


	// 업데이트된 위치를 계산할 프레임 시간을 설정
	m_Position->SetFrameTime(frameTime);

	if (input->IsAPressed()) {
		m_PlayerStone->Move(-moveSpeed, 0.0f, 0.0f); // X축 감소
		if (m_PlayerStone->CheckCollision(m_WallStone))
		{
			m_PlayerStone->Move(moveSpeed, 0.0f, 0.0f);
			m_WallStone->Move(-moveSpeed * 0.5f, 0.0f, 0.0f);
			m_WallStone->Update(0.0f, -rotateSpeed * 0.5f);
		}
		else
			m_PlayerStone->Update(0.0f, -rotateSpeed);
	}
	if (input->IsDPressed()) {
		m_PlayerStone->Move(moveSpeed, 0.0f, 0.0f);  // X축 증가
		if (m_PlayerStone->CheckCollision(m_WallStone))
		{
			m_PlayerStone->Move(-moveSpeed, 0.0f, 0.0f);
			m_WallStone->Move(-moveSpeed * 0.5f, 0.0f, 0.0f);
			m_WallStone->Update(0.0f, -rotateSpeed * 0.5f);
		}
		else
			m_PlayerStone->Update(0.0f, rotateSpeed);
	}
	if (input->IsWPressed()) {
		m_PlayerStone->Move(0.0f, 0.0f, moveSpeed);  // Z축 증가 (앞으로)
	}
	if (input->IsSPressed()) {
		m_PlayerStone->Move(0.0f, 0.0f, -moveSpeed); // Z축 감소 (뒤로)
	}


	// Handle the input.
	keyDown = input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = input->IsZPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = input->IsXPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	// 뷰포트 position/rotation.
	pos = m_Position->GetPosition();
	rot = m_Position->GetRotation();
	//m_Position->GetPosition(posX, posY, posZ);
	//m_Position->GetRotation(rotX, rotY, rotZ);

	// 카메라 position
	m_Camera->SetPosition(pos.x, pos.y, pos.z);
	m_Camera->SetRotation(rot.x, rot.y, rot.z);

	if (input->IsF1Toggled())
	{
		//m_displayUI = !m_displayUI;
	}

	int wheelDelta = input->GetMouseWheelDelta();
	if (wheelDelta != 0)
	{
		m_Camera->Zoom((float)wheelDelta);
	}

	if (input->IsF2Toggled())
	{
		m_wireFrame = !m_wireFrame;
	}

	if (input->IsF3Toggled())
	{
		m_cellLines = !m_cellLines;
	}

	if (input->IsF4Toggled())
	{
		m_heightLocked = !m_heightLocked;
	}


	return;
} // HandleMovementInput
