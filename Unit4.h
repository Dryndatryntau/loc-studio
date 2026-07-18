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
#include <fstream>
#include <vector>

// ============================================================================
// PILNAS ÞAIDIMO FAILØ TIPØ SÀRAÐAS (Suderintas su jûsø komentarais):
// Átraukti visi numatytieji þaidimo tipai ir nauji filtrai sàraðo gale.
// ============================================================================
enum EH4rFileType {
    H4R_ACTOR_SEQUENCE = 0,                       // Sprite animation
    H4R_ADV_ACTOR,                                // Hero sprite
    H4R_ADV_OBJECT,                               // Object sprite
    H4R_ANIMATION,                                // Animation
    H4R_BATTLEFIELD_PRESET_MAP,                   // ???
    H4R_BITMAP_RAW,                               // Image
    H4R_BINK,                                     // Movie
    H4R_CASTLE,                                   // ???
    H4R_COMBAT_ACTOR,                             // ???
    H4R_COMBAT_HEADER_TABLE_CACHE,                // ???
    H4R_COMBAT_OBJECT,                            // ???
    H4R_FONT,                                     // Font
    H4R_GAME_MAPS,                                // Map (campaign or tutorial)
    H4R_LAYERS,                                   // ???
    H4R_SOUND,                                    // Music or sound
    H4R_STRINGS,                                  // Text
    H4R_TABLE,                                    // Text
    H4R_TERRAIN,                                  // ???
    H4R_TRANSITION,                               // ???

    // Pridëti nauji tipai masiniam valdymui ir filtravimui
    H4R_ALL,                                      // Rodyti visus failus lentelëje
    H4R_NOTHING                                   // Neþinomi / kiti failø tipai
};

struct TH4rFileEntry {
    String FileName;       // Name
    unsigned int Offset;   // Offset
    unsigned int SizeAfter;// Size (suspaustas)
    unsigned int SizeBefore;// Unpacked size (iðpakuotas)
    EH4rFileType Type;     // Type
    String DevPath;        // Path (Komentaras)
    unsigned int FileDate; // Date
    unsigned int HeaderID; // Vidinis áraðo numeris (Debug/Nr)
    unsigned int DebugSize;
};

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
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Apie1Click(TObject *Sender);
	void __fastcall Ieiti1Click(TObject *Sender);
private:	// User declarations
    std::vector<TH4rFileEntry> AllArchiveFiles;
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

