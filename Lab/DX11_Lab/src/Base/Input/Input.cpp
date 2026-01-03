////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Input.h"
#include <cstring>

Input::Input()
	: m_directInput(nullptr),
	m_keyboard(nullptr),
	m_mouse(nullptr),
	m_screenWidth(0),
	m_screenHeight(0),
	m_mouseX(0),
	m_mouseY(0),
	m_F1_released(true),
	m_F2_released(true),
	m_F3_released(true),
	m_F4_released(true)
{
	m_mouseState = {};
	memset(m_keyboardState, 0, sizeof(m_keyboardState));
} // Input


Input::Input(const Input& other)
	: m_directInput(nullptr),
	m_keyboard(nullptr),
	m_mouse(nullptr),
	m_screenWidth(0),
	m_screenHeight(0),
	m_mouseX(0),
	m_mouseY(0),
	m_F1_released(true),
	m_F2_released(true),
	m_F3_released(true),
	m_F4_released(true)
{
	m_mouseState = {};
	memset(m_keyboardState, 0, sizeof(m_keyboardState));
} // Input


Input::~Input()
{
} // ~Input


bool Input::Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;


	// 마우스 커서 위치에 사용할 화면 크기를 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 화면에서 마우스의 위치를 ​​초기화
	m_mouseX = 0;
	m_mouseY = 0;

	// 메인 직접 입력 인터페이스를 초기화
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드용 직접 입력 인터페이스를 초기화
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 데이터 형식을 설정
	// 이 경우 키보드이므로 미리 정의된 데이터 형식을 사용할 수 있음
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드의 협업 수준을 다른 프로그램과 공유하지 않도록 설정
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// 마우스에 대한 직접 입력 인터페이스를 초기화
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 미리 정의된 마우스 데이터 형식을 사용하여 마우스의 데이터 형식을 설정
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스의 협력 수준을 다른 프로그램과 공유하도록 설정
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	m_F1_released = true;
	m_F2_released = true;

	return true;
} // Init


void Input::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
} // Shutdown


bool Input::Frame()
{
	bool result;


	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// 마우스와 키보드의 변경 사항을 처리
	ProcessInput();

	return true;
} // Frame


bool Input::ReadKeyboard()
{
	HRESULT result;


	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
} // ReadKeyboard


bool Input::ReadMouse()
{
	HRESULT result;


	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
} // ReadMouse


void Input::ProcessInput()
{
	// 프레임 동안 마우스 위치 변화에 따라 마우스 커서의 위치를 ​​업데이트
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 마우스 위치가 화면 너비나 높이 제한.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
} // ProcessInput


bool Input::IsEscapePressed()
{
	//키보드 상태에 대해 비트 단위 AND 연산을 수행하여 ESC 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
} // IsEscapePressed


void Input::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
} // GetMouseLocation


bool Input::IsMousePressed()
{
	// 왼쪽 마우스 버튼의 상태를 확인
	if (m_mouseState.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
} // IsMousePressed


bool Input::IsLeftPressed()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
} // IsLeftPressed


bool Input::IsRightPressed()
{
	// 키보드 상태를 비트 단위로 AND 연산하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
} // IsRightPressed


bool Input::IsUpPressed()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
} // IsUpPressed


bool Input::IsDownPressed()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
} // IsDownPressed


bool Input::IsAPressed()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
} // IsAPressed


bool Input::IsZPressed()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_Z] & 0x80)
	{
		return true;
	}

	return false;
} // IsZPressed


bool Input::IsWPressed() { 
	return (m_keyboardState[DIK_W] & 0x80); 
} //  IsWPressed


bool Input::IsSPressed() {
	return (m_keyboardState[DIK_S] & 0x80);
} // IsSPressed


bool Input::IsDPressed() {
	return (m_keyboardState[DIK_D] & 0x80);
} // IsDPressed


bool Input::IsXPressed() {
	return (m_keyboardState[DIK_X] & 0x80);
} // IsXPressed



bool Input::IsPgUpPressed()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_PGUP] & 0x80)
	{
		return true;
	}

	return false;
} // IsPgUpPressed


bool Input::IsPgDownPressed()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_PGDN] & 0x80)
	{
		return true;
	}

	return false;
} // IsPgDownPressed


bool Input::IsF1Toggled()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_F1] & 0x80)
	{
		if (m_F1_released)
		{
			m_F1_released = false;
			return true;
		}
	}
	else
	{
		m_F1_released = true;
	}

	return false;
} // IsF1Toggled


bool Input::IsF2Toggled()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_F2] & 0x80)
	{
		if (m_F2_released)
		{
			m_F2_released = false;
			return true;
		}
	}
	else
	{
		m_F2_released = true;
	}

	return false;
} // IsF2Toggled


bool Input::IsF3Toggled()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_F3] & 0x80)
	{
		if (m_F3_released)
		{
			m_F3_released = false;
			return true;
		}
	}
	else
	{
		m_F3_released = true;
	}

	return false;
} // IsF3Toggled


bool Input::IsF4Toggled()
{
	// 키보드 상태에 대해 비트 단위 AND 연산을 수행하여 해당 키가 현재 눌려 있는지 확인
	if (m_keyboardState[DIK_F4] & 0x80)
	{
		if (m_F3_released)
		{
			m_F3_released = false;
			return true;
		}
	}
	else
	{
		m_F3_released = true;
	}

	return false;
} // IsF4Toggled


int Input::GetMouseWheelDelta()
{
	return m_mouseState.lZ;
} // GetMouseWheelDelta


bool Input::IsLeftArrowPressed() {
	if (m_keyboardState[DIK_LEFT] & 0x80) return true;
	return false;
} // IsLeftArrowPressed


bool Input::IsRightArrowPressed() {
	if (m_keyboardState[DIK_RIGHT] & 0x80) return true;
	return false;
} // IsRightArrowPressed