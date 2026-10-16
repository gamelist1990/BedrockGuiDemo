// Native, interactive in-game Minecraft UI. No separate Windows GUI window.
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include <bedrock/gui_overlay.hpp>
#include <bedrock/gui.hpp>
#include <imgui.h>
#include <array>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstdio>

namespace ui = bedrock::gui;
namespace draw = bedrock::gui::draw;
using ui::Vec2;
using ui::Color;

struct DemoState {
    int page=0;
    bool autoSprint=true;
    bool hud=true;
    bool notifications=true;
    bool animations=true;
    bool shadow=true;
    float uiScale=1.0f;
    float opacity=0.94f;
    float accentHue=0.62f;
    float easing=0.f;
    float activity=0.f;
    float graph[96]{};
    int graphHead=0;
    Color accent{.43f,.59f,1.0f,1.0f};
    char clientName[80]="Bedrock Dynamic Client";
    std::string status="Overlay ready. F10 to toggle.";
    void frame() {
        const float dt=std::min(ImGui::GetIO().DeltaTime,.1f);
        easing=ui::smooth(easing,animations?1.f:0.f,8.f,dt);
        activity+=dt;
        if (activity>=.075f) {
            activity=0.f;
            graph[graphHead++ % 96]=.60f + .22f*std::sin((float)ImGui::GetTime()*1.1f)
                                      + .13f*std::sin((float)ImGui::GetTime()*2.65f);
        }
    }
};
static DemoState state;

static void title(const char* name,const char* caption) {
    ImGui::SetWindowFontScale(1.48f);
    ui::text(name);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text,ImVec4{.56f,.62f,.75f,1.f});
    ui::text(caption);
    ImGui::PopStyleColor();
    ui::space(10);
}

static bool navButton(const char* id,const char* name,bool chosen) {
    ImGui::PushStyleColor(ImGuiCol_Header,
                          chosen ? ImVec4{.22f,.29f,.49f,.94f} : ImVec4{0,0,0,0});
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,ImVec4{.23f,.31f,.52f,.9f});
    const bool clicked=ImGui::Selectable(name,chosen,0,Vec2{0,48.f});
    ImGui::PopStyleColor(2);
    return clicked;
}

static void smallText(const char* name) {
    ImGui::PushStyleColor(ImGuiCol_Text,ImVec4{.54f,.61f,.73f,1.f});
    ui::text(name);
    ImGui::PopStyleColor();
}

static void infoCard(const char* id,const char* metric,
                     const char* detail,Vec2 size,ImU32 accent) {
    ImGui::BeginChild(id,size,ImGuiChildFlags_Borders);
    Vec2 min=ImGui::GetWindowPos();
    auto* list=ImGui::GetWindowDrawList();
    list->AddRectFilled({min.x+15,min.y+16},{min.x+19,min.y+45},accent,3.f);
    ImGui::Indent(17);
    smallText(detail);
    ImGui::SetWindowFontScale(1.36f);
    ui::text(metric);
    ImGui::SetWindowFontScale(1.f);
    ImGui::Unindent(17);
    ImGui::EndChild();
}

static void toggleSetting(const char* label,const char* description,bool& value) {
    const auto start=ImGui::GetCursorScreenPos();
    ImGui::PushID(label);
    ui::text(label);
    smallText(description);
    const float width=ImGui::GetContentRegionAvail().x;
    ImGui::SameLine(width-68.f);
    // Wide hit region with a real ImGui control; toggle has keyboard support.
    ImGui::SetCursorPosY(ImGui::GetCursorPosY()-32.f);
    ImGui::Checkbox("##toggle",&value);
    ImGui::PopID();
    ui::space(12);
}

static void moduleCard(const char* id,const char* titleText,
                       const char* caption,bool& enabled,
                       ImU32 accentColor) {
    ImGui::BeginChild(id,{0,99},ImGuiChildFlags_Borders);
    Vec2 p=ImGui::GetWindowPos();
    ImGui::GetWindowDrawList()->AddRectFilled(
        {p.x+13,p.y+18},{p.x+17,p.y+64},accentColor,4.f);
    ImGui::Indent(15);
    ui::text(titleText);
    smallText(caption);
    ImGui::Unindent(15);
    ImGui::SameLine(ImGui::GetWindowWidth()-71.f);
    ImGui::SetCursorPosY(36.f);
    ImGui::Checkbox("##enabled",&enabled);
    ImGui::EndChild();
}

