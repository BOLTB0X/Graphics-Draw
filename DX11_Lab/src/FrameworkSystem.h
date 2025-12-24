////////////////////////////////////////////////////////////////////////////////
// Filename: FrameworkSystem.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN


//////////////
// INCLUDES //
//////////////
#include <windows.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Input/Input.h"
#include "Application/App.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FrameworkSystem
////////////////////////////////////////////////////////////////////////////////
class FrameworkSystem
{
public:
	FrameworkSystem();
	FrameworkSystem(const FrameworkSystem&);
	~FrameworkSystem();

	bool Init();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_Input;
	App* m_App;
}; // FrameworkSystem


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static FrameworkSystem* ApplicationHandle = 0;


#endif