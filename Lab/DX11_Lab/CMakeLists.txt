cmake_minimum_required(VERSION 3.21)
project(DX11_Lab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Assimp 경로 설정
set(ASSIMP_ROOT "C:/dev/External/assimp-6.0.2-build")

file(GLOB_RECURSE SOURCES 
    "src/*.cpp" 
    "src/*.h"
    "hlsl/*.hlsl"
)
add_executable(DX11_Lab WIN32 ${SOURCES})

# 2. 헤더 포함 경로
target_include_directories(DX11_Lab PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/System"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Base"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/World"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Application"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Framework"
    "${ASSIMP_ROOT}/include"
    "C:/dev"
)

# 3. 라이브러리 및 디렉토리 설정 (변수명 통일)
set(ASSIMP_LIB_NAME "assimp-vc143-mtd")
set(ZLIB_LIB_NAME   "zlibstaticd")
set(ASSIMP_BIN_DIR  "${ASSIMP_ROOT}/bin/Debug")
set(ASSIMP_LIB_DIR  "${ASSIMP_ROOT}/lib/Debug")
set(ZLIB_LIB_DIR    "${ASSIMP_ROOT}/contrib/zlib/Debug")



# 링커에게 라이브러리 탐색 경로 알려주기
target_link_directories(DX11_Lab PRIVATE "${ASSIMP_LIB_DIR}" "${ZLIB_LIB_DIR}")

# 4. 라이브러리 연결 (변수명 수정 완료)
find_package(imgui CONFIG REQUIRED)

target_link_libraries(DX11_Lab
    PRIVATE
    d3d11
    dxgi
    d3dcompiler
    dinput8
    dxguid
    imgui::imgui
    ${ASSIMP_LIB_NAME}   # 변수명 일치 확인
    ${ZLIB_LIB_NAME}     # 변수명 일치 확인
)

add_custom_command(TARGET DX11_Lab POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/hlsl" "$<TARGET_FILE_DIR:DX11_Lab>/hlsl")

add_custom_command(TARGET DX11_Lab POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:DX11_Lab>/assets")

# DLL 파일 복사 (파일명 자동 구성)
add_custom_command(TARGET DX11_Lab POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ASSIMP_BIN_DIR}/${ASSIMP_LIB_NAME}.dll" "$<TARGET_FILE_DIR:DX11_Lab>")