////////////////////////////////////////////////////////////////////////////////
// Filename: Input.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUT_H_
#define _INPUT_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//////////////
// INCLUDES //
//////////////
#include <dinput.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: Input
////////////////////////////////////////////////////////////////////////////////
class Input
{
public:
	Input();
	Input(const Input&);
	~Input();

	bool Init(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	/// <summary>
	/// 카메라,포지션 관련
	/// </summary>
	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	bool IsMousePressed();
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();

	bool IsAPressed();
	bool IsZPressed();
	bool IsWPressed();
	bool IsSPressed();
	bool IsDPressed();
	bool IsXPressed();

	bool IsPgUpPressed();
	bool IsPgDownPressed();

	bool IsF1Toggled();
	bool IsF2Toggled();
	bool IsF3Toggled();
	bool IsF4Toggled();

	int GetMouseWheelDelta();

	/// <summary>
	/// Actors 관련
	/// </summary>
	bool IsLeftArrowPressed();
	bool IsRightArrowPressed();


private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight, m_mouseX, m_mouseY;

	bool m_F1_released, m_F2_released, m_F3_released, m_F4_released;
}; // Input

#endif