static void performance(DemoState& s) {
    auto* list=ImGui::GetWindowDrawList();
    const Vec2 p=ImGui::GetCursorScreenPos();
    const float w=ImGui::GetContentRegionAvail().x;
    constexpr float h=132.f;
    list->AddRectFilled(p,{p.x+w,p.y+h},draw::rgba(.065f,.083f,.14f),13.f);
    for(int i=1;i<=3;++i) {
        float y=p.y + i*h/4.f;
        list->AddLine({p.x+12,y},{p.x+w-12,y},draw::rgba(.26f,.34f,.50f,.24f));
    }
    for(int i=1;i<96;++i) {
        int ia=(s.graphHead+i-1)%96;
        int ib=(s.graphHead+i)%96;
        const float x1=p.x+14.f+(i-1)*(w-28.f)/95.f;
        const float x2=p.x+14.f+i*(w-28.f)/95.f;
        const float y1=p.y+h-9.f-s.graph[ia]*(h-22.f);
        const float y2=p.y+h-9.f-s.graph[ib]*(h-22.f);
        list->AddLine({x1,y1},{x2,y2},draw::rgba(.46f,.66f,1.f,.92f),2.3f);
    }
    ImGui::Dummy({w,h});
}

static void overview(DemoState& s) {
    title("Dashboard","Your in-game client workspace");
    const float w=ImGui::GetContentRegionAvail().x;
    const float gap=12.f;
    const float card=(w-gap*2.f)/3.f;
    infoCard("health","CONNECTED","CLIENT STATUS",{card,96},draw::rgba(.30f,.80f,.60f));
    ImGui::SameLine(0,gap);
    infoCard("render","DXGI","RENDER BRIDGE",{card,96},draw::rgba(.45f,.65f,1.f));
    ImGui::SameLine(0,gap);
    infoCard("backend",ui::Overlay::nativeCursorReady()?"READY":"WAITING",
             "MOUSE BRIDGE",{card,96},
             ui::Overlay::nativeCursorReady() ?
                 draw::rgba(.32f,.80f,.60f) : draw::rgba(.95f,.67f,.29f));
    ui::space(12);
    ImGui::BeginChild("modules",{0,275},ImGuiChildFlags_Borders);
    ImGui::SetWindowFontScale(1.20f); ui::text("Modules"); ImGui::SetWindowFontScale(1.f);
    smallText("Simple, independent event-based extensions");
    ui::space(12);
    moduleCard("autosprint","AutoSprint","Forward input shortcut",s.autoSprint,
               draw::rgba(.46f,.66f,1.f));
    ui::space(4);
    moduleCard("hud","Client HUD","Custom on-screen visuals",s.hud,
               draw::rgba(.52f,.86f,.70f));
    ImGui::EndChild();
    ui::space(12);
    ImGui::BeginChild("timeline",{0,210},ImGuiChildFlags_Borders);
    ImGui::SetWindowFontScale(1.16f);ui::text("Performance preview");ImGui::SetWindowFontScale(1.f);
    smallText("Animated example data, not measured FPS");
    ui::space(5);
    performance(s);
    ImGui::EndChild();
}

