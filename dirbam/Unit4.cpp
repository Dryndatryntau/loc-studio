//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include <shellapi.h>   // for ShellExecuteEx
#include <psapi.h>      // optional

#pragma package(smart_init)
#pragma resource "*.dfm"

TForm4 *Form4;

// ==========================================================================
// Constructor / Destructor
// ==========================================================================
__fastcall TForm4::TForm4(TComponent* Owner)
    : TForm(Owner), LangIni(NULL)
{
    // Set form properties
    Caption = L"Heroes Modding Platform";
    Width = 800;
    Height = 600;
    Position = poScreenCenter;
    WindowState = wsNormal;

    // Create menu
    MainMenu = new TMainMenu(this);

    // Create "Games" menu
    MenuGames = new TMenuItem(MainMenu);
    MenuGames->Caption = L"&Games";
    MainMenu->Items->Add(MenuGames);

    // Create "Settings" menu
    TMenuItem *MenuSettings = new TMenuItem(MainMenu);
    MenuSettings->Caption = L"&Settings";
    MainMenu->Items->Add(MenuSettings);

    // Language submenu
    MenuLanguage = new TMenuItem(MenuSettings);
    MenuLanguage->Caption = L"&Language";
    MenuSettings->Add(MenuLanguage);

    // Separator
    TMenuItem *Sep = new TMenuItem(MenuSettings);
    Sep->Caption = L"-";
    MenuSettings->Add(Sep);

    // Exit
    MenuExit = new TMenuItem(MenuSettings);
    MenuExit->Caption = L"E&xit";
    MenuExit->OnClick = ExitItemClick;
    MenuSettings->Add(MenuExit);

    // Background image
    BackgroundImage = new TImage(this);
    BackgroundImage->Parent = this;
    BackgroundImage->Align = alClient;
    BackgroundImage->Center = true;
    BackgroundImage->Proportional = true;
    // Load default background if exists
    String bgFile = ExtractFilePath(Application->ExeName) + L"bg.jpg";
    if (FileExists(bgFile))
        BackgroundImage->Picture->LoadFromFile(bgFile);

    // Status bar
    StatusBar = new TStatusBar(this);
    StatusBar->Parent = this;
    StatusBar->Align = alBottom;
    StatusBar->SimplePanel = true;
    StatusBar->SimpleText = L"Loading...";

    // Load saved language
    String iniPath = ExtractFilePath(Application->ExeName) + L"host.ini";
    TIniFile *hostIni = new TIniFile(iniPath);
    String savedLang = hostIni->ReadString(L"Settings", L"Language", L"lithuanian");
    delete hostIni;

    // Load that language
    LoadLanguage(savedLang);
}

//---------------------------------------------------------------------------
__fastcall TForm4::~TForm4()
{
    // Unload any loaded DLLs
    for (auto it = LoadedDLLs.begin(); it != LoadedDLLs.end(); ++it) {
        if (it->second)
            FreeLibrary(it->second);
    }
    LoadedDLLs.clear();

    // Clean up language INI
    if (LangIni) {
        delete LangIni;
        LangIni = NULL;
    }

    // Clean up menu item tags (TPluginItem pointers)
    for (int i = 0; i < MenuGames->Count; i++) {
        TMenuItem *catItem = MenuGames->Items[i];
        if (catItem) {
            for (int j = 0; j < catItem->Count; j++) {
                TMenuItem *pluginItem = catItem->Items[j];
                if (pluginItem && pluginItem->Tag) {
                    delete (TPluginItem*)pluginItem->Tag;
                    pluginItem->Tag = 0;
                }
            }
        }
    }
}

