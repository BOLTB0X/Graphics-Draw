#pragma once
#include <iostream>
#include <string>


namespace EngineSettings {
    // 화면 설정
    const bool FULL_SCREEN = false;
    const bool VSYNC_ENABLED = true;

    inline int SCREEN_WIDTH = 800;
    inline int SCREEN_HEIGHT = 600;

    // 클리핑 평면
    const float SCREEN_DEPTH = 1000.0f;
    const float SCREEN_NEAR = 0.1f;
}


namespace EngineSettings { // 모델 및 텍스처
    const std::string ASSET_PATH = "assets/";
    const std::string NOISE_PATH = ASSET_PATH + "Cloud/noise.png";
}


namespace EngineSettings { // HLSL
    const std::wstring DEFAULT_VS = L"HLSL/DefaultVS.hlsl";
    const std::wstring CLOUD_PS = L"HLSL/CloudPS.hlsl";
    const std::wstring SUN_PS = L"HLSL/SunPS.hlsl";
};