static void components(DemoState& s) {
    title("Appearance","Fine-tune your client's look and feel");
    ImGui::BeginChild("appearance",{0,0},ImGuiChildFlags_Borders);
    ImGui::SetWindowFontScale(1.2f);ui::text("Interface");ImGui::SetWindowFontScale(1.f);
    smallText("Comfortable spacing, smooth geometry and readable type");
    ui::space(15);
    ImGui::Checkbox("Smooth animations",&s.animations);
    ui::space(5);
    ImGui::Checkbox("Notifications",&s.notifications);
    ui::space(8);
    ImGui::SetNextItemWidth(std::min(430.f,ImGui::GetContentRegionAvail().x));
    ImGui::SliderFloat("Opacity",&s.opacity,.65f,1.f,"%.0f%%",ImGuiSliderFlags_None);
    ui::space(7);
    ImGui::SetNextItemWidth(std::min(430.f,ImGui::GetContentRegionAvail().x));
    ImGui::ColorEdit3("Accent colour",&s.accent.x,ImGuiColorEditFlags_NoInputs);
    ui::space(12);
    smallText("CLIENT IDENTITY");
    ImGui::SetNextItemWidth(std::min(540.f,ImGui::GetContentRegionAvail().x));
    ImGui::InputText("##clientName",s.clientName,sizeof(s.clientName));
    ui::space(17);
    if (ui::button("Apply theme",{175,42})) {
        ui::Theme::dark(s.accent);
        s.status="Appearance changes applied";
    }
    ImGui::SameLine();
    if (ui::button("Reset",{125,42})) {
        s.accent={.43f,.59f,1.0f,1.f};
        s.opacity=.94f;
        s.animations=true;
        ui::Theme::dark(s.accent);
        s.status="Appearance reset";
    }
    ui::space(16);
    ImGui::Separator();
    ui::space(12);
    ImGui::SetWindowFontScale(1.1f);ui::text("Rendering");ImGui::SetWindowFontScale(1.f);
    smallText(ui::Overlay::nativeCursorReady() ?
        "Native ClientInstance cursor: READY" :
        "Native ClientInstance cursor: WAITING (requires a game cursor event)");
    smallText(ui::Overlay::gameMouseBlocked() ?
        "Minecraft click / hotbar blocker: ACTIVE" :
        "Minecraft click / hotbar blocker: FALLBACK (WndProc only)");
    smallText("TrueType font with oversampled anti-aliasing");
    smallText("Frame-time animation interpolation");
    smallText("Native Minecraft swap-chain rendering");
    ImGui::EndChild();
}

static void canvas(DemoState& s) {
    title("Canvas","Build your own interactive widgets and overlays");
    ImGui::BeginChild("customCanvas",{0,320},ImGuiChildFlags_Borders);
    const Vec2 pos=ImGui::GetCursorScreenPos();
    const float width=ImGui::GetContentRegionAvail().x;
    const Vec2 a{pos.x+10.f,pos.y+10.f};
    const Vec2 b{pos.x+width-10.f,pos.y+203.f};
    auto* list=ImGui::GetWindowDrawList();
    list->AddRectFilled(a,b,draw::rgba(.095f,.13f,.23f),15.f);
    const float pulse=.5f+.5f*std::sin((float)ImGui::GetTime()*1.7f);
    list->AddRectFilledMultiColor({a.x+12.f,a.y+12.f},{b.x-12.f,b.y-12.f},
        draw::rgba(.22f,.39f,.78f,.96f),draw::rgba(.35f,.38f,.78f,.9f),
        draw::rgba(.28f,.20f,.53f,.93f),draw::rgba(.14f,.23f,.52f,.95f));
    list->AddCircleFilled({a.x+72.f,a.y+94.f},23.f,
        draw::rgba(.64f,.79f,1.f,.6f+.4f*pulse),48);
    list->AddText({a.x+118.f,a.y+60.f},draw::rgba(1.f,1.f,1.f),
                  "Custom UI drawing");
    list->AddText({a.x+118.f,a.y+94.f},draw::rgba(.80f,.87f,1.f),
                  "Animated primitives, lines and gradients");
    ImGui::Dummy({width,235.f});
    smallText("Rendered directly on Minecraft's scene via DXGI.");
    ImGui::EndChild();
    ui::space(16);
    if (ui::button("Trigger motion",{190,45})) {
        s.easing=0.f;
        s.status="Animation triggered";
    }
    ImGui::SameLine();
    smallText("All transitions are based on frame time.");
}

