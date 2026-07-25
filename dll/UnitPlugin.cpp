//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitPlugin.h"
#include <algorithm>
#include <System.SysUtils.hpp>
#include <System.Types.hpp>
#include <windows.h>

#pragma package(smart_init)

// ==========================================================================
// Constructor / Destructor
// ==========================================================================
__fastcall TPluginForm::TPluginForm(TComponent* Owner)
    : TForm(Owner), hCoreDll(NULL), dllGetH4RFileCount(NULL),
      dllGetH4RFileInfo(NULL), dllExtractH4RByIndices(NULL), dllPackH4R(NULL),
      FSortColumn(-1), FSortAscending(true)
{
    Caption = L"Heroes 4 Unpack/Pack";
    Width = 900;
    Height = 600;
    Position = poScreenCenter;

    // ----- Create UI components -----
    // Top panel
    TPanel *TopPanel = new TPanel(this);
    TopPanel->Parent = this;
    TopPanel->Align = alTop;
    TopPanel->Height = 40;
    TopPanel->BevelOuter = bvNone;

    BtnOpen = new TButton(TopPanel);
    BtnOpen->Parent = TopPanel;
    BtnOpen->Caption = L"Atidaryti .h4r";
    BtnOpen->Left = 10;
    BtnOpen->Top = 8;
    BtnOpen->Width = 100;
    BtnOpen->OnClick = BtnOpenClick;

    EditSearch = new TEdit(TopPanel);
    EditSearch->Parent = TopPanel;
    EditSearch->Left = 120;
    EditSearch->Top = 9;
    EditSearch->Width = 200;
    EditSearch->Text = L"";
    EditSearch->OnChange = EditSearchChange;

    BtnExtractSel = new TButton(TopPanel);
    BtnExtractSel->Parent = TopPanel;
    BtnExtractSel->Caption = L"Išpakuoti pasirinktus";
    BtnExtractSel->Left = 340;
    BtnExtractSel->Top = 8;
    BtnExtractSel->Width = 150;
    BtnExtractSel->OnClick = BtnExtractSelClick;

    BtnExtractAll = new TButton(TopPanel);
    BtnExtractAll->Parent = TopPanel;
    BtnExtractAll->Caption = L"Išpakuoti viską";
    BtnExtractAll->Left = 500;
    BtnExtractAll->Top = 8;
    BtnExtractAll->Width = 130;
    BtnExtractAll->OnClick = BtnExtractAllClick;

    BtnPack = new TButton(TopPanel);
    BtnPack->Parent = TopPanel;
    BtnPack->Caption = L"Supakuoti į /packed";
    BtnPack->Left = 640;
    BtnPack->Top = 8;
    BtnPack->Width = 140;
    BtnPack->OnClick = BtnPackClick;

    // Grid
    GridFiles = new TStringGrid(this);
    GridFiles->Parent = this;
    GridFiles->Align = alClient;
    GridFiles->FixedCols = 0;
    GridFiles->FixedRows = 1;
    GridFiles->Options = GridFiles->Options << goRowSelect << goMultiSelect;
    GridFiles->DefaultRowHeight = 20;
    GridFiles->OnDrawCell = GridFilesDrawCell;
    GridFiles->OnHeaderClick = GridFilesHeaderClick;
    GridFiles->OnSelectCell = GridFilesSelectCell;

    GridFiles->ColCount = 5;
    GridFiles->ColWidths[0] = 70;   // ID
    GridFiles->ColWidths[1] = 100;  // Size
    GridFiles->ColWidths[2] = 100;  // Compressed
    GridFiles->ColWidths[3] = 130;  // Type
    GridFiles->ColWidths[4] = 400;  // Filename

    // Set default headers (will be replaced by localization)
    GridFiles->Cells[0][0] = L"ID";
    GridFiles->Cells[1][0] = L"Size";
    GridFiles->Cells[2][0] = L"Compressed";
    GridFiles->Cells[3][0] = L"Type";
    GridFiles->Cells[4][0] = L"Filename";

    // Progress bar
    ProgressBar1 = new TProgressBar(this);
    ProgressBar1->Parent = this;
    ProgressBar1->Align = alBottom;
    ProgressBar1->Height = 24;
    ProgressBar1->Visible = false;

    // Dialogs
    OpenDialog1 = new TOpenDialog(this);
    OpenDialog1->Filter = L"Heroes IV archives (*.h4r)|*.h4r|Visi failai (*.*)|*.*";
    OpenDialog1->InitialDir = ExtractFilePath(ParamStr(0));

    SaveDialog1 = new TSaveDialog(this);
    SaveDialog1->Filter = L"Heroes IV archives (*.h4r)|*.h4r|Visi failai (*.*)|*.*";

    // Load core DLL
    LoadCoreDll();

    // Language will be set via ExecutePlugin
}

