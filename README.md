# BedrockGuiDemo

A native Dear ImGui + DirectX11 window showcasing the **optional**
[BedrockDynamicSDK](https://github.com/gamelist1990/BedrockDynamicSDK)
`bedrock::gui` and `bedrock::gui_host` APIs.

- Dashboard, navigation, rounded cards, dark theme, custom accent colors
- Panels, tabs, switches, sliders, inputs, progress bars, gradient drawing
- Smooth animations using delta time, sample performance timeline
- Fully adjustable Dear ImGui widgets and draw lists via `ImGui::` directly
- No game process access, injection, hooks or game-version dependencies

## Build

On Windows with Visual Studio 2022 and CMake:

```powershell
git clone https://github.com/gamelist1990/BedrockGuiDemo.git
cd BedrockGuiDemo
cmake -S . -B build -A x64
cmake --build build --config Release
.\build\Release\BedrockGuiDemo.exe
```

CMake **FetchContent** fetches the SDK from GitHub when building. The SDK
only fetches Dear ImGui when `BEDROCK_SDK_ENABLE_GUI=ON`.

## Making your own client GUI

```cpp
#include <bedrock/gui.hpp>
#include <bedrock/gui_host.hpp>

std::string error;
bedrock::gui::HostOptions options;
bedrock::gui::GuiHost::run(options, [] {
    bedrock::gui::Window window("Client settings");
    if (window) {
        static bool enabled = false;
        bedrock::gui::toggle("Enable", enabled);
    }
}, error);
```

`GuiHost::run` blocks on the calling GUI thread. Run outside `DllMain`.
The SDK never automatically starts a worker thread or intercepts the game.

**Important:** This is a **separate Win32 tool window**, not yet an ImGui layer
drawn *inside* Minecraft. Rendering directly in Minecraft's swap chain
requires a version-compatible DirectX backend adapter and real in-game testing.
This demo intentionally does not claim that capability.

The UI widgets and theming can be reused in a future in-game render adapter.
