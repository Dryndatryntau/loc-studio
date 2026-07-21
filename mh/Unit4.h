//---------------------------------------------------------------------------
#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <System.IOUtils.hpp>
#include <System.Types.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <vector>

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

struct TH4rFileEntry {
    int FileIndex;
    String FileName;
    unsigned int Size;
    unsigned int CompSize;
    EH4rFileType Type;
};

typedef int (__cdecl *GetCountFunc)(const char* h4rPath);
typedef bool (__cdecl *GetInfoFunc)(const char* h4rPath, int index, char* outName, unsigned int* outSize, unsigned int* outCompSize, unsigned int* outType);
typedef bool (__cdecl *ExtractFunc)(const char* h4rPath, const char* outDir);
typedef bool (__cdecl *PackFunc)(const char* h4lPath, const char* outH4RPath);

//---------------------------------------------------------------------------
class TForm4 : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *MenuPlugins;
	TMenuItem *Help1;
	TMenuItem *Apie1;
	TMenuItem *Failas1;
	TMenuItem *Ieiti1;
	TMenuItem *Ieiti2;
	TMenuItem *Veiksla1;
	TMenuItem *Ipakuotipassirinkt1;
	TMenuItem *Ipakuotivisk1;
	TComboBox *ComboFilter;
	TOpenDialog *OpenDialog1;
	TListView *ListViewFiles;
	TProgressBar *ProgressBar1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Apie1Click(TObject *Sender);
	void __fastcall Ieiti1Click(TObject *Sender);
	void __fastcall Ipakuotivisk1Click(TObject *Sender);
	void __fastcall Ipakuotipassirinkt1Click(TObject *Sender);
    void __fastcall ListViewFilesData(TObject *Sender, TListItem *Item);
private:	// User declarations
    std::vector<TH4rFileEntry> AllArchiveFiles; // Sàraðas operatyviojoje atmintyje
    String OpenedArchivePath;

    HINSTANCE hMh4CoreDll;
    GetCountFunc dllGetH4RFileCount;
    GetInfoFunc  dllGetH4RFileInfo;
    ExtractFunc  dllExtractH4R;
    PackFunc     dllPackH4R;

    void ReadH4rStructure(String ArchivePath);
    void RefreshFileTable(int FilterType);
public:		// User declarations
	__fastcall TForm4(TComponent* Owner);
    void __fastcall PluginMenuClick(TObject *Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm4 *Form4;
//---------------------------------------------------------------------------
#endif

