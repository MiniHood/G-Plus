// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include "GUIh.h"
#include "psapi.h"
#include "Controller.h"

class Resize {
public:
    Resize(std::string& s) : s_{ s } { s.resize(len_); }
    ~Resize() { s_.resize(strlen(s_.c_str())); }
    operator char* () { return s_.data(); }
    static constexpr auto len() { return len_ - 1; }
private:
    std::string& s_;
    static constexpr auto len_ = 255;
};

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define WINDOW_TITLE "GPlusV2 - V0 - Developed by mateoss13"

// Themes from https://github.com/Patitotective/ImThemes <3
void HalfLifeStyle()
{
    // Classic Steam style by metasprite from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2862745225429535f, 0.3372549116611481f, 0.2588235437870026f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.2862745225429535f, 0.3372549116611481f, 0.2588235437870026f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.5f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.1372549086809158f, 0.1568627506494522f, 0.1098039224743843f, 0.5199999809265137f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2666666805744171f, 0.2980392277240753f, 0.2274509817361832f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2980392277240753f, 0.3372549116611481f, 0.2588235437870026f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2862745225429535f, 0.3372549116611481f, 0.2588235437870026f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2784313857555389f, 0.3176470696926117f, 0.239215686917305f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2470588237047195f, 0.2980392277240753f, 0.2196078449487686f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.2274509817361832f, 0.2666666805744171f, 0.2078431397676468f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.5f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2862745225429535f, 0.3372549116611481f, 0.2588235437870026f, 0.4000000059604645f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.5f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 0.6000000238418579f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.5f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.1372549086809158f, 0.1568627506494522f, 0.1098039224743843f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1882352977991104f, 0.2274509817361832f, 0.1764705926179886f, 0.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.7799999713897705f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.7764706015586853f, 0.2784313857555389f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.729411780834198f, 0.6666666865348816f, 0.239215686917305f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void FutureDarkStyle()
{
    // Future Dark style by rewrking from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 1.0f;
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(20.0f, 20.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(6.0f, 6.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(12.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 3.0f);
    style.CellPadding = ImVec2(12.0f, 6.0f);
    style.IndentSpacing = 20.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabMinSize = 12.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5372549295425415f, 0.5529412031173706f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.2901960909366608f, 0.5960784554481506f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.9960784316062927f, 0.4745098054409027f, 0.6980392336845398f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
}

// Main code
int GUI::StartGUI()
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"GPlusV2", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"GPlusV2 - V0 - Developed by mateoss13", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop

    // Set default style
    HalfLifeStyle();

    // Make it so our menus stay in the window.
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    Server* default_server = new Server("localhost", "127.0.0.1", "80");
    Server* selectedServer = default_server;
    Controller::AddNewServer(default_server);

    // This worked before addnewserver required a client class incase it already exists
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Thank you to https://www.unknowncheats.me/forum/members/1543409.html for this posted in https://www.unknowncheats.me/forum/c-and-c-/427936-current-program-size-imgui.html
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

        // Show Menus
        static bool showClients = false;
        static bool showServers = false;
        static bool showSettings = false;
        
        // Editing Client Settings
        static bool EditingUsername = false;
        static bool EditingPassword = false;
        static bool EditingIPCName = false;
        static bool EditingServerName = false;
        static bool EditingIP = false;
        static bool EditingPort = false;
        static bool AddingClient = false;

        // Storing variables for client and server selection
        static std::set<Client*> selectedClients;
        static char searchBuffer[128] = "";

        static Server* selectedSrv = nullptr;
        static char server_searchBuffer[128] = "";

        // GUI Settings
        static bool useVsync = false;

        // Main Menu
        { 
            ImGui::Begin(WINDOW_TITLE, NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            ImGui::Checkbox("Clients", &showClients);
            ImGui::SameLine();
            ImGui::Checkbox("Servers", &showServers);
            ImGui::SameLine();
            ImGui::Checkbox("Settings", &showSettings);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }


        // Servers
        {
            if (showServers) {

                static bool AddingServer = false;

                ImGui::Begin("Servers", &showServers, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoNavFocus);

                // Determine the combo box label
                std::string comboLabel = selectedSrv ? selectedSrv->servername : "Select Server";

                // Server selection combo
                if (ImGui::BeginCombo("##servers", comboLabel.c_str())) {
                    for (auto& server : Controller::Servers) {
                        bool isSelected = (selectedSrv && selectedSrv->servername == server->servername);
                        std::string label = server->servername + "##" + std::to_string(reinterpret_cast<uintptr_t>(server));
                        if (ImGui::Selectable(label.c_str(), isSelected)) {
                            selectedSrv = server;
                            selectedClients.clear(); // Reset the selected client when server changes
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                // Button to select all clients associated with the selected server
                if (selectedSrv && ImGui::Button("Select All Clients")) {
                    selectedClients.clear();
                    for (auto& client : Controller::Clients) {
                        if (client->server->servername == selectedSrv->servername) {
                            selectedClients.insert(client);
                        }
                    }
                }

                if (!AddingServer && ImGui::Button("Add New Server"))
                {
                    AddingServer = true;
                }
                
                if(AddingServer) {
                    static std::string ServerNameBuffer;
                    static std::string IPBuffer;
                    static std::string PortBuffer;

                    static bool ERROR_NO_SERVER_NAME = false;
                    static bool ERROR_NO_IP = false;
                    static bool ERROR_BAD_IP = false;

                    if (ERROR_NO_SERVER_NAME)
                        ImGui::Text("You must provide a server name in order to identify it.");

                    ImGui::InputText("- Server Name", Resize(ServerNameBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue);

                    if(ERROR_NO_IP)
                        ImGui::Text("You must provide a server ip in order to connect.");

                    if (ERROR_BAD_IP)
                        ImGui::Text("The IP you provided is malformed, please remove any \":\" and instead place the port in the correct field.");

                    ImGui::InputText("- IP", Resize(IPBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::Text("Leave the port blank if it does not require a port to connect.");
                    ImGui::InputText("- Port", Resize(PortBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::Separator();
                    if (ImGui::Button("Create Server"))
                    {
                        if (ServerNameBuffer.size() <= 1)
                        {
                            ERROR_NO_SERVER_NAME = true;
                        }
                        else {
                            ERROR_NO_SERVER_NAME = false;
                        }

                        if (IPBuffer.size() <= 4) {
                            ERROR_NO_IP = true;
                        }
                        else {
                            ERROR_NO_IP = false;
                        }

                        if (IPBuffer.find(":") != std::string::npos)
                        {
                            ERROR_BAD_IP = true;
                        }
                        else {
                            ERROR_BAD_IP = false;
                        }

                        if (!ERROR_BAD_IP && !ERROR_NO_IP && !ERROR_NO_SERVER_NAME)
                        {
                            if (PortBuffer.size() <= 9 or PortBuffer.size() == std::string::npos)
                            {
                                PortBuffer = "No Port Provided";
                            }

                            Server* server = new Server(ServerNameBuffer, IPBuffer, PortBuffer);
                            Controller::AddNewServer(server);

                            ServerNameBuffer = {};
                            IPBuffer = {};
                            PortBuffer = {};
                            AddingServer = false;
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ServerNameBuffer = {};
                        IPBuffer = {};
                        PortBuffer = {};
                        AddingServer = false;
                    }
                }

                ImGui::End();
            }
        }

        // Clients
        {
            if (showClients) {
                ImGui::Begin("Clients", &showClients, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoNavFocus);

                // Button to select/unselect all
                if (selectedClients.size() == Controller::Clients.size()) {
                    if (ImGui::Button("Unselect All")) {
                        selectedClients.clear();
                    }
                }
                else {
                    if (ImGui::Button("Select All")) {
                        selectedClients = Controller::Clients;
                    }
                }

                // Input text for searching by client name
                ImGui::InputTextWithHint("##search", "Search by Name", searchBuffer, IM_ARRAYSIZE(searchBuffer));

                // Dropdown to filter by server
                if (ImGui::BeginCombo("##serverFilter", selectedServer ? selectedServer->servername.c_str() : "All Servers")) {
                    if (ImGui::Selectable("All Servers", selectedServer == nullptr)) {
                        selectedServer = nullptr;
                    }
                    for (auto& server : Controller::Servers) {
                        std::string label = server->servername + "##" + std::to_string(reinterpret_cast<uintptr_t>(server));
                        if (ImGui::Selectable(label.c_str(), selectedServer && selectedServer->servername == server->servername)) {
                            selectedServer = server;
                        }
                    }
                    ImGui::EndCombo();
                }

                // Determine the combo box label
                std::string comboLabel;
                if (selectedClients.empty()) {
                    comboLabel = "Select Clients";
                }
                else if (selectedClients.size() == 1) {
                    comboLabel = (*selectedClients.begin())->username;
                }
                else {
                    comboLabel = std::to_string(selectedClients.size()) + " Clients Selected";
                }

                // Filtered clients
                std::vector<Client*> filteredClients;
                for (auto& client : Controller::Clients) {
                    bool matchesSearch = strlen(searchBuffer) == 0 || client->username.find(searchBuffer) != std::string::npos;
                    bool matchesServer = !selectedServer || client->server->servername == selectedServer->servername;
                    if (matchesSearch && matchesServer) {
                        filteredClients.push_back(client);
                    }
                }

                // Client selection combo
                if (ImGui::BeginCombo("##clients", comboLabel.c_str())) {
                    for (auto& client : filteredClients) {
                        bool isSelected = (selectedClients.find(client) != selectedClients.end());
                        if (ImGui::Selectable(client->username.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups)) {
                            if (isSelected) {
                                selectedClients.erase(client);
                            }
                            else {
                                selectedClients.insert(client);
                            }
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                {

                    if (AddingClient)
                    {
                        // Error handler
                        static bool USERNAME_NEEDS_MORE_LENGTH = false;
                        static bool USERNAME_TOO_LONG = false;
                        static bool PASSWORD_NEEDS_MORE_LENGTH = false;
                        static bool IPC_NAME_NEEDS_MORE_LENGTH = false;
                        static bool SERVER_NEEDS_MORE_LENGTH = false;
                        
                        // Buffers
                        // Client
                        static std::string UsernameBuffer{ "" };
                        static std::string PasswordBuffer{ "" };
                        static std::string Ipc_Name{ "" };
                        // Server - Create servers in server tab.


                        ImGui::Text("Client Information");
                        if (USERNAME_NEEDS_MORE_LENGTH)
                            ImGui::Text("Your username needs more characters.");
                        if (USERNAME_TOO_LONG)
                            ImGui::Text("Your username is too long.");
                        ImGui::InputText("- Username", Resize(UsernameBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue);
                        if (PASSWORD_NEEDS_MORE_LENGTH)
                            ImGui::Text("Your password needs more characters.");
                        ImGui::InputText("- Password", Resize(PasswordBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
                        ImGui::InputText("- IPC Name", Resize(Ipc_Name), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue);
                        ImGui::Separator();
                        if (SERVER_NEEDS_MORE_LENGTH)
                            ImGui::Text("You must select a server, if you do not have one, create one via the servers tab.");
                        if (ImGui::BeginCombo("Server List", selectedServer ? selectedServer->servername.c_str() : "Select Server")) {
                            std::cout << "Currently selected: " << selectedServer << std::endl;
                            for (Server* server : Controller::Servers) {
                                bool isSelected = (selectedServer == server);
                                std::string label = server->servername + "##" + std::to_string(reinterpret_cast<uintptr_t>(server));
                                if (ImGui::Selectable(label.c_str(), isSelected)) {
                                    selectedServer = server;
                                }
                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::Separator();
                        if (ImGui::Button("Create Client"))
                        {
                            // Check username
                            if (UsernameBuffer.size() <= 3)
                            {
                                USERNAME_NEEDS_MORE_LENGTH = true;
                                USERNAME_TOO_LONG = false;
                            } 

                            if (UsernameBuffer.size() >= 64)
                            {
                                USERNAME_TOO_LONG = true;
                                USERNAME_NEEDS_MORE_LENGTH = false;
                            }

                            if (UsernameBuffer.size() > 4 && UsernameBuffer.size() < 65)
                            {
                                USERNAME_TOO_LONG = false;
                                USERNAME_NEEDS_MORE_LENGTH = false;
                            }

                            // Check password
                            if (PasswordBuffer.size() <= 5)
                            {
                                PASSWORD_NEEDS_MORE_LENGTH = true;
                            }
                            else {
                                PASSWORD_NEEDS_MORE_LENGTH = false;
                            }

                            // Check server
                            if (selectedServer == nullptr)
                            {
                                SERVER_NEEDS_MORE_LENGTH = true;
                            }
                            else {
                                SERVER_NEEDS_MORE_LENGTH = false;
                            }

                            if (!SERVER_NEEDS_MORE_LENGTH && !PASSWORD_NEEDS_MORE_LENGTH && !USERNAME_TOO_LONG && !USERNAME_NEEDS_MORE_LENGTH)
                            {
                                GMOD gmod{};
                                Steam steam{};
                                Client* client = new Client(UsernameBuffer, PasswordBuffer, selectedServer, steam, gmod, Ipc_Name);
                                Controller::AddNewClient(client);

                                // Clean up
                                UsernameBuffer = {};
                                PasswordBuffer = {};
                                Ipc_Name = {};
                                selectedServer = nullptr;

                                AddingClient = false;
                            }
                        }

                        if (AddingClient)
                            ImGui::SameLine();

                        if (AddingClient && ImGui::Button("Cancel"))
                        {
                            UsernameBuffer = {};
                            PasswordBuffer = {};
                            Ipc_Name = {};
                            selectedServer = nullptr;

                            AddingClient = false;
                        }
                    } else {
                        if (ImGui::Button("Add Client"))
                        {
                            if (AddingClient)
                            {
                                // Submit
                                AddingClient = false;
                            }
                            else {
                                AddingClient = true;
                            }
                            // We'll deal with this later
                        }
                    }

                }

                ImGui::Separator();

                // Client Info



                if (selectedClients.size() == 1 && !AddingClient)
                {
                    auto selectedClient = *(selectedClients.begin());


                    #pragma region Client Info
                    if (EditingUsername) {
                        std::string buf{ "" };
                        if (ImGui::InputText("- Username", Resize(buf), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue)) {
                            selectedClient->username = buf;
                            EditingUsername = false;
                        }
                    }
                    else {
                        std::ostringstream Username;
                        Username << "Username: " << selectedClient->username;
                        ImGui::Text(Username.str().c_str());
                        ImGui::SameLine();
                    }

                    if (!EditingUsername && ImGui::Button("Edit Username")) {
                         EditingUsername = true;
                         EditingPassword = false;
                         EditingIPCName = false;
                         EditingServerName = false;
                         EditingIP = false;
                         EditingPort = false;

 
                    }

                    // Similar blocks for Password and IPC Name

                    if (EditingPassword) {
                        std::string buf{ "" };
                        if (ImGui::InputText("- Password", Resize(buf), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password)) {
                            selectedClient->password = buf;
                            EditingUsername = false;
                            EditingPassword = true;
                            EditingIPCName = false;
                            EditingServerName = false;
                            EditingIP = false;
                            EditingPort = false;
                        }
                    }
                    else {
                        std::ostringstream Password;
                        Password << "Password: **********"; //<< selectedClient->password;
                        ImGui::Text(Password.str().c_str());
                        ImGui::SameLine();
                    }

                    if (!EditingPassword && ImGui::Button("Edit Password")) {
                        EditingUsername = false;
                        EditingPassword = true;
                        EditingIPCName = false;
                        EditingServerName = false;
                        EditingIP = false;
                        EditingPort = false;
                    }

                    if (EditingIPCName) {
                        std::string buf{ "" };
                        if (ImGui::InputText("- IPC Name", Resize(buf), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue)) {
                            selectedClient->ipc_name = buf;
                            EditingIPCName = false;
                        }
                    }
                    else {
                        std::ostringstream IPCName;
                        IPCName << "IPC Name: " << selectedClient->ipc_name;
                        ImGui::Text(IPCName.str().c_str());
                        ImGui::SameLine();
                    }

                    if (!EditingIPCName && ImGui::Button("Edit IPC Name")) {
                        EditingUsername = false;
                        EditingPassword = false;
                        EditingIPCName = true;
                        EditingServerName = false;
                        EditingIP = false;
                        EditingPort = false;
                    }

                    #pragma endregion
                    ImGui::Separator();
                    #pragma region Server Info

                    // Editing Server Name
                    if (EditingServerName) {
                        std::string buf{ "" };
                        if (ImGui::InputText("- Server Name", Resize(buf), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue)) {
                            selectedClient->server->servername = buf;
                            EditingServerName = false;
                        }
                    }
                    else {
                        std::ostringstream ServerName;
                        ServerName << "Server Name: " << selectedClient->server->servername;
                        ImGui::Text(ServerName.str().c_str());
                        ImGui::SameLine();
                    }

                    if (!EditingServerName && ImGui::Button("Edit Servername")) {
                        EditingUsername = false;
                        EditingPassword = false;
                        EditingIPCName = false;
                        EditingServerName = true;
                        EditingIP = false;
                        EditingPort = false;
                    }
                    // End Of Server Name Edit

                    // Editing IP
                    if (EditingIP) {
                        std::string buf{ "" };
                        if (ImGui::InputText("- IP", Resize(buf), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue)) {
                            selectedClient->server->ip = buf;
                            EditingIP = false;
                        }
                    }
                    else {
                        std::ostringstream IP;
                        IP << "IP: " << selectedClient->server->ip;
                        ImGui::Text(IP.str().c_str());
                        ImGui::SameLine();
                    }

                    if (!EditingIP && ImGui::Button("Edit IP")) {
                        EditingUsername = false;
                        EditingPassword = false;
                        EditingIPCName = false;
                        EditingServerName = false;
                        EditingIP = true;
                        EditingPort = false;
                    }
                    // End Of IP Edit


                    // Port
                    if (EditingPort) {
                        std::string buf{ "" };
                        if (ImGui::InputText("- Port", Resize(buf), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue)) {
                            selectedClient->server->port = buf; // because of this i've changed the gmod class to hold the port as a string, since it's easier
                            // to convert a string to int than this shitty buffer to a string
                            EditingPort = false;
                        }
                    }
                    else {
                        std::ostringstream Port;
                        Port << "Port: " << selectedClient->server->port;
                        ImGui::Text(Port.str().c_str());
                        ImGui::SameLine();
                    }

                    if (!EditingPort && ImGui::Button("Edit Port")) {
                        EditingUsername = false;
                        EditingPassword = false;
                        EditingIPCName = false;
                        EditingServerName = false;
                        EditingIP = false;
                        EditingPort = true;
                    }

                    #pragma endregion
                    ImGui::Separator();
                    #pragma region GMOD
                    // GMOD
                    // pHandle
                    std::ostringstream pHandle;
                    pHandle << "pHandle: " << selectedClient->gmod.pHandle;
                    ImGui::Text(pHandle.str().c_str());

                    // Player Health
                    ImGui::Separator();
                    std::ostringstream Health;
                    Health << "Health: " << selectedClient->gmod.localPlayer.health;
                    ImGui::Text(Health.str().c_str());

                    // Player Position
                    std::ostringstream Position;
                    Position << "Position: " << "X: " << selectedClient->gmod.localPlayer.position.x << " Y: " << selectedClient->gmod.localPlayer.position.y << " Z: " << selectedClient->gmod.localPlayer.position.z;
                    ImGui::Text(Position.str().c_str());
                    #pragma endregion

                    
                }
                else {
                    ImGui::Text("Client info not available for multiple clients");
                }



                ImGui::End();
            }

            if (showSettings)
            {
                ImGui::Begin("Settings", &showSettings, ImGuiWindowFlags_NoNavFocus);
                ImGui::Text("Themes");

                if (ImGui::Button("Half Life"))
                {
                    HalfLifeStyle();
                }

                if (ImGui::Button("Future Dark"))
                {
                   FutureDarkStyle();
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("GUI Settings");
                ImGui::Checkbox("Use vSync", &useVsync);

                ImGui::End();
            }
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = NULL;
        if (useVsync)
        {
            hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        }
        else {
            hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        }
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
