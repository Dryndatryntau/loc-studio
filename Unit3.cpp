#include <vcl.h>
#pragma hdrstop
#include "Unit3.h"
#include <stdio.h>
#include <vector> // Pridėta čia saugiam vektorių naudojimui
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;

// Globalūs jūsų veikiančios programos kintamieji ir masyvai
sriftai sr[300];
TColor senasp[300][128][128];
bool pakeisted_simbolis[300];
int sr_count = 0;
int senas = -1;
const int point = 10;
FILE *failas;

// PRIDĖTA: Globalus vektorius archyvo failų sąrašui saugoti
std::vector<H4R_Failas> g_archyvas;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
	: TForm(Owner)
{
    sr_count = 0;
    senas = -1;
    for(int i = 0; i < 300; i++) pakeisted_simbolis[i] = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::BtnOpenFontClick(TObject *Sender)
{
	TOpenDialog *OpenDialog1 = new TOpenDialog(Application);
	OpenDialog1->Filter = "Heroes 4 Font (*.fnt)|*.fnt|All files (*.*)|*.*";

	if (OpenDialog1->Execute())
	{
        failas = _wfopen(OpenDialog1->FileName.w_str(), L"rb");
        if (!failas) {
            delete OpenDialog1;
            return;
        }

        if (ListFonts) ListFonts->Items->Clear();
        sr_count = 0;

        fseek(failas, 0, SEEK_END);
        long failo_dydis = ftell(failas);
        fseek(failas, 28, SEEK_SET);

        while (ftell(failas) < failo_dydis - 8 && sr_count < 300)
        {
            sr[sr_count].lmargin = fgetc(failas) | (fgetc(failas) << 8);
            sr[sr_count].rmargin = fgetc(failas) | (fgetc(failas) << 8);

            int x = fgetc(failas) | (fgetc(failas) << 8);
            int y = fgetc(failas) | (fgetc(failas) << 8);

            sr[sr_count].plotis = x;
            sr[sr_count].ilgis = y;

            for (int i = 0; i < y; i++) {
                for (int j = 0; j < x; j++) {
                    unsigned char sk = fgetc(failas);
                    unsigned char pilkumas = 255 - sk;
                    senasp[sr_count][j][i] = (TColor)RGB(pilkumas, pilkumas, pilkumas);
                }
            }

            if (ListFonts) {
                ListFonts->Items->Add("Simbolis " + IntToStr(sr_count) + " [" + IntToStr(x) + "x" + IntToStr(y) + "]");
            }
            sr_count++;
        }
        fclose(failas);
	}
	delete OpenDialog1;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::ListFontsClick(TObject *Sender)
{
	if (ListFonts->ItemIndex != -1) {
        senas = ListFonts->ItemIndex;
        rodyk(senas);
	}
}
//---------------------------------------------------------------------------
void TForm3::rodyk(int sr_sk)
{
    if (sr_sk < 0 || sr_sk >= sr_count) return;

    int x = sr[sr_sk].plotis;
    int y = sr[sr_sk].ilgis;

    Image1->Picture->Bitmap->SetSize(x, y);
    Image1->Canvas->Brush->Color = clWhite;
    Image1->Canvas->FillRect(TRect(0, 0, x, y));

    Image2->Picture->Bitmap->SetSize(x * point, y * point);
    Image2->Canvas->Brush->Color = clNavy;
    Image2->Canvas->FillRect(TRect(0, 0, Image2->Width, Image2->Height));

    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            TColor spalva = senasp[sr_sk][j][i];

            Image1->Canvas->Pixels[j][i] = spalva;

            Image2->Canvas->Brush->Color = spalva;
            Image2->Canvas->FillRect(TRect(j * point, i * point, (j + 1) * point, (i + 1) * point));
        }
    }
    Image1->Refresh();
    Image2->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Image2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (senas == -1 || sr_count <= 0) return;

	int px = X / point;
	int py = Y / point;

	if (px >= 0 && px < sr[senas].plotis && py >= 0 && py < sr[senas].ilgis)
	{
        senasp[senas][px][py] = (Button == mbLeft) ? (TColor)RGB(0,0,0) : (TColor)RGB(255,255,255);
        pakeisted_simbolis[senas] = true;
        rodyk(senas);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Image2MouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
	if (senas == -1 || sr_count <= 0) return;
	if (!Shift.Contains(ssLeft) && !Shift.Contains(ssRight)) return;

	int px = X / point;
	int py = Y / point;

	if (px >= 0 && px < sr[senas].plotis && py >= 0 && py < sr[senas].ilgis)
	{
        senasp[senas][px][py] = Shift.Contains(ssLeft) ? (TColor)RGB(0,0,0) : (TColor)RGB(255,255,255);
        pakeisted_simbolis[senas] = true;
        rodyk(senas);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm3::BtnOpenH4RClick(TObject *Sender)
{
	TOpenDialog *OD = new TOpenDialog(Application);
	OD->Filter = "Heroes 4 Archyvas (*.h4r)|*.h4r";

	if (OD->Execute())
	{
		FILE *arch = _wfopen(OD->FileName.w_str(), L"rb");
		if (!arch) { delete OD; return; }

		g_archyvas.clear();
		fseek(arch, 0, SEEK_END);
		long failo_dydis = ftell(arch);

		fseek(arch, failo_dydis - 12, SEEK_SET);
		long lenteles_startas;
		int failu_kiekis;
		fread(&lenteles_startas, 4, 1, arch);
		fread(&failu_kiekis, 4, 1, arch);

		fseek(arch, lenteles_startas, SEEK_SET);

		StringGrid1->RowCount = failu_kiekis + 1;
		StringGrid1->ColCount = 3;
		StringGrid1->Cells[0][0] = "Failas";
		StringGrid1->Cells[1][0] = "Offsetas";
		StringGrid1->Cells[2][0] = "Dydis (Baitai)";

		for (int i = 0; i < failu_kiekis; i++)
		{
			long f_offset, f_dydis;
			char vardo_buf[32];

			fread(&f_offset, 4, 1, arch);
			fread(&f_dydis, 4, 1, arch);
			fread(vardo_buf, 1, 32, arch);

			H4R_Failas inf;
			inf.vardas = AnsiString(vardo_buf);
			inf.startas = f_offset;
			inf.dydis = f_dydis;
			g_archyvas.push_back(inf);

			StringGrid1->Cells[0][i+1] = inf.vardas;
			StringGrid1->Cells[1][i+1] = IntToStr((int)inf.startas);
			StringGrid1->Cells[2][i+1] = IntToStr((int)inf.dydis);

		}
		fclose(arch);
		ShowMessage("Sėkmingai nuskaityta " + IntToStr(failu_kiekis) + " failų!");
	}
	delete OD;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::BtnUnpackClick(TObject *Sender)
{
	if (g_archyvas.empty()) {
		ShowMessage("Pirmiausia atidarykite .h4r failą!");
		return;
	}

	TOpenDialog *OD = new TOpenDialog(Application);
	OD->Title = "Pasirinkite archyvą išpakavimui";
	OD->Filter = "Heroes 4 Archyvas (*.h4r)|*.h4r";

	if (OD->Execute())
	{
		FILE *arch = _wfopen(OD->FileName.w_str(), L"rb");
		if (!arch) { delete OD; return; }

		AnsiString prog_kelias = ExtractFilePath(Application->ExeName);
		AnsiString base_extracted = prog_kelias + "extracted\\";
		AnsiString base_packed = prog_kelias + "packed\\";

		ForceDirectories(base_packed);

		for (size_t i = 0; i < g_archyvas.size(); i++)
		{
			fseek(arch, g_archyvas[i].startas, SEEK_SET);

			AnsiString sub_aplankas = "other\\";
			AnsiString papildoma_galune = "";
			AnsiString f_vardas = g_archyvas[i].vardas.LowerCase();

			// Pataisyta: Naudojama .Pos() > 0 vietoj .Contains() moderniam suderinamumui
			if (f_vardas.Pos("font") > 0 || f_vardas.Pos(".fnt") > 0) {
				sub_aplankas = "fonts\\";
			}
			else if (f_vardas.Pos("map") > 0 || f_vardas.Pos(".h4m") > 0) {
				sub_aplankas = "game_maps\\";
			}
			else if (f_vardas.Pos("sound") > 0 || f_vardas.Pos("music") > 0 || f_vardas.Pos(".mp3") > 0 || f_vardas.Pos(".wav") > 0) {
				sub_aplankas = "sound\\";
			}
			else if (f_vardas.Pos("string") > 0 || f_vardas.Pos(".str") > 0) {
				sub_aplankas = "strings\\";
				papildoma_galune = ".txt";
			}
			else if (f_vardas.Pos("table") > 0 || f_vardas.Pos(".tbl") > 0) {
				sub_aplankas = "table\\";
				papildoma_galune = ".txt";
			}
			else if (f_vardas.Pos("actor") > 0 || f_vardas.Pos("object") > 0 || f_vardas.Pos("anim") > 0 || f_vardas.Pos("bitmap") > 0) {
				sub_aplankas = "sprites\\";
			}
			else if (f_vardas.Pos("bink") > 0 || f_vardas.Pos(".bik") > 0) {
				sub_aplankas = "video\\";
			}

			AnsiString tikslus_aplankas = base_extracted + sub_aplankas;
			ForceDirectories(tikslus_aplankas);

			AnsiString pilnas_kelias = tikslus_aplankas + g_archyvas[i].vardas + papildoma_galune;

			FILE *f_isvestis = fopen(pilnas_kelias.c_str(), "wb");
			if (f_isvestis)
			{
				std::vector<char> buferis(g_archyvas[i].dydis);
				fread(buferis.data(), 1, g_archyvas[i].dydis, arch);
				fwrite(buferis.data(), 1, g_archyvas[i].dydis, f_isvestis);
				fclose(f_isvestis);
			}
		}
		fclose(arch);
		ShowMessage("Failai sėkmingai išpakuoti! Sukurti tik reikalingi aplankai:\n" + base_extracted);
	}
	delete OD;
}

