//---------------------------------------------------------------------------
#ifndef UnitPluginH
#define UnitPluginH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <System.IOUtils.hpp>
#include <vector>
#include <set>
#include <string>
#include <IniFiles.hpp>

// ==========================================================================
// Plugin interface structures (match host EXE)
// ==========================================================================
typedef struct {
    const char* PluginName;
    const char* Version;
} TPluginInfo;

// ==========================================================================
// File entry for archive
// ==========================================================================
enum EH4rFileType {
    H4R_ACTOR_SEQUENCE = 0,
    H4R_ADV_ACTOR,
    H4R_ADV_OBJECT,
    H4R_ANIMATION,
    H4R_BATTLEFIELD_PRESET_MAP,
    H4R_BITMAP_RAW,
    H4R_BINK,
    H4R_CASTLE,
    H4R_COMBAT_ACTOR,
    H4R_COMBAT_HEADER_TABLE_CACHE,
    H4R_COMBAT_OBJECT,
    H4R_FONT,
    H4R_GAME_MAPS,
    H4R_LAYERS,
    H4R_SOUND,
    H4R_STRINGS,
    H4R_TABLE,
    H4R_TERRAIN,
    H4R_TRANSITION,
    H4R_ALL,
    H4R_NOTHING
};

struct TFileEntry {
    int ID;
    String FileName;
    unsigned int Size;
    unsigned int CompSize;
    EH4rFileType Type;
};

// ==========================================================================
// Core DLL function pointers
// ==========================================================================
typedef int (__cdecl *GetCountFunc)(const char* h4rPath);
typedef bool (__cdecl *GetInfoFunc)(const char* h4rPath, int index, char* outName,
                                    unsigned int* outSize, unsigned int* outCompSize,
                                    unsigned int* outType);
typedef bool (__cdecl *ExtractIndicesFunc)(const char* h4rPath, const char* outDir,
                                           const int* indices, int count);
typedef bool (__cdecl *PackFunc)(const char* h4lPath, const char* outH4RPath);

// ==========================================================================
// Plugin form class
// ==========================================================================
class TPluginForm : public TForm
{
__published:
    // These components MUST match the .dfm file
    TStringGrid *GridFiles;
    TEdit *EditSearch;
    TButton *BtnOpen;
    TButton *BtnExtractSel;
    TButton *BtnExtractAll;
    TButton *BtnPack;
    TProgressBar *ProgressBar1;
    TOpenDialog *OpenDialog1;
    TSaveDialog *SaveDialog1;

private:
    std::vector<TFileEntry> AllFiles;
    std::vector<TFileEntry> FilteredFiles;
    std::set<int> SelectedRows;   // grid row indices (1-based, excluding header)
    String OpenedArchivePath;
    String CurrentLangCode;

    int FSortColumn;
    bool FSortAscending;

    HINSTANCE hCoreDll;
    GetCountFunc dllGetH4RFileCount;
    GetInfoFunc  dllGetH4RFileInfo;
    ExtractIndicesFunc dllExtractH4RByIndices;
    PackFunc     dllPackH4R;

    bool LoadCoreDll();
    void UnloadCoreDll();
    void ReadH4rStructure(const String& ArchivePath);
    void ApplyFilter(const String& searchText);
    void UpdateGrid();
    void ApplySort();
    void UpdateCaption();

    // Event handlers
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall BtnOpenClick(TObject *Sender);
    void __fastcall BtnExtractSelClick(TObject *Sender);
    void __fastcall BtnExtractAllClick(TObject *Sender);
    void __fastcall BtnPackClick(TObject *Sender);
    void __fastcall EditSearchChange(TObject *Sender);
    void __fastcall GridFilesDrawCell(TObject *Sender, int ACol, int ARow,
                                      const TRect &Rect, TGridDrawState State);
    void __fastcall GridFilesMouseUp(TObject *Sender, TMouseButton Button,
                                     TShiftState Shift, int X, int Y);
    void __fastcall GridFilesSelectCell(TObject *Sender, int ACol, int ARow,
                                        bool &CanSelect);

public:
    __fastcall TPluginForm(TComponent* Owner);
    __fastcall ~TPluginForm();

    // Public method for language loading (called from ExecutePlugin)
    void LoadLanguage(const String& langCode);
};

// ==========================================================================
// Exported plugin functions
// ==========================================================================
extern "C" __declspec(dllexport) void __stdcall GetPluginInfo(TPluginInfo* Info);
extern "C" __declspec(dllexport) void __stdcall ExecutePlugin(HWND parentHWND, const char* langCode);

//---------------------------------------------------------------------------
#endif
