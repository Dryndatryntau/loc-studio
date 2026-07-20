{ KOL MCK } // Do not remove this line!
{$DEFINE KOL_MCK}
unit MainUnit;

interface

{$IFDEF KOL_MCK}
uses Windows, Messages, ShellAPI, KOL {$IFNDEF KOL_MCK}, mirror, Classes,  Controls, mckCtrls, mckObjs {$ENDIF}, sysutils, kolgzipfuncs, cases, spcaboutdialog;
{$ELSE}
{$I uses.inc}
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;
{$ENDIF}

type

  {$IFDEF KOL_MCK}
  {$I MCKfakeClasses.inc}
  PForm1 = ^TForm1;
  TForm1 = object(TObj)
    Form: PControl;
  {$ELSE not_KOL_MCK}
  TForm1 = class(TForm)
  {$ENDIF KOL_MCK}
    KOLProject1: TKOLProject;
    KOLForm1: TKOLForm;
    ListBox1: TKOLListBox;
    MainMenu1: TKOLMainMenu;
    KOLApplet1: TKOLApplet;
    Label1: TKOLLabel;
    OpenDirDialog1: TKOLOpenDirDialog;
    OpenSaveDialog1: TKOLOpenSaveDialog;
    Panel1: TKOLPanel;
    Label4: TKOLLabel;
    Label5: TKOLLabel;
    Label2: TKOLLabel;
    Label3: TKOLLabel;
    procedure KOLForm1FormCreate(Sender: PObj);
    procedure KOLForm1N3Menu(Sender: PMenu; Item: Integer);
    procedure KOLForm1N5Menu(Sender: PMenu; Item: Integer);
    procedure MainMenu1N6Menu(Sender: PMenu; Item: Integer);
    procedure KOLForm1N2Menu(Sender: PMenu; Item: Integer);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

//  procedure SAdd(Str: PChar; var TS: TSList);
//  procedure SClear(var TS: TSList);

