//---------------------------------------------------------------------------
#ifndef UnitMainH
#define UnitMainH
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
//---------------------------------------------------------------------------
// Plugin interface (copied locally to avoid external dependency)
typedef struct {
    const wchar_t* PluginName;
    const wchar_t* Version;
} TPluginInfo;

typedef const TPluginInfo* (__stdcall *TGetPluginInfoFunc)(const wchar_t* langCode);
typedef void (__stdcall *TExecutePluginFunc)(HWND parentHWND, const wchar_t* langFilePath);

enum EPluginType { ptDLL };

struct TPluginItem {
    std::wstring DisplayName;
    std::wstring Path;
    std::wstring GameCategory;
    EPluginType Type;
};

//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:
    TMainMenu *MainMenu;
    TImage *BackgroundImage;
    TStatusBar *StatusBar;
    TMenuItem *MenuGames;
    TMenuItem *MenuLanguage;
    TMenuItem *MenuExit;

private:
    std::wstring CurrentLangCode;
    std::wstring CurrentLangFile;
    TMemIniFile *LangIni;
    std::map<std::wstring, std::wstring> LangMap;

    std::vector<TPluginItem> PluginItems;
    std::map<std::wstring, HMODULE> LoadedDLLs; // saugome užkrautas DLL (jei norime)

    void __fastcall LoadLanguage(const std::wstring& langCode);
    void __fastcall BuildLanguageMenu();
    void __fastcall ScanPlugins();
    void __fastcall BuildPluginMenus();
    void __fastcall ExecutePlugin(const TPluginItem& item);
    void __fastcall UpdateStatusBar(const std::wstring& msg);

    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall MenuItemClick(TObject *Sender);
    void __fastcall LanguageItemClick(TObject *Sender);
    void __fastcall ExitItemClick(TObject *Sender);

public:
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif