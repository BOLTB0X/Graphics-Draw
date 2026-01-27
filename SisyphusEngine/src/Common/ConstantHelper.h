#pragma once
#include <iostream>
#include <string>


namespace ConstantHelper {
    // 화면 설정
    const bool FULL_SCREEN = false;
    const bool VSYNC_ENABLED = true;

    inline int SCREEN_WIDTH = 800;
    inline int SCREEN_HEIGHT = 600;

    // 클리핑 평면
    const float SCREEN_DEPTH = 1000.0f;
    const float SCREEN_NEAR = 0.1f;
}


namespace ConstantHelper { // 모델 및 텍스처
    const std::string ASSET_PATH = "assets/";
    const std::string NOISE_PATH = ASSET_PATH + "Cloud/noise.png";
    const std::string BLUE_NOISE_PATH = ASSET_PATH + "Cloud/blue-noise.png";
}


namespace ConstantHelper { // HLSL
    const std::wstring DEFAULT_VS = L"HLSL/DefaultVS.hlsl";
    const std::wstring QUAD_VS = L"HLSL/QuadVS.hlsl";
    const std::wstring CUBE_VS = L"HLSL/CubeVS.hlsl";

    const std::wstring CLOUD_PS = L"HLSL/CloudPS.hlsl";
    const std::wstring SUN_PS = L"HLSL/SunPS.hlsl";
    const std::wstring BICUBIC_PS = L"HLSL/BicubicPS.hlsl";
    const std::wstring SKY_PS = L"HLSL/SkyPS.hlsl";
};