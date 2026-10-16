cmake_minimum_required(VERSION 3.24)
project(BedrockGuiDemo LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

# GUI dependency is OPT-IN. SDK without BEDROCK_SDK_ENABLE_GUI stays tiny.
set(BEDROCK_SDK_ENABLE_GUI ON CACHE BOOL "" FORCE)
set(BEDROCK_SDK_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BEDROCK_SDK_REF "main" CACHE STRING "Tag or commit of BedrockDynamicSDK")
FetchContent_Declare(
    bedrock_sdk
    GIT_REPOSITORY https://github.com/gamelist1990/BedrockDynamicSDK.git
    GIT_TAG ${BEDROCK_SDK_REF}
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(bedrock_sdk)

if(WIN32)
    # In-game DXGI DLL, NOT a separate Win32 window.
    add_library(BedrockGuiOverlay SHARED src/main.cpp)
    target_link_libraries(BedrockGuiOverlay PRIVATE bedrock::gui_overlay)
    if(MSVC)
        set_property(TARGET BedrockGuiOverlay PROPERTY MSVC_RUNTIME_LIBRARY
            "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()
