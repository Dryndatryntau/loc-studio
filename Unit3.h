#ifndef Unit3H
#define Unit3H

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <vector> // Átraukiame vektoriø

struct sriftai {
    int plotis;
    int ilgis;
    int lmargin;
    int rmargin;
    int nzn;
    int nzn2;
};

// Struktûra archyvo failø parametrams saugoti
struct H4R_Failas {
    AnsiString vardas;
    long startas;
    long dydis;
};

class TForm3 : public TForm
{
__published:	// IDE-managed Components
	TListBox *ListFonts;
	TImage *Image1;
	TImage *Image2;
	TButton *BtnOpenFont;
	TPageControl *PageControl1;
	TTabSheet *TabArch;   // Archyvavimo tab'as
	TTabSheet *TabFonts;  // Ðriftø tab'as
	TTabSheet *TabText;   // Tekstø tab'as
	TPanel *Panel1;       // Mygtukø skydelis
	TStringGrid *StringGrid1; // Lentelë archyvo failams
	TComboBox *ComboGame;
	TComboBox *ComboLang;

	void __fastcall BtnOpenFontClick(TObject *Sender);
	void __fastcall ListFontsClick(TObject *Sender);
	void __fastcall Image2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Image2MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall BtnOpenH4RClick(TObject *Sender); // Pridëtas atidarymas
	void __fastcall BtnUnpackClick(TObject *Sender);  // Pridëtas iðpakavimas
private:
public:
	__fastcall TForm3(TComponent* Owner);
    void rodyk(int sr_sk);
};

extern PACKAGE TForm3 *Form3;

#endif

