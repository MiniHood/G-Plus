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
#include "Styles.h"
#include "filesystem"
#include "Globals.h"

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
bool done = false;

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
    Styles::Classic_Steam();

    // Make it so our menus stay in the window.
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // create localhost server
    Server* default_server = new Server("localhost", "127.0.0.1", "80");
    Server* selectedServer = default_server;
    Controller::AddNewServer(default_server);
    
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

        // Settings
        static bool ShowThemes = false;

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
                        ImGui::Text("I highly suggest you do not edit the IPC Name unless you must.");
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
                                Client* client = new Client(UsernameBuffer, PasswordBuffer, selectedServer, Steam{}, GMOD{}, !Ipc_Name.empty() ? Ipc_Name : "");
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
                //  Resize(ServerNameBuffer), Resize::len(),

                ImGui::Checkbox("Show Themes", &ShowThemes);

                if (ShowThemes)
                {
                    if (ImGui::Button("Black Devil"))
                    {
                        Styles::BlackDevil();
                    }
                    if (ImGui::Button("Bootstrap Dark"))
                    {
                        Styles::BootstrapDark();
                    }
                    if (ImGui::Button("Cherry"))
                    {
                        Styles::Cherry();
                    }
                    if (ImGui::Button("Classic"))
                    {
                        Styles::Classic();
                    }
                    if (ImGui::Button("Classic Steam"))
                    {
                        Styles::Classic_Steam();
                    }
                    if (ImGui::Button("Clean Dark Red"))
                    {
                        Styles::Clean_Dark_Red();
                    }
                    if (ImGui::Button("Comfy"))
                    {
                        Styles::Comfy();
                    }
                    if (ImGui::Button("Duck Red (Quack)"))
                    {
                        Styles::Duck_Quack_Red();
                    }
                    if (ImGui::Button("Darcula"))
                    {
                        Styles::Darcula();
                    }
                    if (ImGui::Button("Dark"))
                    {
                        Styles::Dark();
                    }
                    if (ImGui::Button("Deep Dark"))
                    {
                        Styles::DeepDark();
                    }
                    if (ImGui::Button("Discord Dark"))
                    {
                        Styles::Discord_Dark();
                    }
                    if (ImGui::Button("Enemymouse"))
                    {
                        Styles::Enemyouse();
                    }
                    if (ImGui::Button("Future Dark"))
                    {
                        Styles::Future_Dark();
                    }
                    if (ImGui::Button("Gold"))
                    {
                        Styles::Gold();
                    }
                    if (ImGui::Button("Green Font"))
                    {
                        Styles::GreenFont();
                    }
                    if (ImGui::Button("Light"))
                    {
                        Styles::Light();
                    }
                    if (ImGui::Button("Material Flat"))
                    {
                        Styles::Material_Flat();
                    }
                    if (ImGui::Button("Microsoft"))
                    {
                        Styles::Microsoft();
                    }
                    if (ImGui::Button("Modern"))
                    {
                        Styles::Modern();
                    }
                    if (ImGui::Button("Moonlight"))
                    {
                        Styles::Moonlight();
                    }
                    if (ImGui::Button("Photoshop"))
                    {
                        Styles::Photoshop();
                    }
                    if (ImGui::Button("Purple Comfy"))
                    {
                        Styles::Purple_Comfy();
                    }
                    if (ImGui::Button("Quick Minimal Look"))
                    {
                        Styles::Quick_Minimal_Look();
                    }
                    if (ImGui::Button("Red Font"))
                    {
                        Styles::Red_Font();
                    }
                    ImGui::BeginDisabled();
                    if (ImGui::Button("Red Oni"))
                    {
                        Styles::Red_Oni();
                    }
                    ImGui::EndDisabled();
                    if (ImGui::Button("Rounded Visual Studio"))
                    {
                        Styles::Rounded_Visual_Studio();
                    }
                    if (ImGui::Button("Soft Cherry"))
                    {
                        Styles::Soft_Cherry();
                    }
                    if (ImGui::Button("Sonic Riders"))
                    {
                        Styles::Sonic_Riders();
                    }
                    if (ImGui::Button("Unreal"))
                    {
                        Styles::Unreal();
                    }
                    if (ImGui::Button("Visual Studio"))
                    {
                        Styles::Visual_Studio();
                    }
                    if (ImGui::Button("ledSynthmaster"))
                    {
                        Styles::IedSynthmaster();
                    }
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("GUI Settings");
                ImGui::Checkbox("Use vSync", &useVsync);
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("Steam");


                static std::string SteamPathBuffer;
                static std::string SteamVersionBuffer;

                std::ostringstream STEAMPL;
                STEAMPL << "Steam Path (" << Globals::Steam::Path << ")";
                std::string labelPass_steampath = STEAMPL.str() + "##" + std::to_string((int)&STEAMPL);
                if (ImGui::InputText(labelPass_steampath.c_str(), Resize(SteamPathBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    if (std::filesystem::is_directory(SteamPathBuffer))
                    {
                        Globals::Steam::Path = SteamPathBuffer;
                    }
                }

                std::ostringstream STEAMVL;
                STEAMVL << "Steam Version (" << Globals::Steam::Version << ")";
                std::string labelPass_steamversion = STEAMVL.str() + "##" + std::to_string((int)&STEAMVL);
                if (ImGui::InputText(labelPass_steamversion.c_str(), Resize(SteamVersionBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    Globals::Steam::Version = SteamVersionBuffer;
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("Garry\'s Mod");

                static std::string GMODPathBuffer;
                static std::string GMODVersionBuffer;

                std::ostringstream GMODPL;
                GMODPL << "Garry\'s Mod Path (" << Globals::GMOD::Path << ")";
                std::string labelPass_gmodpath = GMODPL.str() + "##" + std::to_string((int)&GMODPL);
                if (ImGui::InputText(labelPass_gmodpath.c_str(), Resize(GMODPathBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    if (std::filesystem::is_directory(GMODPathBuffer))
                    {
                        Globals::GMOD::Path = GMODPathBuffer;
                    }
                }


                std::ostringstream GMODVL;
                GMODPL << "Garry\'s Mod Version (" << Globals::GMOD::PatchVersion << ")";
                std::string labelPass_version = GMODPL.str() + "##" + std::to_string((int)&GMODVL);
                if (ImGui::InputText(labelPass_version.c_str(), Resize(GMODVersionBuffer), Resize::len(), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    if (std::filesystem::is_directory(GMODVersionBuffer))
                    {
                        Globals::GMOD::Path = GMODVersionBuffer;
                    }
                }


                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

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