// ==========================================================================
// Language handling
// ==========================================================================
void TForm4::LoadLanguage(const String& langCode)
{
    CurrentLangCode = langCode;

    // Close previous INI
    if (LangIni) {
        delete LangIni;
        LangIni = NULL;
    }

    // Build path to language file
    String langDir = ExtractFilePath(Application->ExeName) + L"lang\\";
    String langFile = langDir + langCode + L".ini";

    if (!FileExists(langFile)) {
        // Fallback to default (english)
        langFile = langDir + L"english.ini";
        if (!FileExists(langFile)) {
            // Create a minimal default ini
            ForceDirectories(langDir);
            TStringList *def = new TStringList;
            def->Add(L"[HostEXE]");
            def->Add(L"Menu_Games=Games");
            def->Add(L"Menu_Language=Language");
            def->Add(L"Menu_Exit=Exit");
            def->Add(L"Status_Ready=Ready. Plugins loaded.");
            def->SaveToFile(langFile);
            delete def;
        }
        CurrentLangCode = L"english";
    }

    // Load INI
    LangIni = new TIniFile(langFile);
    CurrentLangFile = langFile;

    // Update menu captions
    MenuGames->Caption = LangIni->ReadString(L"HostEXE", L"Menu_Games", L"Games");
    MenuLanguage->Caption = LangIni->ReadString(L"HostEXE", L"Menu_Language", L"Language");
    MenuExit->Caption = LangIni->ReadString(L"HostEXE", L"Menu_Exit", L"Exit");

    // Update status bar
    String statusMsg = LangIni->ReadString(L"HostEXE", L"Status_Ready", L"Ready.");
    UpdateStatusBar(statusMsg);

    // Rebuild language menu (to show current selection)
    BuildLanguageMenu();

    // Save preference
    String iniPath = ExtractFilePath(Application->ExeName) + L"host.ini";
    TIniFile *hostIni = new TIniFile(iniPath);
    hostIni->WriteString(L"Settings", L"Language", CurrentLangCode);
    delete hostIni;
}

//---------------------------------------------------------------------------
void TForm4::BuildLanguageMenu()
{
    // Clear existing items
    MenuLanguage->Clear();

    // Scan lang folder for .ini files
    String langDir = ExtractFilePath(Application->ExeName) + L"lang\\";
    if (!DirectoryExists(langDir))
        ForceDirectories(langDir);

    TStringDynArray files = TDirectory::GetFiles(langDir, L"*.ini");
    for (int i = 0; i < files.Length; i++) {
        String fileName = ExtractFileName(files[i]);
        String langCode = ChangeFileExt(fileName, L""); // remove .ini

        TMenuItem *item = new TMenuItem(MenuLanguage);
        item->Caption = langCode; // display the code
        item->Tag = (NativeInt)new String(langCode); // store lang code
        item->OnClick = LanguageItemClick;
        MenuLanguage->Add(item);

        // Check if this is the current language
        if (langCode == CurrentLangCode)
            item->Checked = true;
    }
}

//---------------------------------------------------------------------------
void __fastcall TForm4::LanguageItemClick(TObject *Sender)
{
    TMenuItem *item = dynamic_cast<TMenuItem*>(Sender);
    if (!item) return;

    String* langPtr = (String*)item->Tag;
    if (langPtr) {
        LoadLanguage(*langPtr);
        delete langPtr;
        item->Tag = 0;
    }
}

// ==========================================================================
// Plugin scanning and menu building
// ==========================================================================
void TForm4::ScanPlugins()
{
    PluginItems.clear();

    String pluginsDir = ExtractFilePath(Application->ExeName) + L"plugins\\";
    if (!DirectoryExists(pluginsDir)) {
        ForceDirectories(pluginsDir);
        return;
    }

    // Enumerate subfolders (each represents a game)
    TStringDynArray gameFolders = TDirectory::GetDirectories(pluginsDir);
    for (int g = 0; g < gameFolders.Length; g++) {
        String gameFolder = gameFolders[g];
        String gameName = ExtractFileName(gameFolder);

        // Scan for .dll and .exe files in that folder (including subfolders)
        TStringDynArray files = TDirectory::GetFiles(gameFolder, L"*.*", TSearchOption::soAllDirectories);
        for (int f = 0; f < files.Length; f++) {
            String ext = ExtractFileExt(files[f]).LowerCase();
            if (ext == L".dll" || ext == L".exe") {
                TPluginItem item;
                item.Path = files[f];
                item.Type = (ext == L".dll") ? ptDLL : ptEXE;
                item.GameCategory = gameName;

                // Determine display name:
                String displayName = ExtractFileName(files[f]);
                displayName = ChangeFileExt(displayName, L""); // remove extension

                if (item.Type == ptDLL) {
                    // Try to load temporarily to read info
                    HMODULE hTemp = LoadLibraryW(files[f].c_str());
                    if (hTemp) {
                        TGetPluginInfoFunc getInfo = (TGetPluginInfoFunc)GetProcAddress(hTemp, "GetPluginInfo");
                        if (getInfo) {
                            const TPluginInfo* info = getInfo();
                            if (info && info->PluginName) {
                                displayName = String(info->PluginName);
                            }
                        }
                        FreeLibrary(hTemp); // don't keep loaded
                    }
                }
                item.DisplayName = displayName;
                PluginItems.push_back(item);
            }
        }
    }
}

