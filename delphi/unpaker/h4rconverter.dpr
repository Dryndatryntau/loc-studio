{ KOL MCK } // Do not remove this line!
program h4rconverter;

uses
KOL,
  MainUnit in 'MainUnit.pas' {Form1},
  Waiting in 'Waiting.pas' {Form2},
  Selection in 'Selection.pas' {Form3},
  Opening in 'Opening.pas' {Form4};

//{$R *.res}

begin // PROGRAM START HERE -- Please do not remove this comment

{$IFDEF KOL_MCK} {$I h4rconverter_0.inc} {$ELSE}

  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TForm2, Form2);
  Application.CreateForm(TForm3, Form3);
  Application.CreateForm(TForm4, Form4);
  Application.Run;

{$ENDIF}

end.

