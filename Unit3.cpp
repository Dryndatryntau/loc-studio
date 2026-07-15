#include <vcl.h>
#pragma hdrstop
#include "Unit3.h"
#include <fstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
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
        FILE *failas = fopen(OpenDialog1->FileName.c_str(), "rb");
        if (!failas) {
            delete OpenDialog1;
            return;
        }

        if (ListFonts) ListFonts->Items->Clear();
        sr_count = 0;

        fseek(failas, 0, SEEK_END);
        long failo_dydis = ftell(failas);
        fseek(failas, 28, SEEK_SET); // Šuolis per Heroes 4 antraštę

        while (ftell(failas) < failo_dydis - 8 && sr_count < 300)
        {
            sr[sr_count].lmargin = fgetc(failas) | (fgetc(failas) << 8);
            sr[sr_count].rmargin = fgetc(failas) | (fgetc(failas) << 8);
            
            int x = fgetc(failas) | (fgetc(failas) << 8);
            int y = fgetc(failas) | (fgetc(failas) << 8);
            
            sr[sr_count].plotis = x;
            sr[sr_count].ilgis = y;

            // Nuskaitome pikselių spalvas į jūsų originalų masyvą
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

    // 1. Mažasis vaizdas (Tikrasis dydis)
    Image1->Picture->Bitmap->SetSize(x, y);
    Image1->Canvas->Brush->Color = clWhite;
    Image1->Canvas->FillRect(TRect(0, 0, x, y));

    // 2. Didysis vaizdas (Tinklelis)
    Image2->Picture->Bitmap->SetSize(x * point, y * point);
    Image2->Canvas->Brush->Color = clNavy;
    Image2->Canvas->FillRect(TRect(0, 0, Image2->Width, Image2->Height));

    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            TColor spalva = senasp[sr_sk][j][i];
            
            // Piešiame mažajame lauke
            Image1->Canvas->Pixels[j][i] = spalva;

            // Piešiame didžiajame redagavimo lauke
            Image2->Canvas->Brush->Color = spalva;
            Image2->Canvas->FillRect(TRect(j * point, i * point, (j + 1) * point, (i + 1) * point));
        }
    }
    Image1->Repaint();
    Image2->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Image2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (senas == -1 || sr_count <= 0) return;

	int px = X / point;
	int py = Y / point;

	if (px >= 0 && px < sr[senas].plotis && py >= 0 && py < sr[senas].ilgis)
	{
        // Kairysis piešia juodai (raidė), dešinysis baltai (fonas)
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
