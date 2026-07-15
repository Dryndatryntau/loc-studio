#ifndef Unit3H
#define Unit3H

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>

struct sriftai {
    int plotis;
    int ilgis;
    int lmargin;
    int rmargin;
    int nzn;
    int nzn2;
};

class TForm3 : public TForm
{
__published:
	TListBox *ListFonts;
	TImage *Image1;
	TImage *Image2;
	TButton *BtnOpenFont;
	void __fastcall BtnOpenFontClick(TObject *Sender);
	void __fastcall ListFontsClick(TObject *Sender);
	void __fastcall Image2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Image2MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
private:
    // Originalūs jūsų kintamieji iš Unit1
    sriftai sr[300];
    TColor senasp[300][100][100];
    bool pakeisted_simbolis[300];
    int sr_count;
    int senas;
    static const int point = 10; // Padidinom tinklelio mastelį iki 10 redagavimui
public:
	__fastcall TForm3(TComponent* Owner);
    void rodyk(int sr_sk); // Jūsų originali rodymo funkcija
};

extern PACKAGE TForm3 *Form3;

#endif