//---------------------------------------------------------------------------
__fastcall TPluginForm::~TPluginForm()
{
    UnloadCoreDll();
}

// ==========================================================================
// Core DLL loading
// ==========================================================================
bool TPluginForm::LoadCoreDll()
{
    wchar_t szPath[MAX_PATH];
    GetModuleFileNameW((HMODULE)(HINSTANCE)this->ModuleHandle, szPath, MAX_PATH);
    String pluginDir = ExtractFilePath(String(szPath));
    String corePath = pluginDir + L"core\\mh4_core.dll";

    hCoreDll = LoadLibraryW(corePath.c_str());
    if (!hCoreDll) {
        ShowMessage(L"Klaida: Nepavyko užkrauti core/mh4_core.dll!");
        return false;
    }

    dllGetH4RFileCount = (GetCountFunc)GetProcAddress(hCoreDll, "GetH4RFileCount");
    dllGetH4RFileInfo  = (GetInfoFunc)GetProcAddress(hCoreDll, "GetH4RFileInfo");
    dllExtractH4RByIndices = (ExtractIndicesFunc)GetProcAddress(hCoreDll, "ExtractH4RByIndices");
    dllPackH4R         = (PackFunc)GetProcAddress(hCoreDll, "PackH4R");

    if (!dllGetH4RFileCount || !dllGetH4RFileInfo || !dllExtractH4RByIndices || !dllPackH4R) {
        ShowMessage(L"Klaida: mh4_core.dll trūksta reikiamų funkcijų!");
        FreeLibrary(hCoreDll);
        hCoreDll = NULL;
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
void TPluginForm::UnloadCoreDll()
{
    if (hCoreDll) {
        FreeLibrary(hCoreDll);
        hCoreDll = NULL;
    }
}

// ==========================================================================
// Language loading
// ==========================================================================
void TPluginForm::LoadLanguage(const String& langCode)
{
    CurrentLangCode = langCode;

    String langDir = ExtractFilePath(Application->ExeName) + L"lang\\";
    String langFile = langDir + langCode + L".ini";
    if (!FileExists(langFile)) {
        // Fallback to english
        langFile = langDir + L"english.ini";
        if (!FileExists(langFile)) return;
    }

    TIniFile *ini = new TIniFile(langFile);

    // Update grid headers
    GridFiles->Cells[0][0] = ini->ReadString(L"h4unpack_pack", L"Col_ID", L"ID");
    GridFiles->Cells[1][0] = ini->ReadString(L"h4unpack_pack", L"Col_Size", L"Size");
    GridFiles->Cells[2][0] = ini->ReadString(L"h4unpack_pack", L"Col_Compressed", L"Compressed");
    GridFiles->Cells[3][0] = ini->ReadString(L"h4unpack_pack", L"Col_Type", L"Type");
    GridFiles->Cells[4][0] = ini->ReadString(L"h4unpack_pack", L"Col_Filename", L"Filename");

    // Buttons
    BtnOpen->Caption = ini->ReadString(L"h4unpack_pack", L"Btn_Open", L"Open .h4r");
    BtnExtractSel->Caption = ini->ReadString(L"h4unpack_pack", L"Btn_ExtractSelected", L"Extract selected");
    BtnExtractAll->Caption = ini->ReadString(L"h4unpack_pack", L"Btn_ExtractAll", L"Extract all");
    BtnPack->Caption = ini->ReadString(L"h4unpack_pack", L"Btn_Pack", L"Pack to /packed");

    // Form caption
    Caption = ini->ReadString(L"h4unpack_pack", L"Form_Caption", L"Heroes 4 Unpack/Pack");

    delete ini;
}

// ==========================================================================
// Archive reading
// ==========================================================================
void TPluginForm::ReadH4rStructure(const String& ArchivePath)
{
    AllFiles.clear();
    OpenedArchivePath = ArchivePath;

    if (!dllGetH4RFileCount || !dllGetH4RFileInfo) {
        ShowMessage(L"Core functions not loaded!");
        return;
    }

    AnsiString AnsiPath = ArchivePath;
    int totalFiles = dllGetH4RFileCount(AnsiPath.c_str());
    if (totalFiles <= 0) {
        ShowMessage(L"Archive is empty or invalid.");
        return;
    }

    AllFiles.reserve(totalFiles);
    ProgressBar1->Min = 0;
    ProgressBar1->Max = totalFiles;
    ProgressBar1->Position = 0;
    ProgressBar1->Visible = true;
    this->Cursor = crHourGlass;

    for (int i = 0; i < totalFiles; i++) {
        if (i % 50 == 0) {
            ProgressBar1->Position = i;
            ProgressBar1->Update();
            Application->ProcessMessages();
        }

        char fName[260] = {0};
        unsigned int fSize = 0, fCompSize = 0, fType = 0;
        if (dllGetH4RFileInfo(AnsiPath.c_str(), i, fName, &fSize, &fCompSize, &fType)) {
            TFileEntry entry;
            entry.ID = i;
            entry.FileName = String(fName);
            entry.Size = fSize;
            entry.CompSize = fCompSize;
            entry.Type = (EH4rFileType)fType;
            AllFiles.push_back(entry);
        }
    }

    ProgressBar1->Position = totalFiles;
    ProgressBar1->Visible = false;
    this->Cursor = crDefault;

    ApplyFilter(EditSearch->Text);
    UpdateCaption();
}

// ==========================================================================
// Filtering and sorting
// ==========================================================================
void TPluginForm::ApplyFilter(const String& searchText)
{
    FilteredFiles.clear();

    if (searchText.IsEmpty()) {
        FilteredFiles = AllFiles;
    } else {
        String lowerSearch = searchText.LowerCase();
        for (const auto& entry : AllFiles) {
            if (entry.FileName.LowerCase().Pos(lowerSearch) > 0) {
                FilteredFiles.push_back(entry);
            }
        }
    }

    ApplySort();
    UpdateGrid();
}

//---------------------------------------------------------------------------
void TPluginForm::UpdateGrid()
{
    GridFiles->RowCount = FilteredFiles.size() + 1;
    GridFiles->Invalidate();
}

//---------------------------------------------------------------------------
void TPluginForm::ApplySort()
{
    if (FilteredFiles.empty() || FSortColumn == -1)
        return;

    std::sort(FilteredFiles.begin(), FilteredFiles.end(),
        [this](const TFileEntry& e1, const TFileEntry& e2) {
            int result = 0;
            switch (FSortColumn) {
                case 0: result = (e1.ID < e2.ID) ? -1 : (e1.ID > e2.ID) ? 1 : 0; break;
                case 1: result = (e1.Size < e2.Size) ? -1 : (e1.Size > e2.Size) ? 1 : 0; break;
                case 2: result = (e1.CompSize < e2.CompSize) ? -1 : (e1.CompSize > e2.CompSize) ? 1 : 0; break;
                case 3: result = (e1.Type < e2.Type) ? -1 : (e1.Type > e2.Type) ? 1 : 0; break;
                case 4: result = e1.FileName.CompareIC(e2.FileName); break;
                default: result = 0;
            }
            if (!FSortAscending) result = -result;
            return result < 0;
        });

    GridFiles->Invalidate();
}

// ==========================================================================
// Grid drawing
// ==========================================================================
void __fastcall TPluginForm::GridFilesDrawCell(TObject *Sender, int ACol, int ARow,
                                               TRect &Rect, TGridDrawState State)
{
    if (ARow == 0) {
        // Header – drawn automatically
        return;
    }

    int idx = ARow - 1;
    if (idx < 0 || idx >= (int)FilteredFiles.size())
        return;

    const TFileEntry& entry = FilteredFiles[idx];

    String text;
    switch (ACol) {
        case 0: text = IntToStr(entry.ID); break;
        case 1: text = IntToStr((int)entry.Size); break;
        case 2: text = IntToStr((int)entry.CompSize); break;
        case 3: {
            switch (entry.Type) {
                case H4R_ACTOR_SEQUENCE: text = "Sprite animation"; break;
                case H4R_ADV_ACTOR:      text = "Hero sprite"; break;
                case H4R_ADV_OBJECT:     text = "Object sprite"; break;
                case H4R_ANIMATION:      text = "Animation"; break;
                case H4R_BITMAP_RAW:     text = "Image (Bitmap)"; break;
                case H4R_BINK:           text = "Movie (Bink)"; break;
                case H4R_FONT:           text = "Font"; break;
                case H4R_GAME_MAPS:      text = "Map (Campaign)"; break;
                case H4R_SOUND:          text = "Music or sound"; break;
                case H4R_STRINGS:        text = "Text strings"; break;
                case H4R_TABLE:          text = "Table data"; break;
                case H4R_LAYERS:         text = "Layers / Interface"; break;
                case H4R_COMBAT_ACTOR:   text = "Combat actor"; break;
                case H4R_COMBAT_OBJECT:  text = "Combat object"; break;
                default:                 text = "Other type"; break;
            }
            break;
        }
        case 4: text = entry.FileName; break;
        default: text = "";
    }

    GridFiles->Canvas->FillRect(Rect);
    GridFiles->Canvas->TextOut(Rect.Left + 2, Rect.Top + 2, text);
}

// ==========================================================================
// Event handlers
// ==========================================================================
void __fastcall TPluginForm::BtnOpenClick(TObject *Sender)
{
    if (OpenDialog1->Execute()) {
        ReadH4rStructure(OpenDialog1->FileName);
    }
}

//---------------------------------------------------------------------------
void __fastcall TPluginForm::BtnExtractSelClick(TObject *Sender)
{
    if (AllFiles.empty() || FilteredFiles.empty()) {
        ShowMessage(L"Please open an archive first.");
        return;
    }

    // Collect selected rows
    std::vector<int> selectedIDs;
    for (int row = 1; row < GridFiles->RowCount; row++) {
        if (GridFiles->IsSelectedRow(row)) {
            int idx = row - 1;
            if (idx >= 0 && idx < (int)FilteredFiles.size()) {
                selectedIDs.push_back(FilteredFiles[idx].ID);
            }
        }
    }

    if (selectedIDs.empty()) {
        ShowMessage(L"No files selected.");
        return;
    }

    if (!dllExtractH4RByIndices) {
        ShowMessage(L"Extract function not available.");
        return;
    }

    String outDir = ExtractFilePath(Application->ExeName) + L"Extracted_Files\\";
    if (!TDirectory::Exists(outDir)) TDirectory::CreateDirectory(outDir);

    ProgressBar1->Min = 0;
    ProgressBar1->Max = selectedIDs.size();
    ProgressBar1->Position = 0;
    ProgressBar1->Visible = true;
    this->Cursor = crHourGlass;

    AnsiString ansiArchive = OpenedArchivePath;
    AnsiString ansiOut = outDir;
    int* indices = new int[selectedIDs.size()];
    for (size_t i = 0; i < selectedIDs.size(); i++) indices[i] = selectedIDs[i];

    bool success = dllExtractH4RByIndices(ansiArchive.c_str(), ansiOut.c_str(),
                                          indices, (int)selectedIDs.size());
    delete[] indices;

    ProgressBar1->Visible = false;
    this->Cursor = crDefault;

    if (success)
        ShowMessage(L"Extracted " + IntToStr(selectedIDs.size()) + L" files to " + outDir);
    else
        ShowMessage(L"Extraction failed.");
}

//---------------------------------------------------------------------------
void __fastcall TPluginForm::BtnExtractAllClick(TObject *Sender)
{
    if (AllFiles.empty()) {
        ShowMessage(L"Please open an archive first.");
        return;
    }

    if (!dllExtractH4RByIndices) {
        ShowMessage(L"Extract function not available.");
        return;
    }

    String outDir = ExtractFilePath(Application->ExeName) + L"Extracted_Files\\";
    if (!TDirectory::Exists(outDir)) TDirectory::CreateDirectory(outDir);

    std::vector<int> allIDs;
    for (const auto& entry : FilteredFiles)
        allIDs.push_back(entry.ID);

    if (allIDs.empty()) {
        ShowMessage(L"No files to extract (filter empty).");
        return;
    }

    ProgressBar1->Min = 0;
    ProgressBar1->Max = allIDs.size();
    ProgressBar1->Position = 0;
    ProgressBar1->Visible = true;
    this->Cursor = crHourGlass;

    AnsiString ansiArchive = OpenedArchivePath;
    AnsiString ansiOut = outDir;
    int* indices = new int[allIDs.size()];
    for (size_t i = 0; i < allIDs.size(); i++) indices[i] = allIDs[i];

    bool success = dllExtractH4RByIndices(ansiArchive.c_str(), ansiOut.c_str(),
                                          indices, (int)allIDs.size());
    delete[] indices;

    ProgressBar1->Visible = false;
    this->Cursor = crDefault;

    if (success)
        ShowMessage(L"Extracted all " + IntToStr(allIDs.size()) + L" files to " + outDir);
    else
        ShowMessage(L"Extraction failed.");
}

//---------------------------------------------------------------------------
void __fastcall TPluginForm::BtnPackClick(TObject *Sender)
{
    String baseDir = ExtractFilePath(Application->ExeName) + L"Extracted_Files\\";
    if (!TDirectory::Exists(baseDir)) {
        ShowMessage(L"Extracted_Files folder not found. Please extract first.");
        return;
    }

    // Build .lst file (simplified – same as original)
    String lstPath = baseDir + L"pack.lst";
    TStringList *lst = new TStringList();
    lst->Add(L"[H4R File List]");

    String h4rName = L"repacked.h4r";
    if (!OpenedArchivePath.IsEmpty())
        h4rName = ExtractFileName(OpenedArchivePath);
    lst->Add(h4rName);

    struct PackFile { String relPath; int type; };
    std::vector<PackFile> packFiles;

    auto mapFolderToType = [](const String& folder) -> int {
        if (folder == L"strings") return H4R_STRINGS;
        if (folder == L"tables") return H4R_TABLE;
        if (folder == L"fonts") return H4R_FONT;
        if (folder == L"maps") return H4R_GAME_MAPS;
        if (folder == L"images") return H4R_BITMAP_RAW;
        if (folder == L"sound") return H4R_SOUND;
        if (folder == L"layers") return H4R_LAYERS;
        return -1;
    };

    TStringDynArray dirs = TDirectory::GetDirectories(baseDir);
    for (int d = 0; d < dirs.Length; d++) {
        String dirName = ExtractFileName(dirs[d]);
        int type = mapFolderToType(dirName);
        if (type == -1) continue;
        TStringDynArray files = TDirectory::GetFiles(dirs[d], L"*.*", TSearchOption::soAllDirectories);
        for (int f = 0; f < files.Length; f++) {
            String relPath = dirName + L"\\" + ExtractFileName(files[f]);
            packFiles.push_back({relPath, type});
        }
    }

    if (packFiles.empty()) {
        ShowMessage(L"No extracted files found in subfolders.");
        delete lst;
        return;
    }

    lst->Add(IntToStr((int)packFiles.size()));
    for (const auto& pf : packFiles)
        lst->Add(pf.relPath);

    lst->SaveToFile(lstPath);
    delete lst;

    if (!dllPackH4R) {
        ShowMessage(L"Pack function not available.");
        return;
    }

    String outDir = ExtractFilePath(Application->ExeName) + L"packed\\";
    if (!TDirectory::Exists(outDir)) TDirectory::CreateDirectory(outDir);
    String outFile = outDir + L"packed.h4r";
    if (!OpenedArchivePath.IsEmpty())
        outFile = outDir + ExtractFileName(OpenedArchivePath);

    AnsiString ansiLst = lstPath;
    AnsiString ansiOut = outFile;

    ProgressBar1->Min = 0;
    ProgressBar1->Max = 1;
    ProgressBar1->Position = 0;
    ProgressBar1->Visible = true;
    this->Cursor = crHourGlass;

    bool success = dllPackH4R(ansiLst.c_str(), ansiOut.c_str());

    ProgressBar1->Visible = false;
    this->Cursor = crDefault;

    if (success)
        ShowMessage(L"Archive packed to " + outFile);
    else
        ShowMessage(L"Packing failed.");
}

//---------------------------------------------------------------------------
void __fastcall TPluginForm::EditSearchChange(TObject *Sender)
{
    ApplyFilter(EditSearch->Text);
}

//---------------------------------------------------------------------------
void __fastcall TPluginForm::GridFilesHeaderClick(TObject *Sender, int ACol)
{
    if (FSortColumn == ACol)
        FSortAscending = !FSortAscending;
    else {
        FSortColumn = ACol;
        FSortAscending = true;
    }
    ApplySort();
}

//---------------------------------------------------------------------------
void __fastcall TPluginForm::GridFilesSelectCell(TObject *Sender, int ACol, int ARow,
                                                  bool &CanSelect)
{
    CanSelect = true;
}

//---------------------------------------------------------------------------
void TPluginForm::UpdateCaption()
{
    if (!OpenedArchivePath.IsEmpty()) {
        String name = ExtractFileName(OpenedArchivePath);
        Caption = L"Heroes 4 Unpack/Pack - [" + name + L"] (Showing: " +
                  IntToStr(FilteredFiles.size()) + L" of " +
                  IntToStr(AllFiles.size()) + L")";
    } else {
        Caption = L"Heroes 4 Unpack/Pack";
    }
}

// ==========================================================================
// Form events (empty, handled in constructor/destructor)
// ==========================================================================
void __fastcall TPluginForm::FormCreate(TObject *Sender) { }
void __fastcall TPluginForm::FormDestroy(TObject *Sender) { }

// ==========================================================================
// EXPORTED PLUGIN FUNCTIONS
// ==========================================================================

extern "C" __declspec(dllexport) void __stdcall GetPluginInfo(TPluginInfo* Info)
{
    if (Info) {
        Info->PluginName = "Heroes 4 Unpack/Pack";
        Info->Version = "1.0.0";
    }
}

extern "C" __declspec(dllexport) void __stdcall ExecutePlugin(HWND parentHWND, const char* langCode)
{
    try {
        // Create form
        TPluginForm *form = new TPluginForm(Application);
        // Set parent window for synchronization (minimize/maximize)
        if (parentHWND) {
            SetWindowLongPtr(form->Handle, GWLP_HWNDPARENT, (LONG_PTR)parentHWND);
        }
        // Load language
        if (langCode) {
            form->LoadLanguage(String(langCode));
        }
        // Show modal
        form->ShowModal();
        delete form;
    } catch (Exception &e) {
        MessageBoxA(parentHWND, e.Message.c_str(), "Plugin Error", MB_OK | MB_ICONERROR);
    }
}

// ==========================================================================
// DLL entry point
// ==========================================================================
int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    return 1;
}
//---------------------------------------------------------------------------