//---------------------------------------------------------------------------
void TForm4::BuildPluginMenus()
{
    // Clear existing game menu items
    MenuGames->Clear();

    // Group items by game category
    std::map<String, std::vector<TPluginItem>> grouped;
    for (const auto& item : PluginItems) {
        grouped[item.GameCategory].push_back(item);
    }

    for (auto& group : grouped) {
        String category = group.first;
        // Create a submenu item for this category
        TMenuItem *catItem = new TMenuItem(MenuGames);
        catItem->Caption = category;
        MenuGames->Add(catItem);

        // Add each plugin in this category
        for (const auto& item : group.second) {
            TMenuItem *pluginItem = new TMenuItem(catItem);
            pluginItem->Caption = item.DisplayName;
            pluginItem->Tag = (NativeInt)new TPluginItem(item); // store a copy
            pluginItem->OnClick = MenuItemClick;
            catItem->Add(pluginItem);
        }
    }
}

// ==========================================================================
// Menu item click - execute plugin
// ==========================================================================
void __fastcall TForm4::MenuItemClick(TObject *Sender)
{
    TMenuItem *item = dynamic_cast<TMenuItem*>(Sender);
    if (!item) return;

    TPluginItem* pItem = (TPluginItem*)item->Tag;
    if (!pItem) return;

    if (pItem->Type == ptDLL) {
        ExecuteDLLPlugin(*pItem);
    } else {
        ExecuteEXEPlugin(*pItem);
    }
}

// ==========================================================================
// Execute DLL plugin
// ==========================================================================
void TForm4::ExecuteDLLPlugin(const TPluginItem& item)
{
    // Check if already loaded
    HMODULE hDll = NULL;
    auto it = LoadedDLLs.find(item.Path);
    if (it != LoadedDLLs.end()) {
        hDll = it->second;
    } else {
        // Load the DLL
        hDll = LoadLibraryW(item.Path.c_str());
        if (!hDll) {
            ShowMessage(L"Failed to load plugin: " + item.Path);
            return;
        }
        LoadedDLLs[item.Path] = hDll;
    }

    // Get ExecutePlugin function
    TExecutePluginFunc execFunc = (TExecutePluginFunc)GetProcAddress(hDll, "_ExecutePlugin@8");
    if (!execFunc) {
        // Try without decoration (some compilers may export without @)
        execFunc = (TExecutePluginFunc)GetProcAddress(hDll, "ExecutePlugin");
    }

    if (!execFunc) {
        ShowMessage(L"Plugin does not export ExecutePlugin function.");
        return;
    }

    // Prepare language code (short code like "lt", "en")
    String langCode = CurrentLangCode;
    AnsiString ansiLang = AnsiString(langCode);

    // Call the plugin – it should create a modal window with parent = this->Handle
    execFunc(this->Handle, ansiLang.c_str());
}

// ==========================================================================
// Execute external EXE plugin (modal)
// ==========================================================================
void TForm4::ExecuteEXEPlugin(const TPluginItem& item)
{
    // Use ShellExecuteEx with wait
    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = item.Path.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteEx(&sei)) {
        ShowMessage(L"Failed to execute " + item.Path);
        return;
    }

    // Disable host window to simulate modality
    EnableWindow(this->Handle, false);

    // Wait for the process to finish
    WaitForSingleObject(sei.hProcess, INFINITE);

    // Re-enable host
    EnableWindow(this->Handle, true);
    SetForegroundWindow(this->Handle);

    CloseHandle(sei.hProcess);
}

// ==========================================================================
// Utility
// ==========================================================================
void TForm4::UpdateStatusBar(const String& msg)
{
    if (StatusBar)
        StatusBar->SimpleText = msg;
}

// ==========================================================================
// Form events
// ==========================================================================
void __fastcall TForm4::FormCreate(TObject *Sender)
{
    // Scan plugins and build menus
    ScanPlugins();
    BuildPluginMenus();

    // Update status
    String msg = LangIni ? LangIni->ReadString(L"HostEXE", L"Status_Ready", L"Ready.") : L"Ready.";
    UpdateStatusBar(msg);
}

//---------------------------------------------------------------------------
void __fastcall TForm4::FormDestroy(TObject *Sender)
{
    // Cleanup is done in destructor
}

//---------------------------------------------------------------------------
void __fastcall TForm4::ExitItemClick(TObject *Sender)
{
    Close();
}

//---------------------------------------------------------------------------
