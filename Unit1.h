//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TOpenDialog *OpenDialog1;
	TListBox *ListBox1;
	TMemo *Memo1;
	TEdit *Edit1;
	TStatusBar *StatusBar1;
	TImage *Image1;
	TImage *Image2;
	TButton *Button2;
	TFontDialog *FontDialog1;
	TButton *Button3;
	TEdit *Edit2;
	TLabel *Label1;
	TLabel *Label2;
	TButton *Button4;
	TLabel *LabelFailas;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Image1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y);
	void __fastcall Edit1Change(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);







private:	// User declarations
public:		// User declarations
    void __fastcall NuskaitytiFaila(TObject *Sender);
    void __fastcall rodyk(int sr_sk);
	void __fastcall Image2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

	__fastcall TForm1(TComponent* Owner);

};;
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
