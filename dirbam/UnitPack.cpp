//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "UnitPack.h"
#include <algorithm>
#include <cstring>
#include <System.IOUtils.hpp>
#include <ShlObj.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TFormPack *FormPack;
//---------------------------------------------------------------------------
__fastcall TFormPack::TFormPack(TComponent* Owner)
    : TForm(Owner), hCoreDll(NULL), dllGetCount(NULL), dllGetInfo(NULL),
      dllExtractByIndices(NULL), SortColumn(-1), SortAscending(true)
{
    Caption = L"H4 Archive Tool";
    Width = 850;
    Height = 550;
    Position = poScreenCenter;

    // Grid
    GridFiles = new TStringGrid(this);
    GridFiles->Parent = this;
    GridFiles->Align = alClient;
    GridFiles->FixedCols = 0;
    GridFiles->FixedRows = 1;
    GridFiles->Options = GridFiles->Options << goRowSelect << goMultiSelect;
    GridFiles->DefaultRowHeight = 22;
    GridFiles->ColCount = 4;
    GridFiles->ColWidths[0] = 60;
    GridFiles->ColWidths[1] = 300;
    GridFiles->ColWidths[2] = 100;
    GridFiles->ColWidths[3] = 100;
    GridFiles->Cells[0][0] = L"ID";
    GridFiles->Cells[1][0] = L"Pavadinimas";
    GridFiles->Cells[2][0] = L"Dydis";
    GridFiles->Cells[3][0] = L"Tipas";
    GridFiles->OnDrawCell = GridFilesDrawCell;
    GridFiles->OnHeaderClick = GridFilesHeaderClick;
    GridFiles->OnSelectCell = GridFilesSelectCell;

    // Viršutinis skydelis
    TPanel *TopPanel = new TPanel(this);
    TopPanel->Parent = this;
    TopPanel->Align = alTop;
    TopPanel->Height = 45;
    TopPanel->BevelOuter = bvNone;

    // Paieškos laukas
    TLabel *LblSearch = new TLabel(TopPanel);
    LblSearch->Parent = TopPanel;
    LblSearch->Caption = L"Paieška:";
    LblSearch->Left = 10;
    LblSearch->Top = 16;

    EditSearch = new TEdit(TopPanel);
    EditSearch->Parent = TopPanel;
    EditSearch->Left = 80;
    EditSearch->Top = 12;
    EditSearch->Width = 250;
    EditSearch->OnChange = EditSearchChange;

    // Išpakavimo mygtukas
    BtnExtract = new TButton(TopPanel);
    BtnExtract->Parent = TopPanel;
    BtnExtract->Caption = L"Išpakuoti pasirinktus";
    BtnExtract->Left = 360;
    BtnExtract->Top = 10;
    BtnExtract->Width = 160;
    BtnExtract->OnClick = BtnExtractClick;

    // Atidarymo mygtukas
    TButton *BtnOpen = new TButton(TopPanel);
    BtnOpen->Parent = TopPanel;
    BtnOpen->Caption = L"Atidaryti .h4r";
    BtnOpen->Left = 540;
    BtnOpen->Top = 10;
    BtnOpen->Width = 120;
    BtnOpen->OnClick = BtnOpenClick; // deklaruosime vėliau

    // Progreso juosta
    ProgressBar1 = new TProgressBar(this);
    ProgressBar1->Parent = this;
    ProgressBar1->Align = alBottom;
    ProgressBar1->Height = 24;
    ProgressBar1->Visible = false;

    // Dialogas
    OpenDialog1 = new TOpenDialog(this);
    OpenDialog1->Filter = L"Heroes IV archives (*.h4r)|*.h4r|Visi failai (*.*)|*.*";

    // Įkelti core DLL
    LoadCoreDll();
}
//---------------------------------------------------------------------------
__fastcall TFormPack::~TFormPack()
{
    UnloadCoreDll();
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::SetLanguageFile(const std::wstring& langFilePath)
{
    LangFilePath = langFilePath;
    LoadLanguage(langFilePath);
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::LoadLanguage(const std::wstring& langFilePath)
{
    if (!FileExists(langFilePath.c_str())) return;

    TMemIniFile *ini = new TMemIniFile(langFilePath.c_str(), TEncoding::UTF8);
    try {
        Caption = ini->ReadString(L"h4unpack_pack", L"FormCaption", L"H4 Archive Tool").w_str();
        BtnExtract->Caption = ini->ReadString(L"h4unpack_pack", L"BtnExtract_Caption", L"Extract selected").w_str();
        GridFiles->Cells[0][0] = ini->ReadString(L"h4unpack_pack", L"Grid_Col0", L"ID").w_str();
        GridFiles->Cells[1][0] = ini->ReadString(L"h4unpack_pack", L"Grid_Col1", L"File name").w_str();
        GridFiles->Cells[2][0] = ini->ReadString(L"h4unpack_pack", L"Grid_Col2", L"Size").w_str();
        GridFiles->Cells[3][0] = ini->ReadString(L"h4unpack_pack", L"Grid_Col3", L"Type").w_str();
    } __finally {
        delete ini;
    }
}
//---------------------------------------------------------------------------
bool __fastcall TFormPack::LoadCoreDll()
{
    wchar_t szPath[MAX_PATH];
    GetModuleFileNameW((HMODULE)(HINSTANCE)this->ModuleHandle, szPath, MAX_PATH);
    std::wstring pluginDir = ExtractFilePath(szPath).w_str();
    std::wstring corePath = pluginDir + L"core\\mh4_core.dll";

    hCoreDll = LoadLibraryW(corePath.c_str());
    if (!hCoreDll) {
        ShowMessage(L"Klaida: Nepavyko užkrauti plugins/core/mh4_core.dll!");
        return false;
    }

    dllGetCount = (TGetCountFunc)GetProcAddress(hCoreDll, "GetH4RFileCount");
    dllGetInfo = (TGetInfoFunc)GetProcAddress(hCoreDll, "GetH4RFileInfo");
    dllExtractByIndices = (TExtractByIndicesFunc)GetProcAddress(hCoreDll, "ExtractH4RByIndices");

    if (!dllGetCount || !dllGetInfo || !dllExtractByIndices) {
        ShowMessage(L"Klaida: mh4_core.dll trūksta reikiamų funkcijų!");
        FreeLibrary(hCoreDll);
        hCoreDll = NULL;
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::UnloadCoreDll()
{
    if (hCoreDll) {
        FreeLibrary(hCoreDll);
        hCoreDll = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::OpenArchive(const std::wstring& archivePath)
{
    AllFiles.clear();
    FilteredIndices.clear();

    if (!dllGetCount || !dllGetInfo) {
        ShowMessage(L"Core DLL funkcijos neįkrautos!");
        return;
    }

    std::string ansiPath = UTF8String(archivePath.c_str()).c_str();
    int totalFiles = dllGetCount(ansiPath.c_str());
    if (totalFiles <= 0) {
        ShowMessage(L"Archyvas tuščias arba netinkamas.");
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
        if (dllGetInfo(ansiPath.c_str(), i, fName, &fSize, &fCompSize, &fType)) {
            TFileEntry entry;
            entry.Index = i;
            entry.FileName = UTF8String(fName).w_str();
            entry.Size = fSize;
            entry.CompSize = fCompSize;
            entry.Type = fType;
            AllFiles.push_back(entry);
            FilteredIndices.push_back((int)AllFiles.size() - 1);
        }
    }

    ProgressBar1->Visible = false;
    this->Cursor = crDefault;

    ApplySort();
    UpdateGrid();
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::ApplyFilter()
{
    std::wstring searchText = EditSearch->Text.w_str();
    FilteredIndices.clear();

    if (searchText.empty()) {
        for (int i = 0; i < (int)AllFiles.size(); i++) {
            FilteredIndices.push_back(i);
        }
    } else {
        std::wstring lowerSearch = searchText;
        std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::towlower);
        for (int i = 0; i < (int)AllFiles.size(); i++) {
            std::wstring fileName = AllFiles[i].FileName;
            std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::towlower);
            if (fileName.find(lowerSearch) != std::wstring::npos) {
                FilteredIndices.push_back(i);
            }
        }
    }

    ApplySort();
    UpdateGrid();
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::ApplySort()
{
    if (FilteredIndices.empty() || SortColumn == -1) return;

    std::sort(FilteredIndices.begin(), FilteredIndices.end(),
        [this](int a, int b) {
            const TFileEntry& e1 = AllFiles[a];
            const TFileEntry& e2 = AllFiles[b];
            int result = 0;
            switch (SortColumn) {
                case 0: // ID
                    if (e1.Index < e2.Index) result = -1;
                    else if (e1.Index > e2.Index) result = 1;
                    break;
                case 1: // Failo pavadinimas
                    result = e1.FileName.compare(e2.FileName);
                    break;
                case 2: // Dydis
                    if (e1.Size < e2.Size) result = -1;
                    else if (e1.Size > e2.Size) result = 1;
                    break;
                case 3: // Tipas
                    if (e1.Type < e2.Type) result = -1;
                    else if (e1.Type > e2.Type) result = 1;
                    break;
                default: result = 0;
            }
            if (!SortAscending) result = -result;
            return result < 0;
        });
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::UpdateGrid()
{
    GridFiles->RowCount = FilteredIndices.size() + 1;
    GridFiles->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::GridFilesDrawCell(TObject *Sender, int ACol, int ARow,
    const TRect &Rect, TGridDrawState State)
{
    if (ARow == 0) return;

    int idx = ARow - 1;
    if (idx < 0 || idx >= (int)FilteredIndices.size()) return;

    const TFileEntry& entry = AllFiles[FilteredIndices[idx]];

    std::wstring text;
    switch (ACol) {
        case 0: text = std::to_wstring(entry.Index); break;
        case 1: text = entry.FileName; break;
        case 2: text = std::to_wstring(entry.Size); break;
        case 3: text = std::to_wstring(entry.Type); break;
        default: text = L"";
    }

    GridFiles->Canvas->FillRect(Rect);
    GridFiles->Canvas->TextOut(Rect.Left + 2, Rect.Top + 2, text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::GridFilesHeaderClick(TObject *Sender, int ACol)
{
    if (SortColumn == ACol) {
        SortAscending = !SortAscending;
    } else {
        SortColumn = ACol;
        SortAscending = true;
    }
    ApplySort();
    UpdateGrid();
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::GridFilesSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect)
{
    CanSelect = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::EditSearchChange(TObject *Sender)
{
    ApplyFilter();
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::BtnExtractClick(TObject *Sender)
{
    if (AllFiles.empty()) {
        ShowMessage(L"Pirmiausia atidarykite archyvą.");
        return;
    }

    // Surinkti pasirinktų eilučių indeksus
    std::vector<int> selectedIndices;
    for (int row = 1; row < GridFiles->RowCount; row++) {
        if (GridFiles->IsSelectedRow(row)) {
            int idx = row - 1;
            if (idx >= 0 && idx < (int)FilteredIndices.size()) {
                selectedIndices.push_back(AllFiles[FilteredIndices[idx]].Index);
            }
        }
    }

    if (selectedIndices.empty()) {
        ShowMessage(L"Nepasirinkta nė vieno failo.");
        return;
    }

    if (!dllExtractByIndices) {
        ShowMessage(L"Trūksta ExtractH4RByIndices funkcijos.");
        return;
    }

    std::wstring outDir = ExtractFilePath(Application->ExeName).w_str() + L"Extracted_Files\\";
    if (!DirectoryExists(outDir.c_str())) {
        CreateDirectoryW(outDir.c_str(), NULL);
    }

    ProgressBar1->Min = 0;
    ProgressBar1->Max = (int)selectedIndices.size();
    ProgressBar1->Position = 0;
    ProgressBar1->Visible = true;
    this->Cursor = crHourGlass;

    std::string ansiArchive = UTF8String(OpenDialog1->FileName.w_str()).c_str();
    std::string ansiOut = UTF8String(outDir.c_str()).c_str();
    int* indices = new int[selectedIndices.size()];
    for (size_t i = 0; i < selectedIndices.size(); i++) indices[i] = selectedIndices[i];

    bool success = dllExtractByIndices(ansiArchive.c_str(), ansiOut.c_str(), indices, (int)selectedIndices.size());
    delete[] indices;

    ProgressBar1->Visible = false;
    this->Cursor = crDefault;

    if (success) {
        ShowMessage(L"Išpakuota " + IntToStr((int)selectedIndices.size()) + L" failų į " + outDir);
    } else {
        ShowMessage(L"Išpakavimo metu įvyko klaidų.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPack::BtnOpenClick(TObject *Sender)
{
    if (OpenDialog1->Execute()) {
        OpenArchive(OpenDialog1->FileName.w_str());
    }
}
//---------------------------------------------------------------------------
// ================== Plugin exports ==================
static TPluginInfo g_PluginInfo = { L"H4Unpack_Pack", L"1.0" };

extern "C" __declspec(dllexport) const TPluginInfo* __stdcall GetPluginInfo(const wchar_t* langCode)
{
    return &g_PluginInfo;
}

extern "C" __declspec(dllexport) void __stdcall ExecutePlugin(HWND parentHWND, const wchar_t* langFilePath)
{
    try {
        Application->Handle = parentHWND;
        TFormPack *f = new TFormPack(Application);
        f->ParentWindow = parentHWND;
        f->SetLanguageFile(langFilePath);
        f->ShowModal();
        delete f;
    } catch (Exception &e) {
        MessageBoxW(parentHWND, e.Message.c_str(), L"H4Unpack_Pack Plugin Error", MB_OK | MB_ICONERROR);
    }
}
//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
    return 1;
}
//---------------------------------------------------------------------------