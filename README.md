# Graphics-Draw

![어어어어엇!!](https://github.com/BOLTB0X/Graphics-Draw/blob/main/DemoGIF/%EB%98%A5%EA%BC%AC%EC%87%BC.gif?raw=true)

그려그려

## - ing

<details>
<summary>추구하는 구조 ex</summary>

<!-- 띄우기 -->

| 시지프스 엔진 (Sisyphus) | 유니티 (Unity) | 언리얼 (Unreal Engine) | 역할 |
|------------------------|---------------|-------------------------|------|
| Model                  | Mesh + Material | Static Mesh + Material | 변하지 않는 원본 데이터 (원본 그릇) |
| ModelManager           | Asset Bundle / Project Window | Content Browser / Manager | 리소스를 로드하고 메모리에 유지함 |
| ActorObject            | GameObject    | Actor                   | 게임 월드에 존재하는 개별 객체 |
| Position (Component)   | Transform     | Scene Component          | 객체의 위치, 회전, 크기를 담당 |

<!-- 띄우기 -->

</details>


```
// ~ ing
src/
├─ Common/
├─ System/
├─ Base/
├─ Graphics/
│  ├─ Camera/
│  ├─ Model/
│  ├─ Shader/
├─ Application/
│  ├─ ModelManager/
│  ├─ TextureManager/
│  ├─ ShaderManager/
├─ Framework/
│  │  ├─ IWidget.h
│  │  ├─ WorldObject.h (모든 객체의 부모)
├─ World/
│  ├─ Actors/
```

#### used 라이브러리 및 툴

- [Github - vcpkg](https://github.com/microsoft/vcpkg?tab=readme-ov-file)

- [Github - stb_image](https://github.com/nothings/stb/tree/master)

- [Github - Imgui](https://github.com/ocornut/imgui)

- [Github - assimp](https://github.com/assimp/assimp)

#### [모델 출처 -> sketchfab](https://sketchfab.com/feed)

- [Stone 이용: sketchfab - Ground//Stone Sphere(Madee)](https://sketchfab.com/3d-models/groundstone-sphere-1c0f2b2e213348e6a760743a546dc7a6)

- [Terrain 이용: sketchfab - Snow Mountain(JesterArts_)](https://sketchfab.com/3d-models/snow-mountain-e42d1a231cf54ff3a01dc691c050762c)


## 참고

- [rastertek - DirectX 11 on Windows 10 Tutorials](https://www.rastertek.com/tutdx11win10.html)

- [rastertek - DirectX 11 on Windows 10 Tutorials 2](https://www.rastertek.com/tutdx11s2ter.html)

- [Learn microsoft - Direct3D 11](https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/dx-graphics-overviews)

- [유튜브 - 환경 설정 | 홍정모, 프로그래밍 기초, 입문 프로그래밍, 게임 프로그래밍, 프로그래밍 강의, 컴퓨터 그래픽스, 게임 개발, 컴퓨터 작동원리, 코딩](https://www.youtube.com/watch?v=CcFtxKg55fM&list=PLNfg4W25Tapy3qmKqftWndwrIaQRzUmEg&index=9)

- [stack overflow - Basic example for a CMake project definition using DirectX 11 on Windows 10?](https://stackoverflow.com/questions/73986114/basic-example-for-a-cmake-project-definition-using-directx-11-on-windows-10#:~:text=cmake_minimum_required%20(VERSION%203.13)%20project(CMakeDirect3DGame%20DESCRIPTION%20%22CMake%20example,_WIN32_WINNT=0x0601)%20endif()%20endif()%20set_property(DIRECTORY%20PROPERTY%20VS_STARTUP_PROJECT%20$%7BPROJECT_NAME%7D))

- [티스토리 참고  - DirectX 11 최초 실행 환경 세팅하기(나는 뉴비다 개발자편)](https://dev-nicitis.tistory.com/43)

- [티스토리 참고 - DirectX 11 도형 출력(버텍스 버퍼, 버텍스 쉐이더, 픽셀 쉐이더 설정)(pub-repository)](https://pub-repository.tistory.com/97)

- [티스토리 참고 - [DirectX11] 모델 가져오기(Model Import)(유니얼)](https://unialgames.tistory.com/entry/DirectX11ModelImport)

- [티스토리 참고 - FBX SDK, FBX Importer, FBX 파일 불러오기, 3ds Max 등(최디디)](https://dlemrcnd.tistory.com/85)

- [티스토리 참고 - 070 OBB(Oriented Bounding Box) collision(designerd)](https://designerd.tistory.com/entry/DirectX11-070-OBBOriented-Bounding-Box-collision#obb-%EC%B6%A9%EB%8F%8C---%EB%B6%84%EB%A6%AC%EC%B6%95-%EC%9D%B4%EB%A1%A0)

- [티스토리 참고 - Collision과 SphereCollider(유니얼)](https://unialgames.tistory.com/entry/DirectX11CollisionAndSphereCollider)

- [티스토리 참고 - Terrain 21 - 지형셀(copynull)](https://copynull.tistory.com/324)

- [Velog 참고 - Terrain 지형 툴 연습(hoya1215)](https://velog.io/@hoya1215/Directx11-Terrain-%EC%A7%80%ED%98%95-%ED%88%B4-%EC%97%B0%EC%8A%B5)

- [티스토리 참고 - DirectX11 3D(designerd)](https://designerd.tistory.com/category/%E2%AD%90%20DirectX/DirectX11%20%203D)

- [벨로그 참고 - Imgui 사용법(charlese_note)](https://velog.io/@charlese_note/ImGUI)

- [네이버 블로그 참고 - Dear ImGui를 공부해보자! 시작단계(AROMA)](https://m.blog.naver.com/sweetsmell9/221618574623)