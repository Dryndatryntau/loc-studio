//---------------------------------------------------------------------------
#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Imaging.jpeg.hpp>
#include <IniFiles.hpp>
#include <System.IOUtils.hpp>
#include <vector>
#include <map>
#include <string>

// ==========================================================================
// Plugin interface definitions (common for all plugins)
// ==========================================================================
typedef struct {
    const char* PluginName;   // Friendly name for menu
    const char* Version;      // Version string
} TPluginInfo;

// Main execution function - must be exported by every DLL plugin
typedef void (__stdcall *TExecutePluginFunc)(HWND parentHWND, const char* langCode);

// Optional function to retrieve plugin info
typedef const TPluginInfo* (__stdcall *TGetPluginInfoFunc)();

// ==========================================================================
// Plugin item types
// ==========================================================================
enum EPluginType { ptDLL, ptEXE };

struct TPluginItem {
    String DisplayName;        // Name shown in menu
    String Path;              // Full path to file
    EPluginType Type;
    String GameCategory;      // e.g., "Heroes4"
};

// ==========================================================================
// Main form class (TForm4)
// ==========================================================================
class TForm4 : public TForm
{
__published:

private:
	TMainMenu *MainMenu;
	TImage *BackgroundImage;
	TStatusBar *StatusBar;
	// Language management
	String CurrentLangCode;
	String CurrentLangFile;
	TIniFile *LangIni;
	std::map<String, String> LangMap;

    // Plugin storage
    std::vector<TPluginItem> PluginItems;
    std::map<String, HMODULE> LoadedDLLs;

    // Menu items
    TMenuItem *MenuGames;
    TMenuItem *MenuLanguage;
    TMenuItem *MenuExit;

    // Internal methods
    void LoadLanguage(const String& langCode);
    void BuildLanguageMenu();
    void BuildPluginMenus();
    void ScanPlugins();
    void ExecuteDLLPlugin(const TPluginItem& item);
    void ExecuteEXEPlugin(const TPluginItem& item);
    void UpdateStatusBar(const String& msg);

    // Event handlers
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall MenuItemClick(TObject *Sender);
    void __fastcall LanguageItemClick(TObject *Sender);
    void __fastcall ExitItemClick(TObject *Sender);

public:
    __fastcall TForm4(TComponent* Owner);
};

//---------------------------------------------------------------------------
extern PACKAGE TForm4 *Form4;
//---------------------------------------------------------------------------
#endif
