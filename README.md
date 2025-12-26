# Graphics-Draw

![어어어어엇!!](https://github.com/BOLTB0X/Graphics-Draw/blob/main/Img/%EB%98%A5%EA%BC%AC%EC%87%BC.gif?raw=true)

그려그려

## - ing

```
DX11_Lab/
├─ CMakeLists.txt
├─ assets/
│  ├─ Stone
├─ hlsl/
├─ src/
│  ├─ Framework/
│  │  ├─ Camera/
│  │  │  └─ Camera.h/cpp
│  │  ├─ Renderer
│  │  │  └─ D3DRenderer.h/cpp
│  │  ├─ Input/
│  │  └─ Input.h/cpp
│  ├─ Application/
│  │  ├─ App.h/cpp
│  ├─ Actors/
│  │  ├─ Stone.h/cpp
│  ├─ Resources/
│  │  ├─ Model/
│  │  │  ├─ Model.h/cpp
│  │  ├─ Texture/
│  │  │  ├─ Texture.h/cpp
│  │  ├─ Light/
│  │  │  ├─ Light.h/cpp
│  │  ├─ Shaders/
│  │  │  ├─ ShaderManager.h/cpp
│  └─ main.cpp
```

## used 라이브러리 및 툴

- [Github - vcpkg](https://github.com/microsoft/vcpkg?tab=readme-ov-file)

- [Github - imgui](https://github.com/ocornut/imgui)

- [Github - assimp](https://github.com/assimp/assimp)

## 참고

- [Learn microsoft - Direct3D 11](https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/dx-graphics-overviews)

- [모델 출처 - Free3D](https://free3d.com/3d-model/rock-base-lp-26252.html)

- [rastertek - DirectX 11 on Windows 10 Tutorials](https://www.rastertek.com/tutdx11win10.html)

- [유튜브 - 환경 설정 | 홍정모, 프로그래밍 기초, 입문 프로그래밍, 게임 프로그래밍, 프로그래밍 강의, 컴퓨터 그래픽스, 게임 개발, 컴퓨터 작동원리, 코딩](https://www.youtube.com/watch?v=CcFtxKg55fM&list=PLNfg4W25Tapy3qmKqftWndwrIaQRzUmEg&index=9)

- [stack overflow - Basic example for a CMake project definition using DirectX 11 on Windows 10?](https://stackoverflow.com/questions/73986114/basic-example-for-a-cmake-project-definition-using-directx-11-on-windows-10#:~:text=cmake_minimum_required%20(VERSION%203.13)%20project(CMakeDirect3DGame%20DESCRIPTION%20%22CMake%20example,_WIN32_WINNT=0x0601)%20endif()%20endif()%20set_property(DIRECTORY%20PROPERTY%20VS_STARTUP_PROJECT%20$%7BPROJECT_NAME%7D))

- [티스토리 참고  - DirectX 11 최초 실행 환경 세팅하기(나는 뉴비다 개발자편)](https://dev-nicitis.tistory.com/43)

- [티스토리 참고 - DirectX 11 도형 출력(버텍스 버퍼, 버텍스 쉐이더, 픽셀 쉐이더 설정)(pub-repository)](https://pub-repository.tistory.com/97)

- [티스토리 참고 - [DirectX11] 모델 가져오기(Model Import)(유니얼)](https://unialgames.tistory.com/entry/DirectX11ModelImport)

- [티스토리 참고 - FBX SDK, FBX Importer, FBX 파일 불러오기, 3ds Max 등(최디디)](https://dlemrcnd.tistory.com/85)

- [티스토리 참고 - 070 OBB(Oriented Bounding Box) collision(designerd)](https://designerd.tistory.com/entry/DirectX11-070-OBBOriented-Bounding-Box-collision#obb-%EC%B6%A9%EB%8F%8C---%EB%B6%84%EB%A6%AC%EC%B6%95-%EC%9D%B4%EB%A1%A0)

- [티스토리 참고 - Collision과 SphereCollider(유니얼)](https://unialgames.tistory.com/entry/DirectX11CollisionAndSphereCollider)