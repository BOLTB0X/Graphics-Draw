////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "FrameworkSystem.h"

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    FrameworkSystem* System;
    bool result;

	System = new FrameworkSystem;
	result = System->Init();

    if (result)
    {
        System->Run();
    }

	System->Shutdown();
	delete System;
	System = 0;

    return 0;
} // wWinMain
