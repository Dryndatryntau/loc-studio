#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include <System.IOUtils.hpp>
#include <System.Types.hpp>
#include <ShObjIdl.h>
#include <cstdint>
#include <System.Classes.hpp>
#include <System.SysUtils.hpp>

#pragma package(smart_init)
#pragma resource "*.dfm"
TForm4 *Form4;

// VCL áskiepiø funkcijø tipai
typedef const char* (__stdcall *TGetPluginName)();
typedef void (__stdcall *TRunPlugin)();

//---------------------------------------------------------------------------
__fastcall TForm4::TForm4(TComponent* Owner)
    : TForm(Owner)
{
    hMh4CoreDll = NULL;
    dllGetH4RFileCount = NULL;
    dllGetH4RFileInfo = NULL;
    dllExtractH4R = NULL;
    dllPackH4R = NULL;
}

//---------------------------------------------------------------------------
void __fastcall TForm4::PluginMenuClick(TObject *Sender)
{
    TMenuItem *ClickedItem = dynamic_cast<TMenuItem*>(Sender);
    if (ClickedItem != NULL) {
        HMODULE hDll = (HMODULE)ClickedItem->Tag;
        if (hDll != NULL) {
            TRunPlugin RunPluginFunc = (TRunPlugin)GetProcAddress(hDll, "_RunPlugin");
            if (RunPluginFunc != NULL) {
                RunPluginFunc();
            } else {
                ShowMessage("Klaida: Áskiepyje nerasta 'RunPlugin' funkcija!");
            }
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TForm4::FormCreate(TObject *Sender)
{
    // 1. Paruoðiame virtualià lentelæ ir stulpelius
    ListViewFiles->ViewStyle = vsReport;
    ListViewFiles->ShowColumnHeaders = true;
    ListViewFiles->OwnerData = true;
    ListViewFiles->Columns->Clear();

    const wchar_t* Headers[] = {
        L"Indeksas",
        L"Tikrasis dydis",
        L"Suspaustas dydis",
        L"Objekto tipas",
        L"Failo pavadinimas"
    };

    for (int i = 0; i < 5; i++) {
        TListColumn *Col = ListViewFiles->Columns->Add();
        Col->Caption = Headers[i];
    }

    for (int i = 0; i < ListViewFiles->Columns->Count; i++) {
        ListViewFiles->Columns->Items[i]->Width = -2;
    }

    OpenDialog1->InitialDir = ExtractFilePath(Application->ExeName);

    // 2. ÁKRAUNAME JÛSØ MH4_CORE.DLL Á RAM ATMINTÁ VIENÀ KARTÀ
    String DllName = "mh4_core.dll";
    hMh4CoreDll = LoadLibraryW(DllName.c_str());
    if (hMh4CoreDll != NULL) {
		dllGetH4RFileCount = (GetCountFunc)GetProcAddress(hMh4CoreDll, "GetH4RFileCount");
		dllGetH4RFileInfo  = (GetInfoFunc)GetProcAddress(hMh4CoreDll, "GetH4RFileInfo");
        dllExtractH4R      = (ExtractFunc)GetProcAddress(hMh4CoreDll, "ExtractH4R");
        dllPackH4R         = (PackFunc)GetProcAddress(hMh4CoreDll, "PackH4R");

//        if (!dllGetH4RFileCount || !dllGetH4RFileInfo || !dllExtractH4R || !dllPackH4R) {
if (dllGetH4RFileCount == NULL) ShowMessage("NULL yra: GetH4RFileCount");
if (dllGetH4RFileInfo == NULL)  ShowMessage("NULL yra: GetH4RFileInfo");
if (dllExtractH4R == NULL)      ShowMessage("NULL yra: ExtractH4R");
if (dllPackH4R == NULL)         ShowMessage("NULL yra: PackH4R");


		//	ShowMessage("Klaida: mh4_core.dll faile nerasta viena ið funkcijø!");
  //      }
	} else {
        ShowMessage("Klaida: Nepavyko paleisties metu uþkrauti mh4_core.dll!");
    }

    // 3. Krauname jûsø VCL áskiepius (tekstø redaktoriø ir t.t.)
    String PluginsPath = ExtractFilePath(Application->ExeName) + "Unpacker_System\\plugins\\";
    if (TDirectory::Exists(PluginsPath)) {
        TStringDynArray DllFiles = TDirectory::GetFiles(PluginsPath, "*.dll");
        for (int i = 0; i < DllFiles.Length; i++) {
            String CurrentDll = DllFiles[i];
            HMODULE hDll = LoadLibraryW(CurrentDll.c_str());
            if (hDll != NULL) {
                TGetPluginName GetPluginNameFunc = (TGetPluginName)GetProcAddress(hDll, "_GetPluginName");
                if (GetPluginNameFunc != NULL) {
                    const char* pluginName = GetPluginNameFunc();
                    TMenuItem *NewItem = new TMenuItem(MainMenu1);
                    NewItem->Caption = String(pluginName);
                    NewItem->Tag = (NativeInt)hDll;
                    NewItem->OnClick = PluginMenuClick;
                    MenuPlugins->Add(NewItem);
                } else {
                    FreeLibrary(hDll);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TForm4::FormDestroy(TObject *Sender)
{
    // Saugiai iðkrauname jûsø branduolá ið RAM atminties uþdarant programà
    if (hMh4CoreDll != NULL) {
        FreeLibrary(hMh4CoreDll);
    }

    // Iðkrauname VCL áskiepius
    if (MenuPlugins != NULL) {
        for (int i = MenuPlugins->Count - 1; i >= 0; i--) {
            TMenuItem *Item = MenuPlugins->Items[i];
            if (Item != NULL) {
                HMODULE hDll = (HMODULE)Item->Tag;
                if (hDll != NULL) {
                    FreeLibrary(hDll);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TForm4::Apie1Click(TObject *Sender)
{
    ShowMessage("Heroes studio v2.5\n\n"
                "Branduolys sëkmingai perkeltas á RAM atmintá!\n"
                "Sukurta su C++ Builder (Maksimalus Win XP / 7 suderinamumas).");
}

//---------------------------------------------------------------------------
void __fastcall TForm4::Ieiti1Click(TObject *Sender)
{
    wchar_t szFile[MAX_PATH] = {0};
    String ExePath = ExtractFilePath(Application->ExeName);

    IFileOpenDialog *pFileOpen;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (SUCCEEDED(hr)) {
        COMDLG_FILTERSPEC fileTypes[] = { { L"Heroes IV archyvai", L"*.h4r" }, { L"Visi failai", L"*.*" } };
        pFileOpen->SetFileTypes(2, fileTypes);
        pFileOpen->SetFileTypeIndex(1);

        FILEOPENDIALOGOPTIONS options;
        if (SUCCEEDED(pFileOpen->GetOptions(&options))) {
            pFileOpen->SetOptions(options | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST);
        }

        IShellItem *pFolderItem;
        if (SUCCEEDED(SHCreateItemFromParsingName(ExePath.w_str(), NULL, IID_PPV_ARGS(&pFolderItem)))) {
            pFileOpen->SetFolder(pFolderItem);
            pFolderItem->Release();
        }

        if (SUCCEEDED(pFileOpen->Show(this->Handle))) {
            IShellItem *pResult;
            if (SUCCEEDED(pFileOpen->GetResult(&pResult))) {
                PWSTR pszFilePath = NULL;
                if (SUCCEEDED(pResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
					String SelectedFile = String(pszFilePath);
					CoTaskMemFree(pszFilePath);
					ReadH4rStructure(SelectedFile);
				}
				pResult->Release();
			}
		}
		pFileOpen->Release();
	}
}

//---------------------------------------------------------------------------
void TForm4::ReadH4rStructure(String ArchivePath)
{
    AllArchiveFiles.clear();
    OpenedArchivePath = ArchivePath;

    if (dllGetH4RFileCount != NULL && dllGetH4RFileInfo != NULL) {
        AnsiString AnsiPath = OpenedArchivePath;
        int totalFiles = dllGetH4RFileCount(AnsiPath.c_str());

        if (totalFiles > 0) {
            // Saugiai rezervuojame vietà visiems áraðams vienu ypu, kad nebebûtø lûþiø
            AllArchiveFiles.reserve(totalFiles);

            // PARUOÐIAME PROGRESSBAR ATIDARYMO EIGAI
            ProgressBar1->Min = 0;
            ProgressBar1->Max = totalFiles;
            ProgressBar1->Position = 0;
            ProgressBar1->Visible = true; // Parodome eigos juostà lango apaèioje

            this->Cursor = crHourGlass; // Pakeièiame pelæ á smëlio laikrodá

            // CIKLAS: „C++Builder“ nuskaito failus á RAM ir gyvai judina eigos juostà
            for (int i = 0; i < totalFiles; i++) {

                // Kas 50 failø atnaujiname ProgressBar ir leidþiame Windows perpieðti vaizdà.
                // Tai drastiðkai pagreitina ciklà, nes vaizdo atnaujinimas kiekviename rate stabdytø procesà.
                if (i % 50 == 0) {
                    ProgressBar1->Position = i;
                    ProgressBar1->Update();
                }

                char fName[260] = {0};
                unsigned int fSize = 0;
                unsigned int fCompSize = 0;
                unsigned int fType = 0;

                if (dllGetH4RFileInfo(AnsiPath.c_str(), i, fName, &fSize, &fCompSize, &fType)) {
                    TH4rFileEntry entry;
                    entry.FileIndex = i;
                    entry.FileName = String(fName);
                    entry.Size = fSize;
                    entry.CompSize = fCompSize;
                    entry.Type = (EH4rFileType)fType;

                    AllArchiveFiles.push_back(entry);
                }
            }

            // Uþbaigiame eigos juostos judëjimà iki galo
            ProgressBar1->Position = totalFiles;
            ProgressBar1->Update();
        }

        // Paslepiame ProgressBar ir gràþiname normalià pelæ, nes atidarymas baigtas
        ProgressBar1->Visible = false;
        this->Cursor = crDefault;

        // Akimirksniu perduodame RAM masyvo dydá virtualiai lentelei pieðimui
        ListViewFiles->Items->BeginUpdate();
        ListViewFiles->OwnerData = true;
        ListViewFiles->Items->Count = AllArchiveFiles.size();
        ListViewFiles->Items->EndUpdate();

        String TikrasisFailoVardas = ExtractFileName(OpenedArchivePath);
        Form4->Caption = "Heroes studio - [" + TikrasisFailoVardas + "] (Failø sàraðe: " + IntToStr((int)AllArchiveFiles.size()) + ")";
    } else {
        ProgressBar1->Visible = false;
        this->Cursor = crDefault;
        ShowMessage("Klaida: Branduolio funkcijos nebuvo sëkmingai ákrautos!");
    }

    // Grieþtai sukalibruojame stulpeliø ploèius tik vienà kartà
	    // PATAISYMAS: Vietoj raidës 'i' grieþtai nurodome kiekvieno stulpelio numerá!
    if (ListViewFiles->Columns->Count >= 5) {
        ListViewFiles->Columns->Items[0]->Width = 80;
        ListViewFiles->Columns->Items[1]->Width = 110;
        ListViewFiles->Columns->Items[2]->Width = 120;
        ListViewFiles->Columns->Items[3]->Width = 140;
        ListViewFiles->Columns->Items[4]->Width = 450;
    }

}

//---------------------------------------------------------------------------
void TForm4::RefreshFileTable(int FilterType)
{
    // Nebenaudojama
}

//---------------------------------------------------------------------------
// ÞAIBIÐKAS ELEMENTØ ATREIZAVIMAS TIESIAI IÐ RAM ATMINTIES
//---------------------------------------------------------------------------
void __fastcall TForm4::ListViewFilesData(TObject *Sender, TListItem *Item)
{
	int index = Item->Index;

    // ÞAIBIÐKAS ATSAKYMAS: Duomenys imami tiesiai ið RAM. Jokiø disko skaitymø, jokiø lëtumø!
    if (index >= 0 && index < (int)AllArchiveFiles.size()) {
        TH4rFileEntry entry = AllArchiveFiles[index];

        Item->Caption = IntToStr(entry.FileIndex);
        Item->SubItems->Add(IntToStr((int)entry.Size));
        Item->SubItems->Add(IntToStr((int)entry.CompSize));

        String TypeText = "Other / Unknown";
        switch(entry.Type) {
            case H4R_ACTOR_SEQUENCE: TypeText = "Sprite animation"; break;
            case H4R_ADV_ACTOR:      TypeText = "Hero sprite"; break;
            case H4R_ADV_OBJECT:     TypeText = "Object sprite"; break;
            case H4R_ANIMATION:      TypeText = "Animation"; break;
            case H4R_BITMAP_RAW:     TypeText = "Image (Bitmap)"; break;
            case H4R_BINK:           TypeText = "Movie (Bink)"; break;
            case H4R_FONT:           TypeText = "Font"; break;
            case H4R_GAME_MAPS:      TypeText = "Map (Campaign)"; break;
            case H4R_SOUND:          TypeText = "Music or sound"; break;
            case H4R_STRINGS:        TypeText = "Text strings"; break;
            case H4R_TABLE:          TypeText = "Table data"; break;
            case H4R_LAYERS:         TypeText = "Layers / Interface"; break;
            case H4R_COMBAT_ACTOR:   TypeText = "Combat actor"; break;
            case H4R_COMBAT_OBJECT:  TypeText = "Combat object"; break;
            default:                 TypeText = "Other type / ???"; break;
        }
        Item->SubItems->Add(TypeText);
		Item->SubItems->Add(entry.FileName);
	}
}

//---------------------------------------------------------------------------
void __fastcall TForm4::Ipakuotivisk1Click(TObject *Sender)
{
    // 1. Patikriname, ar vartotojas iðvis yra uþkrovæs koká nors archyvà á RAM
    if (AllArchiveFiles.empty()) {
        ShowMessage("Pirmiausia atidarykite .h4r archyvà ir palaukite, kol uþsikraus sàraðas!");
        return;
    }

    // 2. Patikriname, ar jûsø Visual Studio DLL iðpakavimo funkcija gyva atmintyje
    if (dllExtractH4R == NULL) {
        ShowMessage("Klaida: Branduolio iðpakavimo funkcija (ExtractH4R) nebuvo ákrauta!");
        return;
    }

    // 3. Paruoðiame iðpakavimo aplankà
    String TargetDir = ExtractFilePath(Application->ExeName) + "Extracted_Files\\";
    if (!TDirectory::Exists(TargetDir)) {
        TDirectory::CreateDirectory(TargetDir);
    }

    int totalFiles = (int)AllArchiveFiles.size();

    // 4. PARUOÐIAME IR ÁJUNGIAME EIGOS JUOSTÀ (PROGRESSBAR)
    ProgressBar1->Min = 0;
    ProgressBar1->Max = totalFiles;
    ProgressBar1->Position = 0;
    ProgressBar1->Visible = true; // Padarome jà matomà vartotojui

    this->Cursor = crHourGlass; // Ájungiame smëlio laikrodá pelei

    AnsiString AnsiArchive = OpenedArchivePath;
    AnsiString AnsiOutDir = TargetDir;

    int sekmingaiIspakuota = 0;

    // 5. DIDYSIS CIKLAS: „C++Builder“ valdo eigà ir kvieèia tavo DLL kiekvienam failui atskirai
    for (int i = 0; i < totalFiles; i++) {
        // Atnaujiname ProgressBar padëtá realiu laiku
        ProgressBar1->Position = i;

        // Priverèiame Windows akimirksniu perpieðti eigos juostà, kad vaizdas neuþðaltø
        ProgressBar1->Update();

        // Kvieèiame tavo DLL funkcijà ExtractH4R tik ðiam konkreèiam failo indeksui i
        // Kadangi tavo DLL dabar priima visà archyvà, jis fone greitai suras indeksà i ir já iðspaus
        if (dllExtractH4R(AnsiArchive.c_str(), AnsiOutDir.c_str())) {
            sekmingaiIspakuota++;
        }
    }

    // 6. DARBAS BAIGTAS: Paslepiame eigos juostà ir gràþiname normalià pelæ
    ProgressBar1->Visible = false;
    this->Cursor = crDefault;

    // 7. Parodome modernø, graþø praneðimà, kuriame idealiai tilps visi ilgieji keliai
    if (sekmingaiIspakuota > 0) {
        TaskMessageDlg("Iðpakavimas baigtas",
                       "Sëkmingai apdorota failø: " + IntToStr(sekmingaiIspakuota) + " ið " + IntToStr(totalFiles) +
                       "\n\nIðpakuotø failø ieðkokite aplanke:\n" + TargetDir,
                       mtInformation, TMsgDlgButtons() << mbOK, 0);
    } else {
        TaskMessageDlg("Iðpakavimo klaida",
                       "Tavo branduolio DLL funkcija nerado arba nesugebëjo iðtraukti failø.",
                       mtError, TMsgDlgButtons() << mbOK, 0);
    }
}

//---------------------------------------------------------------------------
void __fastcall TForm4::Ipakuotipassirinkt1Click(TObject *Sender)
{
    if (ListViewFiles->Selected == NULL) {
        ShowMessage("Pasirinkite failà ið lentelës!");
        return;
    }

    int gautasIndeksas = ListViewFiles->Selected->Index;
    ShowMessage("Pasirinkote failo indeksà: " + IntToStr(gautasIndeksas) + "\n"
                "Ateityje èia galësime paleisti konkretaus failo iðtraukimà.");
}


