#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include <System.IOUtils.hpp>
#include <System.Types.hpp>
#include <ShObjIdl.h>
#include <cstdint>
#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <sstream>

#pragma package(smart_init)
#pragma resource "*.dfm"
TForm4 *Form4;

// Funkcijø tipø apraðymai áskiepiams
typedef const char* (__stdcall *TGetPluginName)();
typedef void (__stdcall *TRunPlugin)();

//---------------------------------------------------------------------------
__fastcall TForm4::TForm4(TComponent* Owner)
    : TForm(Owner)
{
}

// Bendra funkcija, kuri suveiks paspaudus BET KURIO áskiepio meniu punktà
void __fastcall TForm4::PluginMenuClick(TObject *Sender)
{
    TMenuItem *ClickedItem = dynamic_cast<TMenuItem*>(Sender);

    if (ClickedItem != NULL)
    {
        HMODULE hDll = (HMODULE)ClickedItem->Tag;

        if (hDll != NULL)
        {
            TRunPlugin RunPluginFunc = (TRunPlugin)GetProcAddress(hDll, "_RunPlugin");

            if (RunPluginFunc != NULL)
            {
                RunPluginFunc();
            }
            else
            {
                ShowMessage("Klaida: Áskiepyje nerasta 'RunPlugin' funkcija!");
            }
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TForm4::FormCreate(TObject *Sender)
{
    // 1. Priverstinai perjungiame TListView á lentelës reþimà
    ListViewFiles->ViewStyle = vsReport;

    // 2. Ájungiame stulpeliø antraðèiø matomumà
    ListViewFiles->ShowColumnHeaders = true;

    // 3. Suteikiame aiðkius pavadinimus stulpeliams grieþtai pagal jø indeksus
    if (ListViewFiles->Columns->Count >= 6)
    {
        ListViewFiles->Columns->Items[0]->Caption = "Poslinkis (Offset)";
        ListViewFiles->Columns->Items[1]->Caption = "Suspaustas dydis";
        ListViewFiles->Columns->Items[2]->Caption = "Tikrasis dydis";
        ListViewFiles->Columns->Items[3]->Caption = "Objekto tipas";
        ListViewFiles->Columns->Items[4]->Caption = "Failo pavadinimas";
        ListViewFiles->Columns->Items[5]->Caption = "Vidinis þaidimo kelias";
    }

    // Nurodome, kad OpenDialog pradinis aplankas bûtø tas pats, kur guli programos EXE
    OpenDialog1->InitialDir = ExtractFilePath(Application->ExeName);

    String PluginsPath = ExtractFilePath(Application->ExeName) + "Unpacker_System\\plugins\\";

    if (TDirectory::Exists(PluginsPath))
    {
        TStringDynArray DllFiles = TDirectory::GetFiles(PluginsPath, "*.dll");

        for (int i = 0; i < DllFiles.Length; i++)
        {
            String CurrentDll = DllFiles[i];

            HMODULE hDll = LoadLibraryW(CurrentDll.c_str());

            if (hDll != NULL)
            {
                TGetPluginName GetPluginNameFunc = (TGetPluginName)GetProcAddress(hDll, "_GetPluginName");

                if (GetPluginNameFunc != NULL)
                {
                    const char* pluginName = GetPluginNameFunc();

                    TMenuItem *NewItem = new TMenuItem(MainMenu1);
                    NewItem->Caption = String(pluginName);
                    NewItem->Tag = (NativeInt)hDll;
                    NewItem->OnClick = PluginMenuClick;

                    MenuPlugins->Add(NewItem);
                }
                else
                {
                    FreeLibrary(hDll);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TForm4::FormDestroy(TObject *Sender)
{
    if (MenuPlugins != NULL)
    {
        for (int i = MenuPlugins->Count - 1; i >= 0; i--)
        {
            TMenuItem *Item = MenuPlugins->Items[i];

            if (Item != NULL)
            {
                HMODULE hDll = (HMODULE)Item->Tag;

                if (hDll != NULL)
                {
                    FreeLibrary(hDll);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TForm4::Apie1Click(TObject *Sender)
{
    ShowMessage("Heroes IV Modding Tool v1.0\n\n"
                "Að ir DI ðitiek darbo ádëjome! :)\n"
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

    if (SUCCEEDED(hr))
    {
        COMDLG_FILTERSPEC fileTypes[] = { { L"Heroes IV archyvai", L"*.h4r" }, { L"Visi failai", L"*.*" } };
        pFileOpen->SetFileTypes(2, fileTypes);
        pFileOpen->SetFileTypeIndex(1);

        FILEOPENDIALOGOPTIONS options;
        if (SUCCEEDED(pFileOpen->GetOptions(&options)))
        {
            pFileOpen->SetOptions(options | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST);
        }

        IShellItem *pFolderItem;
        if (SUCCEEDED(SHCreateItemFromParsingName(ExePath.w_str(), NULL, IID_PPV_ARGS(&pFolderItem))))
        {
            pFileOpen->SetFolder(pFolderItem);
            pFolderItem->Release();
        }

        if (SUCCEEDED(pFileOpen->Show(this->Handle)))
        {
            IShellItem *pResult;
            if (SUCCEEDED(pFileOpen->GetResult(&pResult)))
            {
                PWSTR pszFilePath = NULL;
                if (SUCCEEDED(pResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
                {
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

    std::ifstream archive(ArchivePath.w_str(), std::ios::binary);
    if (!archive.is_open()) {
        ShowMessage("Nepavyko atidaryti .h4r archyvo!");
        return;
    }

    uint32_t magicNumber = 0;
    uint32_t dataSize = 0;
    uint32_t totalFiles = 0;

    // 1. Nuskaitome pagrindinæ 12 baitø antraðtæ
    archive.read((char*)&magicNumber, sizeof(magicNumber));
    archive.read((char*)&dataSize, sizeof(dataSize));
    archive.read((char*)&totalFiles, sizeof(totalFiles));

    archive.seekg(0, std::ios::end);
    std::streamsize fileSize = archive.tellg();

    if (totalFiles == 0 || totalFiles > 500000) {
        totalFiles = dataSize;
    }

    // Sukalibruojame pradiná katalogo taðkà
    uint32_t calculatedFatOffset = 12 + dataSize;
    if (calculatedFatOffset > 16) {
        calculatedFatOffset -= 16;
    }

    if (calculatedFatOffset >= (uint32_t)fileSize || calculatedFatOffset < 12) {
        calculatedFatOffset = 12;
    }

    archive.seekg(calculatedFatOffset, std::ios::beg);

    uint32_t currentHeaderID = 0;

    // Masinis ðvarus failø nuskaitymas
    for (uint32_t i = 0; i < totalFiles; i++) {
        if (archive.peek() == EOF || !archive.good()) {
            break;
        }

        while (archive.peek() == 0 && archive.good()) {
            char dummy;
            archive.read(&dummy, 1);
        }

        if (archive.peek() == EOF) break;

        uint32_t fileOffset = 0;
        uint32_t fileSizeAfter = 0;
        uint32_t fileSizeBefore = 0;
        uint32_t fileDateRaw = 0;

        archive.read((char*)&fileOffset, sizeof(fileOffset));
        archive.read((char*)&fileSizeAfter, sizeof(fileSizeAfter));
        archive.read((char*)&fileSizeBefore, sizeof(fileSizeBefore));
        archive.read((char*)&fileDateRaw, sizeof(fileDateRaw));

        uint16_t fileNameLength = 0;
        archive.read((char*)&fileNameLength, sizeof(fileNameLength));

        if (fileNameLength > 1000 || fileNameLength == 0) {
            continue;
        }

        // ============================================================================
        // TIKROJI BINARINË STRUKTÛRA:
        // Ciklas sukasi ir skaito pavadinimus ið to paties bloko, kol fileNameLength tap nulis.
        // ============================================================================
        while (fileNameLength != 0 && archive.good()) {
            TH4rFileEntry entry;

            entry.Offset = fileOffset;
            entry.SizeAfter = fileSizeAfter;
            entry.SizeBefore = fileSizeBefore;
            entry.FileDate = fileDateRaw;
            entry.HeaderID = currentHeaderID++;

            std::vector<char> nameBuffer(fileNameLength + 1, 0);
            archive.read(nameBuffer.data(), fileNameLength);
            entry.FileName = String(nameBuffer.data());

            uint16_t dirLength = 0;
            archive.read((char*)&dirLength, sizeof(dirLength));

            if (dirLength > 0 && dirLength < 2000) {
                std::vector<char> dirBuffer(dirLength + 1, 0);
                archive.read(dirBuffer.data(), dirLength);
                entry.DevPath = String(dirBuffer.data());
            } else {
                entry.DevPath = "";
            }

            uint16_t alwaysZero = 0;
            archive.read((char*)&alwaysZero, sizeof(alwaysZero));

            uint32_t compressionType = 1;
            archive.read((char*)&compressionType, sizeof(compressionType));

            // Tipo atpaþinimas per saugø .Pos()
            String NameUpper = entry.FileName.UpperCase();

            if (NameUpper.Pos("FONT.") == 1) entry.Type = H4R_FONT;
            else if (NameUpper.Pos("LAYERS.") == 1) entry.Type = H4R_LAYERS;
            else if (NameUpper.Pos("ANIMATION.") == 1) entry.Type = H4R_ANIMATION;
            else if (NameUpper.Pos("ADV_ACTOR.") == 1) entry.Type = H4R_ADV_ACTOR;
            else if (NameUpper.Pos("ADV_OBJECT.") == 1) entry.Type = H4R_ADV_OBJECT;
            else if (NameUpper.Pos("ACTOR_SEQUENCE.") == 1) entry.Type = H4R_ACTOR_SEQUENCE;
            else if (NameUpper.Pos("COMBAT_ACTOR.") == 1) entry.Type = H4R_COMBAT_ACTOR;
            else if (NameUpper.Pos("COMBAT_OBJECT.") == 1) entry.Type = H4R_COMBAT_OBJECT;
            else if (NameUpper.Pos("TABLE.") == 1) entry.Type = H4R_TABLE;
            else if (NameUpper.Pos("STRINGS.") == 1) entry.Type = H4R_STRINGS;
            else if (NameUpper.Pos(".WAV") > 0 || NameUpper.Pos(".MP3") > 0) entry.Type = H4R_SOUND;
            else if (NameUpper.Pos(".BIK") > 0 || NameUpper.Pos(".BINK") > 0) entry.Type = H4R_BINK;
            else if (NameUpper.Pos(".H4M") > 0 || NameUpper.Pos(".H4C") > 0) entry.Type = H4R_GAME_MAPS;
            else if (NameUpper.Pos("BITMAP.") == 1 || NameUpper.Pos(".RAW") > 0) entry.Type = H4R_BITMAP_RAW;
            else entry.Type = H4R_NOTHING;

            AllArchiveFiles.push_back(entry);

            // GRIEÞTAS REÞIMO GRÀÞINIMAS:
            // Jei tai normalus failas (offset nelygus 0), jo paketas baigtas - iðkart iðvalome ilgá,
            // kad iðeitume ið while ciklo ir FOR judëtø toliau.
            // Jei tai nulinis kataloginis áraðas, skaitome sekantá ilgá!
            if (fileOffset != 0 || fileSizeAfter != 0) {
                fileNameLength = 0;
            } else {
                archive.read((char*)&fileNameLength, sizeof(fileNameLength));
                if (fileNameLength > 1000) break;
            }
        }
    }

    archive.close();

    Form4->Caption = "Heroes IV Unpacker - Uþkrauta áraðø: " + IntToStr((int)AllArchiveFiles.size());

    RefreshFileTable((int)H4R_ALL);
}

//---------------------------------------------------------------------------
void TForm4::RefreshFileTable(int FilterType)
{
    ListViewFiles->Items->BeginUpdate();
    ListViewFiles->Items->Clear();

    for (size_t i = 0; i < AllArchiveFiles.size(); i++)
    {
        TH4rFileEntry entry = AllArchiveFiles[i];

        if (FilterType != (int)H4R_ALL && FilterType != -1 && (int)entry.Type != FilterType) {
            continue;
        }

        TListItem *ListItem = ListViewFiles->Items->Add();

        ListItem->Caption = IntToStr((int)entry.Offset);
        ListItem->SubItems->Add(IntToStr((int)entry.SizeAfter));
        ListItem->SubItems->Add(IntToStr((int)entry.SizeBefore));

        String TypeText = "Unknown";
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

        ListItem->SubItems->Add(TypeText);
        ListItem->SubItems->Add(entry.FileName);
        ListItem->SubItems->Add(entry.DevPath);
    }

	ListViewFiles->Items->EndUpdate();

	for (int i = 0; i < ListViewFiles->Columns->Count; i++) {
		ListViewFiles->Columns->Items[i]->Width = -1;
		ListViewFiles->Columns->Items[i]->Width = -2;
	}
}

