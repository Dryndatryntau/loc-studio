{ KOL MCK } // Do not remove this line!
program h4rpacker;

uses
KOL,
  MainUnit in 'MainUnit.pas' {Form1},
  waiting in 'waiting.pas' {Form2};

//{$R *.res}

begin // PROGRAM START HERE -- Please do not remove this comment

{$IFDEF KOL_MCK} {$I h4rpacker_0.inc} {$ELSE}

  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.CreateForm(TForm2, Form2);
  Application.Run;

{$ENDIF}

end.

