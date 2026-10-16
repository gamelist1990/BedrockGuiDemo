#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include <bedrock/gui_overlay.hpp>
#include <bedrock/gui.hpp>
#include <imgui.h>
#include <array>
#include <cmath>
#include <string>

using bedrock::gui::Color;
using bedrock::gui::Vec2;
namespace ui = bedrock::gui;
namespace draw = bedrock::gui::draw;

struct DemoState {
    int page = 0;
    bool enabled = true;
    bool modern = true;
    bool notifications = true;
    float volume = 0.72f;
    float radius = 12.f;
    float glass = 0.92f;
    Color accent{0.45f, 0.59f, 1.0f, 1.0f};
    char project[64] = "My Minecraft Client";
    float animation = 0.f;
    float simulatedFps = 144.f;
    std::array<float,120> graph{};
    int graphOffset = 0;
    std::string lastAction = "GUI backend initialized";
    void tick() {
        animation = ui::smooth(animation, enabled ? 1.f : 0.f,
                               9.f, ImGui::GetIO().DeltaTime);
        graph[graphOffset++ % graph.size()] =
            118.f + 15.f * std::sin(static_cast<float>(ImGui::GetTime()) * 2.0f);
    }
};

static void background() {
    const auto p = ImGui::GetMainViewport();
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    const Vec2 lo = p->Pos;
    const Vec2 hi{lo.x + p->Size.x, lo.y + p->Size.y};
    // Do NOT cover Minecraft's rendered scene with an opaque rectangle.
    drawList->AddRectFilled(lo,hi,draw::rgba(.017f,.023f,.045f,.23f));
    drawList->AddCircleFilled({hi.x - 120,lo.y + 25}, 150,
                              draw::rgba(.19f,.25f,.42f,.16f),60);
}

static void indicator(const char* label, const char* value, Vec2 size) {
    ui::Panel panel(label,size);
    if (!panel) return;
    ui::muted(label);
    ImGui::SetWindowFontScale(1.32f);
    ui::text(value);
    ImGui::SetWindowFontScale(1.0f);
}

static void performanceGraph(const DemoState& s) {
    auto p = ImGui::GetCursorScreenPos();
    const float width = ImGui::GetContentRegionAvail().x;
    const float height = 116.f;
    ImDrawList* d = ImGui::GetWindowDrawList();
    d->AddRectFilled(p, {p.x + width,p.y+height},
        draw::rgba(.070f,.095f,.16f),10.f);
    for (int i=1;i<4;++i) {
        const float y = p.y + i*height/4.f;
        d->AddLine({p.x+8,y},{p.x+width-8,y},draw::rgba(.28f,.34f,.49f,.20f));
    }
    for (int i=1;i<119;++i) {
        const int idx1=(s.graphOffset+i-1) % s.graph.size();
        const int idx2=(s.graphOffset+i) % s.graph.size();
        const float x1=p.x + 10.f + (i-1)*(width-20.f)/118.f;
        const float x2=p.x + 10.f + i*(width-20.f)/118.f;
        const float y1=p.y + height-10.f - (s.graph[idx1]-85.f)*1.35f;
        const float y2=p.y + height-10.f - (s.graph[idx2]-85.f)*1.35f;
        d->AddLine({x1,y1},{x2,y2},draw::rgba(.49f,.68f,1.f),2.2f);
    }
    ImGui::Dummy({width,height});
}

static void dashboard(DemoState& s) {
    ui::heading("Overview",1.42f);
    ui::muted("A native, fully customizable UI for your Bedrock client");
    ui::space(6);
    indicator("CLIENT",s.enabled ? "ACTIVE" : "DISABLED",{205,96});
    ImGui::SameLine();
    indicator("RENDERER","DirectX 11",{205,96});
    ImGui::SameLine();
    indicator("DATA SOURCE","Optional",{205,96});
    ui::space(10);
    {
        ui::Panel panel("performance", {0,210});
        if(panel) {
            ui::heading("Frame timeline",1.1f);
            ui::muted("Visual demonstration - synthetic data");
            performanceGraph(s);
        }
    }
    ui::space(7);
    {
        ui::Panel panel("quick actions",{0,170});
        if(panel) {
            ui::heading("Quick actions",1.1f);
            ui::toggle("Enable client",s.enabled);
            ImGui::SameLine(220);
            ui::toggle("Toast notifications",s.notifications);
            ui::slider("Interface opacity",s.glass,0.55f,1.f);
            if(ui::button("Apply theme",{150,32})) {
                ui::Theme::dark(s.accent);
                s.lastAction="Theme applied";
            }
        }
    }
}

static void widgets(DemoState& s) {
    ui::heading("Component lab",1.40f);
    ui::muted("Layout, animation, controls and low-level drawing");
    ui::space(10);
    ui::Panel container("controls", {0,260});
    if (!container) return;
    ui::toggle("Enable UI effects",s.modern);
    ui::slider("Corner radius",s.radius,0.f,25.f);
    ui::slider("Master volume",s.volume,0.f,1.f);
    ui::colorPicker("Accent color",s.accent);
    ui::input("Client name",s.project,sizeof(s.project));
    ui::progress(s.volume);
}