static void drawUi(DemoState& s) {
    s.frame();
    const ImGuiViewport* viewport=ImGui::GetMainViewport();
    if (!viewport) return;
    // Leave the Minecraft world visible behind the floating client panel.
    const float width=std::min(1060.f,viewport->WorkSize.x-34.f);
    const float height=std::min(740.f,viewport->WorkSize.y-32.f);
    if (width<540.f || height<430.f) return;
    const Vec2 pos{viewport->WorkPos.x+(viewport->WorkSize.x-width)*.5f,
                   viewport->WorkPos.y+(viewport->WorkSize.y-height)*.5f};
    ImGui::SetNextWindowPos(pos,ImGuiCond_Always);
    ImGui::SetNextWindowSize({width,height},ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(s.opacity);
    ui::Window shell("##BedrockClient",nullptr,
        ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoDecoration|
        ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize);
    if (!shell) return;

    // Header
    const float full=ImGui::GetContentRegionAvail().x;
    ImGui::SetWindowFontScale(1.28f);
    ui::text("BEDROCK");
    ImGui::SetWindowFontScale(1.f);
    ImGui::SameLine();
    ImGui::SetCursorPosX(160.f);
    smallText("DYNAMIC SDK   /   IN-GAME CLIENT");
    ImGui::SameLine();
    ImGui::SetCursorPosX(full-92.f);
    if (ui::button("X  F10",{88.f,32.f})) {
        ui::Overlay::show(false);
    }
    ui::space(7);
    ui::separator();
    ui::space(9);

    const float bottom=ImGui::GetContentRegionAvail().y - 54.f;
    ImGui::BeginChild("##Navigation",{203.f,bottom},ImGuiChildFlags_Borders);
    smallText("WORKSPACE");
    ui::space(11);
    if (navButton("nav1","  Overview",s.page==0)) s.page=0;
    if (navButton("nav2","  Appearance",s.page==1)) s.page=1;
    if (navButton("nav3","  UI Canvas",s.page==2)) s.page=2;
    ui::space(16);
    smallText("QUICK ACCESS");
    ui::space(7);
    ImGui::TextColored({.56f,.79f,1.f,1.f},"  [F10]  Hide / Show");
    ImGui::EndChild();
    ImGui::SameLine(0,14.f);

    ImGui::BeginChild("##Content",{0,bottom},ImGuiChildFlags_None);
    if (s.page==0) overview(s);
    else if (s.page==1) components(s);
    else canvas(s);
    ImGui::EndChild();

    // Footer / status line
    ui::space(7);
    ui::separator();
    ImGui::TextColored({.43f,.86f,.66f,1.f},"* ");
    ImGui::SameLine(0,2.f);
    ui::text(s.status);
    ImGui::SameLine();
    ImGui::SetCursorPosX(full-124.f);
    smallText("v1.26 / Preview");
}

#ifdef _WIN32
static DWORD WINAPI initializeOverlay(void*) {
    std::string error;
    bedrock::gui::OverlayOptions options;
    options.initiallyVisible=true;
    options.toggleVirtualKey=VK_F10;
    options.captureInput=true;
    options.nativeCursor=true;
    const bool result=bedrock::gui::Overlay::install(
        [](void* data) { drawUi(*static_cast<DemoState*>(data)); },
        &state,error,options);
    if (!result) {
        OutputDebugStringA(("BedrockGuiOverlay: "+error+"\n").c_str());
    } else {
        OutputDebugStringA("BedrockGuiOverlay ready: F10 to toggle; cursor unlocked\n");
    }
    return result ? 0 : 1;
}
extern "C" __declspec(dllexport) void BedrockGuiOverlay_Show(BOOL show) {
    bedrock::gui::Overlay::show(show != FALSE);
}
extern "C" __declspec(dllexport) BOOL BedrockGuiOverlay_Active() {
    return bedrock::gui::Overlay::running() ? TRUE : FALSE;
}
extern "C" __declspec(dllexport) void BedrockGuiOverlay_Stop() {
    bedrock::gui::Overlay::shutdown();
}
BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,LPVOID) {
    if (reason==DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        HANDLE worker=CreateThread(nullptr,0,initializeOverlay,nullptr,0,nullptr);
        if (worker) CloseHandle(worker);
    }
    return TRUE;
}
#endif