var
  Form1 {$IFDEF KOL_MCK} : PForm1 {$ELSE} : TForm1 {$ENDIF} ;
  TotalFiles: LongInt;
  TotalSize: LongInt;
  StartPath: string;
  zipID: array[0..9]of byte=($1F,$8B,$08,$00,$00,$00,$00,$00,$00,$0B);
  h4rID: array[0..7]of char=('H','4','R',#$05,#$08,#$00,#$00,#$00);
  OpenDir, SaveDir: string;

  Entry: record
           Offset: LongInt;
           Size: LongInt;     //Если Нуль, то вместо пути неястное имя
           RealSize: LongInt;
           Junk_1: array[1..4]of char;
           NameLength: Word;
           Name: string;
           PathLength: Word;  //Если Нуль, то пути нет
           Path: string;
           Flags_1: Word;     //Если не Нуль, то файл пустой
           Flags_2: LongInt;
         end;

{$IFDEF KOL_MCK}
procedure NewForm1( var Result: PForm1; AParent: PControl );
{$ENDIF}

implementation

uses waiting;

{$IFNDEF KOL_MCK} {$R *.DFM} {$ENDIF}

{$IFDEF KOL_MCK}
{$I MainUnit_1.inc}
{$ENDIF}

procedure ScanDir(Path: string; cut: Integer);
var
  sr: TSearchRec;
  i: Integer;
  Temp: string;
begin
  if FindFirst(Path+'*.*', faAnyFile, sr) = 0 then
  begin
    if (not (sr.Name='.'))and(not (sr.Name='..'))then
    if (sr.Attr and faDirectory)=faDirectory then ScanDir(Path+sr.Name+'\',cut) else
    begin
      TotalSize:=TotalSize+sr.Size;
      if sr.Size>0 then
      begin
        Form1.ListBox1.Add(LowerCase(Path+sr.Name));
        Inc(TotalFiles);
      end;
    end;
    while FindNext(sr) = 0 do
    begin
      if (not (sr.Name='.'))and(not (sr.Name='..'))then
        if (sr.Attr and faDirectory)=faDirectory then ScanDir(Path+sr.Name+'\',cut) else
        begin
          TotalSize:=TotalSize+sr.Size;
          if sr.Size>0 then
          begin
            Form1.ListBox1.Add(LowerCase(Path+sr.Name));
            Inc(TotalFiles);
          end;
        end;
    end;
    FindClose(sr);
  end;
end;

procedure TForm1.KOLForm1FormCreate(Sender: PObj);
begin
  GetDir(0,OpenDir);
  GetDir(0,SaveDir);
  InitAboutDialog(Form);
end;

procedure TForm1.KOLForm1N3Menu(Sender: PMenu; Item: Integer);
var
  i: Integer;
begin
  OpenDirDialog1.Title:='Путь к исходным файлам...';
  OpenDirDialog1.InitialPath:=OpenDir;
  if not OpenDirDialog1.Execute then Exit;
  OpenDir:=OpenDirDialog1.Path;
  StartPath:=OpenDir+'\';
  //Получение списка файлов
  Form2.Label1.Caption:='Загрузка списка файлов...';
  Form2.Form.Show;
  Form2.Form.Update;
  TotalSize:=0;
  TotalFiles:=0;
  ListBox1.Clear;
  ScanDir(StartPath,Length(StartPath));
  Form2.Form.Hide;
  SetFocus(ListBox1.Handle);
  Label4.Caption:=Int2Ths(TotalFiles);
  Label5.Caption:=Int2Ths(TotalSize);
  MainMenu1.ItemEnabled[2]:=True;
end;

procedure TForm1.KOLForm1N5Menu(Sender: PMenu; Item: Integer);
begin
  Form.Close;
end;

procedure TForm1.MainMenu1N6Menu(Sender: PMenu; Item: Integer);
var
  Fh4r: PStream;
  FSource,FDest: PStream;
  i,z: Integer;
  CBuffer: array[0..255]of char;
  SavePosition: LongInt;
  fRealSize,fSize,fOffset: LongInt;
  fNameLength: Integer;
  NumLength: Byte;
begin
  OpenSaveDialog1.InitialDir:=SaveDir;
  OpenSaveDialog1.Filename:='equi_text';
  if not OpenSaveDialog1.Execute then Exit;
  SaveDir:=ExtractFilePath(OpenSaveDialog1.Filename);
  Fh4r:=NewReadWriteFileStream(OpenSaveDialog1.FileName);
  //Запись заголовка
  Fh4r.Write(h4rID,8);
  //Запись количества файлов
  Fh4r.Write(TotalFiles,4);
  //Запись записей о файлах
  Form2.Label1.Caption:='Запись таблицы файлов...';
  Form2.Form.Show;
  Form2.Form.Update;
  if TotalFiles>=10000 then NumLength:=5 else
  if TotalFiles>=1000 then NumLength:=4 else
  if TotalFiles>=100 then NumLength:=3 else
  if TotalFiles>=10 then NumLength:=2 else NumLength:=1;
  for i:=1 to TotalFiles do
  begin
    Entry.Offset:=$00;
    Entry.Size:=$00;
    Entry.RealSize:=$00;
    Entry.NameLength:=Length(ListBox1.Items[i-1])-Length(StartPath);
    Entry.Junk_1:=#$00+#$00+#$00+#$00;
    Entry.PathLength:=$00;
    Entry.Flags_1:=$0000;
    Entry.Flags_2:=$00000003;
    Fh4r.Write(Entry.Offset,4);
    Fh4r.Write(Entry.Size,4);
    Fh4r.Write(Entry.RealSize,4);
    Fh4r.Write(Entry.Junk_1,4);
    Fh4r.Write(Entry.NameLength,2);
    for z:=1 to Entry.NameLength do CBuffer[z-1]:=ListBox1.Items[i-1][Length(StartPath)+z];
    for z:=1 to Entry.NameLength do if CBuffer[z-1]='\' then CBuffer[z-1]:='.';
    Fh4r.Write(CBuffer,Entry.NameLength);
    CBuffer[0]:=#$00;
    CBuffer[1]:=#$00;
    Fh4r.Write(CBuffer,2);
    Fh4r.Write(Entry.Flags_1,2);
    Fh4r.Write(Entry.Flags_2,4);
  end;
  Fh4r.Seek($0C,spBegin);
  for i:=1 to TotalFiles do
  begin
    SavePosition:=Fh4r.Position;
    Fh4r.Seek(Fh4r.Size,spBegin);
    fOffset:=Fh4r.Position;
    FSource:=NewReadFileStream(ListBox1.Items[i-1]);
    FDest:=NewMemoryStream;
    fRealSize:=fSource.Size;
    CompressStreamToStream(FSource,FSource.Size,FDest);
    fSize:=fDest.Size+8+4;
    FDest.Seek($02,spBegin);
    Fh4r.Write(zipID,10);
//    ShowMessage(Int2Str(Fh4r.Size));
    Stream2Stream(Fh4r,FDest,FDest.Size-2);
    Fh4r.Write(fRealSize,4);
    FDest.Free;
    FSource.Free;
    Fh4r.Seek(SavePosition,spBegin);
    Fh4r.Write(fOffset,4);
    Fh4r.Write(fSize,4);
    Fh4r.Write(fRealSize,4);
    Fh4r.Seek($04,spCurrent);
    Fh4r.Read(fNameLength,2);
    Fh4r.Seek(fNameLength,spCurrent);
    Fh4r.Seek($08,spCurrent);
    Form2.Label1.Caption:='Файлов запаковано:['+BuildStr(Int2Str(i),' ',NumLength)+'/'+BuildStr(Int2Str(TotalFiles),' ',NumLength)+']';
    Form2.Form.Update;
  end;
  Form2.Form.Hide;
  SetFocus(ListBox1.Handle);
  Fh4r.Free;
end;

procedure TForm1.KOLForm1N2Menu(Sender: PMenu; Item: Integer);
begin
  ShowAboutDialog('h4r packer v1.0','21 июня 2002г.','Михаил Бесчетнов','collapse_forever@mail.ru','http://extractor.far.ru');
end;

end.
