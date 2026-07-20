{ KOL MCK } // Do not remove this line!
{$DEFINE KOL_MCK}
unit Selection;

interface

{$IFDEF KOL_MCK}
uses Windows, Messages, ShellAPI, KOL {$IFNDEF KOL_MCK}, mirror, Classes,
  Controls, mckCtrls, mckObjs {$ENDIF};
{$ELSE}
{$I uses.inc}
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;
{$ENDIF}

type
  {$IFDEF KOL_MCK}
  {$I MCKfakeClasses.inc}
  PForm3 = ^TForm3;
  TForm3 = object(TObj)
    Form: PControl;
  {$ELSE not_KOL_MCK}
  TForm3 = class(TForm)
  {$ENDIF KOL_MCK}
    KOLForm1: TKOLForm;
    Label1: TKOLLabel;
    EditBox1: TKOLEditBox;
    CheckBox1: TKOLCheckBox;
    CheckBox2: TKOLCheckBox;
    Button1: TKOLButton;
    Button2: TKOLButton;
    Timer1: TKOLTimer;
    CheckBox3: TKOLCheckBox;
    procedure Timer1Timer(Sender: PObj);
    procedure KOLForm1Close(Sender: PObj; var Accept: Boolean);
    procedure Button1Click(Sender: PObj);
    procedure Button2Click(Sender: PObj);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form3 {$IFDEF KOL_MCK} : PForm3 {$ELSE} : TForm3 {$ENDIF} ;

{$IFDEF KOL_MCK}
procedure NewForm3( var Result: PForm3; AParent: PControl );
{$ENDIF}

implementation

{$IFNDEF KOL_MCK} {$R *.DFM} {$ENDIF}

{$IFDEF KOL_MCK}
{$I Selection_1.inc}
{$ENDIF}

procedure TForm3.Timer1Timer(Sender: PObj);
begin
  if Length(EditBox1.Text)=0 then Button1.Enabled:=False else Button1.Enabled:=True;
end;

procedure TForm3.KOLForm1Close(Sender: PObj; var Accept: Boolean);
begin
  Accept:=False;
end;

procedure TForm3.Button1Click(Sender: PObj);
begin
  Form.ModalResult:=1;
end;

procedure TForm3.Button2Click(Sender: PObj);
begin
  Form.ModalResult:=2;
end;

end.


