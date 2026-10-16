# BedrockGuiDemo — Minecraft内オーバーレイ

Minecraft Bedrock Windows x64の既存画面に直接GUIを描画するDLLサンプルです。
独立したウィンドウは生成しません。

SDK: https://github.com/gamelist1990/BedrockDynamicSDK

GUIはDear ImGuiのドッキング対応版を使用。ダッシュボード、サイドバー、
設定、アクセントカラー、自由描画、グラフ、ボタン等を表示します。

## 仕組み

- Optional SDK target: bedrock::gui_overlay
- DX11: MinecraftのIDXGISwapChain::PresentへImGui描画
- DX12: IDXGISwapChain3::PresentとExecuteCommandListsからD3D12キューを取得し
  D3D11On12を利用してImGuiを合成
- リサイズ時のGPUバックバッファと描画ターゲット再作成
- F10でGUI表示ON/OFF
- WndProcからImGuiへ入力を転送（ゲーム内マウスロックは実機調整が必要）
- Minecraft内部のActorやClientInstanceの不確かなABIに依存しない

**注意:** Minecraft 1.26.52.3 + GDK Protonの実機表示・入力確認は
まだ未実施。ビルド成功だけで描画成功とは判定しません。

## ビルド

Visual Studio 2022 / CMake:

    git clone https://github.com/gamelist1990/BedrockGuiDemo.git
    cd BedrockGuiDemo
    cmake -S . -B build -A x64
    cmake --build build --config Release

DLL: build/Release/BedrockGuiOverlay.dll。
GitHub Actionsからダウンロードできます。

Minecraftプロセス起動後に、使用者のDLLローダーからロードします。
ワーカースレッドで描画フックを作成。**独立GUIウィンドウは開きません**。

F10で表示切替。BedrockGuiOverlay_Show(BOOL)で手動表示切替、
BedrockGuiOverlay_Active()で稼働状態確認、
BedrockGuiOverlay_Stop()でフック解除可能。
フック稼働中はDLLをアンロードしないでください。

## 使用例

    #include <bedrock/gui.hpp>
    #include <bedrock/gui_overlay.hpp>

    void draw(void*) {
        bedrock::gui::Window w("My Client");
        if (w) {
            static bool enabled = true;
            bedrock::gui::toggle("Enabled", enabled);
        }
    }
    // DLLロード後のワーカースレッドから:
    std::string error;
    bedrock::gui::Overlay::install(&draw, nullptr, error);

SDKのGUI機能はデフォルトOFF。
BEDROCK_SDK_ENABLE_GUI=ONを有効化した時だけDear ImGuiを取得・ビルド。

GUIは src/main.cpp にあります。
BetterRenderDragonで使用されているDX11/DX12・D3D11On12方式を参考にした
独自実装で、AGPLのコードはコピーしていません。
