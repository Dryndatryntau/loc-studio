#ifndef Unit3H
#define Unit3H

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <vector>

struct GlifoInfo {
    long failo_offsetas;
    int plotis;
    int aukstis;
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
	std::vector<char> g_srifto_buf;
	long g_srifto_dydis;
	std::vector<GlifoInfo> g_glifu_indeksas;
public:
	__fastcall TForm3(TComponent* Owner);
};

extern PACKAGE TForm3 *Form3;

#endif
