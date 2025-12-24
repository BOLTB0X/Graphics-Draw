////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "D3DRenderer.h"
#include <cstring>
#include <vector>


D3DRenderer::D3DRenderer()
{
	m_vsync_enabled = false;
	m_videoCardMemory = 0;
	memset(m_videoCardDescription, 0, sizeof(m_videoCardDescription));

	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;

	m_projectionMatrix = XMMatrixIdentity();
	m_worldMatrix = XMMatrixIdentity();
	m_orthoMatrix = XMMatrixIdentity();

	m_viewport = { 0 };
} // D3DRenderer


D3DRenderer::D3DRenderer(const D3DRenderer& other)
{
	m_vsync_enabled = false;
	m_videoCardMemory = 0;
	memset(m_videoCardDescription, 0, sizeof(m_videoCardDescription));

	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;

	m_projectionMatrix = XMMatrixIdentity();
	m_worldMatrix = XMMatrixIdentity();
	m_orthoMatrix = XMMatrixIdentity();

	m_viewport = { 0 };
} // D3DRenderer


D3DRenderer::~D3DRenderer()
{
} // ~D3DRenderer


bool D3DRenderer::Init(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes = 0, i = 0, numerator = 0, denominator = 0;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;

	// (1: 모니터 정보 및 새로고침 빈도(Refresh Rate) 설정
	// ------------------------------------------------
	m_vsync_enabled = vsync; // 수직 동기화(vsync) 설정 저장

	// DXGI 팩토리 생성 (그래픽 인터페이스 관련 객체 생성용)
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// 기본 그래픽 카드(어댑터) 정보 가져오기
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// 기본 출력 장치(모니터) 정보 가져오기
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 모니터에서 지원하는 디스플레이 모드 리스트 개수 파악 (포맷: R8G8B8A8_UNORM)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 디스플레이 모드 리스트 메모리 할당
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 디스플레이 모드 상세 정보 리스트 채우기
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// 현재 화면 설정(너비, 높이)과 일치하는 모드를 찾아 
	// 새로고침 빈도(분자 / 분모) 저장
		for (i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int)screenWidth)
			{
				if (displayModeList[i].Height == (unsigned int)screenHeight)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		} // for - i

	// 1): ------------------------------------------------

	// (2: 그래픽 카드 정보 저장 및 자원 해제
	// ------------------------------------------------
	
	// 그래픽 카드 설명(이름, 메모리 등) 가져오기
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 그래픽 카드 이름을 문자열 배열에 복사하여 저장
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// 사용이 끝난 임시 객체들(디스플레이 리스트, 출력장치, 어댑터, 팩토리) 메모리 해제
	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;
	// 2): ------------------------------------------------

	// (3: 스왑 체인 및 디바이스, 렌더 타겟 뷰 생성
	// ------------------------------------------------

	// 스왑 체인 설정 구조체 초기화
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1; // 백 버퍼 개수
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 수직 동기화 여부에 따른 새로고침 빈도 설정
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백 버퍼의 사용 방식을 설정
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd; // 창이 렌더링될 때 사용할 핸들을 설정
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// 피처 레벨을 DX11로 설정하여 디바이스와 스왑 체인 생성1.
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}
	// 3): ------------------------------------------------

	// (4: 렌더 타겟 뷰 생성 및 Depth버퍼, 뎁스 스텐실 설정
	// ------------------------------------------------
	// 스왑 체인으로부터 백 버퍼 포인터 가져오기
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// 백 버퍼를 렌더 타겟 뷰로 생성 (실제 그리기가 일어나는 곳)
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	backBufferPtr->Release();
	backBufferPtr = 0;

	// depth buffer 초기화
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// depth buffer 설정
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 작성된 설명을 사용하여 깊이 버퍼용 텍스처를 생성
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 스텐실 상태에 대한 설명을 초기화
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스텐실 상태에 대한 설명을 설정
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀이 정면을 향하는 경우 스텐실 작업이 수행
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	// 픽셀이 뒷면을 향하고 있는 경우 스텐실 작업이 수행
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이 스텐실 상태를 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// 깊이 스텐실 상태를 설정
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 깊이 스텐실 뷰를 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 디스크립션을 설정
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// depth stencil view 생성
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더링 파이프라인에 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	// 4): ------------------------------------------------

	// (5: 래스터라이저 상태 및 뷰포트 설정
	// ------------------------------------------------
	// 폴리곤이 어떻게 그리고 어떤 폴리곤이 그려질지를 결정하는 래스터 description을 설정
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 방금 입력한 설명을 바탕으로 래스터라이저 상태를 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// 래스터라이저 상태를 설정
	m_deviceContext->RSSetState(m_rasterState);

	// 렌더링을 위한 뷰포트를 설정
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// viewport 생성
	m_deviceContext->RSSetViewports(1, &m_viewport);
	// 5): ------------------------------------------------

	// (6: 행렬 설정
	// --------------------------------------------------
	
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D 렌더링을 위한 투영 행렬을 생성
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	m_worldMatrix = XMMatrixIdentity();

	// 2D 렌더링을 위한 직교 투영 행렬을 생성
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
} // Init


void D3DRenderer::Shutdown()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
} // Shutdown


void D3DRenderer::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// back buffer 비우기
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// depth buffer 비우기
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
} // BeginScene


void D3DRenderer::EndScene()
{
	// 렌더링이 완료되었으므로 백 버퍼를 화면에 표시
	if (m_vsync_enabled)
	{
		// 화면 새로 고침 빈도에 맞춰 고정
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}

	return;
} // EndScene


ID3D11Device* D3DRenderer::GetDevice()
{
	return m_device;
} // GetDevice


ID3D11DeviceContext* D3DRenderer::GetDeviceContext()
{
	return m_deviceContext;
} // GetDeviceContext


void D3DRenderer::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
} // GetProjectionMatrix


void D3DRenderer::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
} // GetWorldMatrix


void D3DRenderer::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
} // GetOrthoMatrix


void D3DRenderer::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
} // GetVideoCardInfo


void D3DRenderer::SetBackBufferRenderTarget()
{
	// 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더링 파이프라인에 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
} // SetBackBufferRenderTarget


void D3DRenderer::ResetViewport()
{
	// 뷰포트 설정
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
} // ResetViewport