static void customCanvas(DemoState& s) {
    ui::heading("Custom canvas",1.35f);
    ui::muted("Use ImGui draw lists or the SDK's draw helpers");
    ui::space(12);
    {
        ui::Panel panel("canvas", {0,220});
        if(panel) {
            Vec2 p=ImGui::GetCursorScreenPos();
            const Vec2 a{p.x+14,p.y+14};
            const Vec2 b{p.x+ImGui::GetContentRegionAvail().x-16,p.y+125};
            draw::gradient(a,b,draw::rgba(.22f,.39f,.88f),
                draw::rgba(.38f,.30f,.83f),draw::rgba(.22f,.17f,.51f),
                draw::rgba(.16f,.30f,.62f));
            draw::rect(a,b,draw::rgba(.65f,.74f,1.f,.9f),s.radius,1.2f);
            draw::circle({a.x+28,a.y+34},9,draw::rgba(.94f,.95f,1.f));
            draw::text({a.x+54,a.y+24},draw::rgba(1.f,1.f,1.f),
                       "Custom gradients / primitives");
            ImGui::Dummy({0,145});
            ui::muted("Animations, icons, images and shaders can be added by your renderer.");
        }
    }
    ui::space(12);
    if(ui::button("Trigger animation",{185,38})) {
        s.animation=0.f;
        s.enabled=true;
        s.lastAction="Animation triggered";
    }
    ui::sameLine();
    ui::muted("Client controls remain independent of the SDK core.");
}

static void render(DemoState& s) {
    s.tick();
    background();
    const auto& viewport = *ImGui::GetMainViewport();
    const Vec2 pageSize{
        std::clamp(viewport.WorkSize.x * 0.73f, 800.f, 1060.f),
        std::clamp(viewport.WorkSize.y * 0.79f, 570.f, 740.f)};
    ImGui::SetNextWindowPos({
        viewport.WorkPos.x + (viewport.WorkSize.x-pageSize.x)*0.5f,
        viewport.WorkPos.y + (viewport.WorkSize.y-pageSize.y)*0.5f
    },ImGuiCond_Always);
    ImGui::SetNextWindowSize(pageSize,ImGuiCond_Always);
    ui::Window shell("##shell",nullptr,
        ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|
        ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBringToFrontOnFocus);
    {
        ui::Panel sidebar("NAVIGATION",{205,0},ImGuiChildFlags_Borders);
        if(sidebar) {
            ui::heading("BEDROCK",1.48f);
            ui::muted("Dynamic SDK / GUI");
            ui::space(21);
            const char* pages[]={"Overview", "Components", "Canvas"};
            for (int i=0;i<3;++i) {
                if(ImGui::Selectable(pages[i],s.page==i,0,{185,43}))
                    s.page=i;
            }
            ui::space(210);
            ui::separator();
            ui::muted("In-game overlay sample");
            ui::text("Dear ImGui / RenderDragon");
        }
    }
    ui::sameLine(0,16);
    {
        ui::Panel content("CONTENT",{0,0},ImGuiChildFlags_None);
        if(content) {
            if(s.page==0) dashboard(s);
            if(s.page==1) widgets(s);
            if(s.page==2) customCanvas(s);
            ui::space(13);
            ui::separator();
            ui::muted(s.lastAction);
        }
    }
    // Optional, unobtrusive custom replacement for the bottom-right version
    // label. This visually overlays text without modifying the actual game UI.
    const char* hudLabel = "BedrockDynamicSDK  |  GUI Preview";
    const Vec2 hudSize=ImGui::CalcTextSize(hudLabel);
    const Vec2 loc{
        viewport.WorkPos.x+viewport.WorkSize.x-hudSize.x-27.f,
        viewport.WorkPos.y+viewport.WorkSize.y-hudSize.y-18.f
    };
    auto* hud=ImGui::GetForegroundDrawList();
    hud->AddRectFilled({loc.x-10.f,loc.y-6.f},
        {loc.x+hudSize.x+10.f,loc.y+hudSize.y+6.f},
        draw::rgba(.065f,.080f,.135f,.88f),7.0f);
    hud->AddText(loc,draw::rgba(.82f,.89f,1.0f),hudLabel);
}

#ifdef _WIN32
namespace {
    DemoState g_demo;
    DWORD WINAPI initializeOverlay(void*) {
        std::string error;
        const bool okay = bedrock::gui::Overlay::install(
            [](void* context) { render(*static_cast<DemoState*>(context)); },
            &g_demo, error);
        if (!okay) {
            OutputDebugStringA(("BedrockGuiOverlay: " + error + "\n").c_str());
        } else {
            OutputDebugStringA(
                "BedrockGuiOverlay: in-game renderer installed. Press F10 to toggle.\n");
        }
        return okay ? 0 : 1;
    }
}
extern "C" __declspec(dllexport) void BedrockGuiOverlay_Show(BOOL value) {
    bedrock::gui::Overlay::show(value != 0);
}
extern "C" __declspec(dllexport) BOOL BedrockGuiOverlay_Active() {
    return bedrock::gui::Overlay::running() ? TRUE : FALSE;
}
extern "C" __declspec(dllexport) void BedrockGuiOverlay_Stop() {
    bedrock::gui::Overlay::shutdown();
}
BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        HANDLE thread = CreateThread(nullptr, 0, &initializeOverlay,
                                     nullptr, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
#endif
