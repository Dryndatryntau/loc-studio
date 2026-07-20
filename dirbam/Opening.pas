{ KOL MCK } // Do not remove this line!
{$DEFINE KOL_MCK}
unit Opening;

interface

{$IFDEF KOL_MCK}
uses Windows, Messages, ShellAPI, KOL {$IFNDEF KOL_MCK}, mirror, Classes,
  Controls, mckCtrls {$ENDIF};
{$ELSE}
{$I uses.inc}
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;
{$ENDIF}

type
  {$IFDEF KOL_MCK}
  {$I MCKfakeClasses.inc}
  PForm4 = ^TForm4;
  TForm4 = object(TObj)
    Form: PControl;
  {$ELSE not_KOL_MCK}
  TForm4 = class(TForm)
  {$ENDIF KOL_MCK}
    KOLForm1: TKOLForm;
    RadioBox1: TKOLRadioBox;
    RadioBox2: TKOLRadioBox;
    GroupBox1: TKOLGroupBox;
    Memo1: TKOLMemo;
    GroupBox2: TKOLGroupBox;
    Label1: TKOLLabel;
    Label2: TKOLLabel;
    Label3: TKOLLabel;
    Label4: TKOLLabel;
    Label5: TKOLLabel;
    EditBox1: TKOLEditBox;
    Button1: TKOLButton;
    procedure RadioBox1Click(Sender: PObj);
    procedure RadioBox2Click(Sender: PObj);
    procedure KOLForm1Close(Sender: PObj; var Accept: Boolean);
    procedure Button1Click(Sender: PObj);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form4 {$IFDEF KOL_MCK} : PForm4 {$ELSE} : TForm4 {$ENDIF} ;

{$IFDEF KOL_MCK}
procedure NewForm4( var Result: PForm4; AParent: PControl );
{$ENDIF}

implementation

{$IFNDEF KOL_MCK} {$R *.DFM} {$ENDIF}

{$IFDEF KOL_MCK}
{$I Opening_1.inc}
{$ENDIF}

procedure TForm4.RadioBox1Click(Sender: PObj);
begin
  if Memo1<>nil then
  begin
    Memo1.Text:='При распаковке будут использоваться стандартные'+#13+#10+'значения путей, записанные в архиве. Этот способ не рекомендуется, т.к. папки, предлагаемые по умолчанию не соответствуют структуре ресурсов. Соседние по смыслу изображения придется искать в разных (не очень близких) местах.';
    EditBox1.Text:='C_\Heroes 4\Assets\layers\button\layers.button.cancel.h4r';
  end;
end;

procedure TForm4.RadioBox2Click(Sender: PObj);
begin
  if Memo1<>nil then
  begin
    Memo1.Text:='При распаковке значения пути будут формироваться из имени файла, путем установки разделителей. Рекомендую использовать именно этот вариант.';
    EditBox1.Text:='layers\button\cancel.h4r';
  end;
end;

procedure TForm4.KOLForm1Close(Sender: PObj; var Accept: Boolean);
begin
  Accept:=False;
end;

procedure TForm4.Button1Click(Sender: PObj);
begin
  Form.ModalResult:=1;
end;

end.


