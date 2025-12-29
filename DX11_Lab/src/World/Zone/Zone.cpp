////////////////////////////////////////////////////////////////////////////////
// Filename: Zone.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Zone.h"


Zone::Zone()
	: m_Camera(nullptr), 
	m_Position(nullptr), 
	m_Terrain(nullptr),
	m_Light(nullptr),
	m_PlayerStone(nullptr), 
	m_WallStone(nullptr),
	m_wireFrame(false)

{
} // Zone


Zone::Zone(const Zone& other)
	: m_Camera(nullptr), 
	m_Position(nullptr), 
	m_Terrain(nullptr),
	m_Light(nullptr), 
	m_PlayerStone(nullptr), 
	m_WallStone(nullptr),
	m_wireFrame(false)
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
	if (!m_Camera)
	{
		return false;
	}
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	m_Position = new Position;
	if (!m_Position)
	{
		return false;
	}
	m_Position->SetPosition(128.0f, 10.0f, -10.0f);
	m_Position->SetRotation(0.0f, 0.0f, 0.0f);

	m_Terrain = new Terrain;
	if (!m_Terrain)
	{
		return false;
	}
	result = m_Terrain->Init(Direct3D->GetDevice(), "assets/Terrain/setup.txt");
	if (!result)
	{
		MessageBoxW(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}
	
	// 조명
	m_Light = new Light;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// 조종할 돌
	m_PlayerStone = new Stone;
	m_PlayerStone->Init(sharedModel, 130.0f, 2.0f, 0.0f, 1.0f);

	// 벽이 될 돌
	m_WallStone = new Stone;
	m_WallStone->Init(sharedModel, 126.0f, 2.0f, 0.0f, 1.0f);

	m_wireFrame = true;

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

	if (m_Light) {
		delete m_Light;
		m_Light = 0;
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

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	return;
} // Shutdown


bool Zone::Frame(D3DRenderer* Direct3D, Input* input, ShaderManager* shaderManager, float frameTime, int fps)
{
	bool result;
	float posX, posY, posZ, rotX, rotY, rotZ;


	// 프레임 입력 처리를 수행
	HandleMovementInput(input, frameTime);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	return true;
} // Frame


bool Zone::Render(D3DRenderer* renderer, ShaderManager* shaderManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;


	//  카메라 위치를 기반으로 뷰 행렬을 생성
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드 행렬, 뷰 행렬, 프로젝션 행렬을 가져옴
	renderer->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	renderer->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	renderer->GetOrthoMatrix(orthoMatrix);

	if (m_wireFrame)
	{
		renderer->EnableWireframe();
	}

	// 컬러 셰이더를 사용하여 지형 격자를 렌더링
	m_Terrain->Render(renderer->GetDeviceContext());
	result = shaderManager->RenderColorShader(renderer->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix);
	if (!result)
	{
		return false;
	}

	result = m_PlayerStone->Render(renderer->GetDeviceContext(), shaderManager,
		viewMatrix, projectionMatrix, m_Light);
	if (!result)
		return false;

	result = m_WallStone->Render(renderer->GetDeviceContext(), shaderManager,
		viewMatrix, projectionMatrix, m_Light);
	if (!result)
		return false;


	if (m_wireFrame)
	{
		renderer->DisableWireframe();
	}

	return true;
} // Render


void Zone::HandleMovementInput(Input* input, float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;

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
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// 카메라 position
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Determine if the user interface should be displayed or not.
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

	return;
} // HandleMovementInput

