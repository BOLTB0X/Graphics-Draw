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

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	bool IsMousePressed();

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
}; // Input

#endif