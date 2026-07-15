#ifndef Unit3H
#define Unit3H

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Dialogs.hpp>

struct H4R_Failas {
  String pavadinimas;
  long offsetas;
  long dydis;
};

class TForm3 : public TForm
{
__published:
	TPageControl *PageControl1;
	TTabSheet *TabFonts;
	TTabSheet *TabText;
	TTabSheet *TabArch;
	TPanel *Panel1;
	TComboBox *ComboGame;
	TComboBox *ComboLang;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;

	// 🎯 SUTAISYTA ČIA: Grąžintas gamyklinis pavadinimas, kad dizaineris nepanikuotų!
	TListBox *ListFonts;
	TStringGrid *StringGrid1;
	TImage *Image1;

	TButton *BtnOpenFont;
	TButton *BtnSaveFont;
	TImage *Image2;

	// Įvykių registracijos
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ComboLangChange(TObject *Sender);
	void __fastcall ComboGameChange(TObject *Sender);
	void __fastcall BtnOpenFontClick(TObject *Sender);
	void __fastcall BtnSaveFontClick(TObject *Sender);
	void __fastcall ListFontsClick(TObject *Sender);
	void __fastcall Image2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    	void __fastcall Image2MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);

private:
public:
	__fastcall TForm3(TComponent* Owner);
	void rodyk(int sr_sk);
	void __fastcall UzkrautiKalba(String kalbos_failas);


};

extern PACKAGE TForm3 *Form3;

#endif

