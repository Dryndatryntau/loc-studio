//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "UnitText.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <string>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TFormText *FormText;
//---------------------------------------------------------------------------
__fastcall TFormText::TFormText(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormText::SetLanguageFile(const std::wstring& langFilePath)
{
    LangFilePath = langFilePath;
    LoadLanguage(langFilePath);
}
//---------------------------------------------------------------------------
void __fastcall TFormText::LoadLanguage(const std::wstring& langFilePath)
{
    if (!FileExists(langFilePath.c_str())) return;

    TMemIniFile *ini = new TMemIniFile(langFilePath.c_str(), TEncoding::UTF8);
    try {
        Caption = ini->ReadString(L"textinout", L"FormCaption", L"Text Input/Output Tool").w_str();
        Button1->Caption = ini->ReadString(L"textinout", L"Button1_Caption", L"Export").w_str();
        Button2->Caption = ini->ReadString(L"textinout", L"Button2_Caption", L"Import").w_str();
    } __finally {
        delete ini;
    }
}
//---------------------------------------------------------------------------
// MYGTUKAS 1: Eksportuoti tekstą
void __fastcall TFormText::Button1Click(TObject *Sender)
{
    if (!OpenDialog1->Execute()) return;

    SaveDialog1->FileName = L"isvestas_tekstas.txt";
    if (!SaveDialog1->Execute()) return;

    int f_in = _wopen(OpenDialog1->FileName.c_str(), _O_RDONLY | _O_BINARY);
    if (f_in == -1) {
        ShowMessage(L"Klaida atidarant žaidimo binarinį failą!");
        return;
    }

    int f_out = _wopen(SaveDialog1->FileName.c_str(),
        _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
    if (f_out == -1) {
        _close(f_in);
        ShowMessage(L"Klaida sukuriant išvesties tekstinį failą!");
        return;
    }

    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    _write(f_out, bom, 3);

    int bendras_eiluciu_kiekis = 0;
    _read(f_in, &bendras_eiluciu_kiekis, 4);

    int eksportuota_zodziu = 0;

    for (int r = 0; r < bendras_eiluciu_kiekis; r++) {
        unsigned short int stulpeliu_kiekis = 0;
        if (_read(f_in, &stulpeliu_kiekis, 2) <= 0) break;

        for (int c = 0; c < stulpeliu_kiekis; c++) {
            unsigned short int zodzio_ilgis = 0;
            _read(f_in, &zodzio_ilgis, 2);

            std::vector<char> zodzio_buf(zodzio_ilgis + 1, 0);
            if (zodzio_ilgis > 0) {
                _read(f_in, zodzio_buf.data(), zodzio_ilgis);
            }

            int w_len = MultiByteToWideChar(1257, 0, zodzio_buf.data(), -1, NULL, 0);
            std::vector<wchar_t> w_zodis(w_len);
            MultiByteToWideChar(1257, 0, zodzio_buf.data(), -1, w_zodis.data(), w_len);
            std::wstring tikrasis_tekstas = w_zodis.data();

            if (tikrasis_tekstas.empty() || tikrasis_tekstas.find_first_not_of(L" \t\r\n") == std::wstring::npos)
                continue;

            if (c == 0 && tikrasis_tekstas.length() > 0) {
                wchar_t pirmas_char = tikrasis_tekstas[0];
                if (pirmas_char >= L'a' && pirmas_char <= L'z') {
                    continue;
                }
            }

            std::wstring isvestis = L"L_" + std::to_wstring(r) + L"_" + std::to_wstring(c) + L"=" + tikrasis_tekstas + L"\r\n";
            std::string utf8_isvestis = UTF8String(isvestis.c_str()).c_str();
            _write(f_out, utf8_isvestis.c_str(), utf8_isvestis.length());
            eksportuota_zodziu++;
        }
    }

    _close(f_in);
    _close(f_out);

    ShowMessage(String().sprintf(L"Sėkmė! Eksportuota %d tik vertimui skirtų eilučių.", eksportuota_zodziu));
}
//---------------------------------------------------------------------------
// MYGTUKAS 2: Importuoti tekstą
void __fastcall TFormText::Button2Click(TObject *Sender)
{
    OpenDialog1->Title = L"Pasirinkite išverstą TXT failą";
    if (!OpenDialog1->Execute()) return;
    std::wstring txt_failas = OpenDialog1->FileName.w_str();

    OpenDialog1->Title = L"Pasirinkite ORIGINALŲ žaidimo failą (.bak)";
    if (!OpenDialog1->Execute()) return;
    std::wstring orig_failas = OpenDialog1->FileName.w_str();

    std::wstring naujas_failas = orig_failas + L".new";
    int f_out = _wopen(naujas_failas.c_str(),
        _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
    if (f_out == -1) {
        ShowMessage(L"Klaida sukuriant naują binarinį failą!");
        return;
    }

    int f_orig = _wopen(orig_failas.c_str(), _O_RDONLY | _O_BINARY);
    if (f_orig == -1) {
        _close(f_out);
        ShowMessage(L"Klaida atidarant originalą!");
        return;
    }
    long orig_dydis = _lseek(f_orig, 0, SEEK_END);
    _lseek(f_orig, 0, SEEK_SET);

    std::vector<char> orig_buf(orig_dydis);
    _read(f_orig, orig_buf.data(), orig_dydis);
    _close(f_orig);

    int f_txt = _wopen(txt_failas.c_str(), _O_RDONLY | _O_BINARY);
    _lseek(f_txt, 0, SEEK_SET);

    char bom, bom2, bom3;
    _read(f_txt, &bom, 1); _read(f_txt, &bom2, 1); _read(f_txt, &bom3, 1);
    if (!((unsigned char)bom == 0xEF && (unsigned char)bom2 == 0xBB && (unsigned char)bom3 == 0xBF)) {
        _lseek(f_txt, 0, SEEK_SET);
    }

    std::vector<UnicodeString> txt_eilutes;
    std::vector<char> eil_buf;
    char b;
    while (_read(f_txt, &b, 1) > 0) {
        if (b != '\n' && b != '\r') {
            eil_buf.push_back(b);
            continue;
        }
        if (b == '\r') continue;
        eil_buf.push_back(0);

        int w_len = MultiByteToWideChar(CP_UTF8, 0, eil_buf.data(), -1, NULL, 0);
        std::vector<wchar_t> w_buf(w_len);
        MultiByteToWideChar(CP_UTF8, 0, eil_buf.data(), -1, w_buf.data(), w_len);

        txt_eilutes.push_back(UnicodeString(w_buf.data()));
        eil_buf.clear();
    }
    _close(f_txt);

    long dabartinis_pos_orig = 0;
    _write(f_out, &orig_buf[dabartinis_pos_orig], 4);

    int bendras_eiluciu_kiekis = *(int*)&orig_buf[dabartinis_pos_orig];
    dabartinis_pos_orig += 4;

    int pakeista = 0;

    for (int r = 0; r < bendras_eiluciu_kiekis; r++) {
        if (dabartinis_pos_orig >= orig_dydis) break;

        _write(f_out, &orig_buf[dabartinis_pos_orig], 2);
        unsigned short int stulpeliu_kiekis = *(unsigned short int*)&orig_buf[dabartinis_pos_orig];
        dabartinis_pos_orig += 2;

        for (int c = 0; c < stulpeliu_kiekis; c++) {
            unsigned short int senas_ilgis = *(unsigned short int*)&orig_buf[dabartinis_pos_orig];
            dabartinis_pos_orig += 2;

            UnicodeString ieskomas_raktas = L"L_" + IntToStr(r) + L"_" + IntToStr(c) + L"=";
            UnicodeString grynasis_vertimas = L"";
            bool rasta_txt = false;

            for (size_t k = 0; k < txt_eilutes.size(); k++) {
                if (txt_eilutes[k].Pos(ieskomas_raktas) == 1) {
                    grynasis_vertimas = txt_eilutes[k].SubString(ieskomas_raktas.Length() + 1,
                        txt_eilutes[k].Length() - ieskomas_raktas.Length());
                    rasta_txt = true;
                    break;
                }
            }

            if (!rasta_txt) {
                _write(f_out, &senas_ilgis, 2);
                if (senas_ilgis > 0) {
                    _write(f_out, &orig_buf[dabartinis_pos_orig], senas_ilgis);
                    dabartinis_pos_orig += senas_ilgis;
                }
                continue;
            }

            int b_len = WideCharToMultiByte(1257, 0, grynasis_vertimas.c_str(), -1, NULL, 0, NULL, NULL);
            std::vector<char> gamyklinis_baltic_buf(b_len);
            WideCharToMultiByte(1257, 0, grynasis_vertimas.c_str(), -1, gamyklinis_baltic_buf.data(), b_len, NULL, NULL);

            unsigned short int naujas_ilgis = (b_len > 0) ? (b_len - 1) : 0;

            _write(f_out, &naujas_ilgis, 2);
            if (naujas_ilgis > 0) {
                _write(f_out, gamyklinis_baltic_buf.data(), naujas_ilgis);
            }

            dabartinis_pos_orig += senas_ilgis;
            pakeista++;
        }
    }

    long likutis = orig_dydis - dabartinis_pos_orig;
    if (likutis > 0 && dabartinis_pos_orig < orig_dydis) {
        _write(f_out, &orig_buf[dabartinis_pos_orig], likutis);
    }

    _close(f_out);
    ShowMessage(String().sprintf(L"Sėkmė! Universalus lentelių importas baigtas. Sėkmingai suleisti %d žodžiai.", pakeista));
}
//---------------------------------------------------------------------------
// ================== Plugin exports ==================
static TPluginInfo g_PluginInfo = { L"TextInOut", L"1.0" };

extern "C" __declspec(dllexport) const TPluginInfo* __stdcall GetPluginInfo(const wchar_t* langCode)
{
    // Galima pagal langCode pakeisti PluginName, bet čia paliekame bazinį
    return &g_PluginInfo;
}

extern "C" __declspec(dllexport) void __stdcall ExecutePlugin(HWND parentHWND, const wchar_t* langFilePath)
{
    try {
        Application->Handle = parentHWND;
        TFormText *f = new TFormText(Application);
        f->ParentWindow = parentHWND;
        f->SetLanguageFile(langFilePath);
        f->ShowModal();
        delete f;
    } catch (Exception &e) {
        MessageBoxW(parentHWND, e.Message.c_str(), L"TextInOut Plugin Error", MB_OK | MB_ICONERROR);
    }
}
//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
    return 1;
}
//---------------------------------------------------------------------------