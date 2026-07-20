{ KOL MCK } // Do not remove this line!
{$DEFINE KOL_MCK}
unit MainUnit;

interface

{$IFDEF KOL_MCK}
uses Windows, Messages, ShellAPI, KOL {$IFNDEF KOL_MCK}, mirror, Classes,
  mckCtrls, Controls, mckObjs {$ENDIF}, kolgzipfuncs, spcAboutDialog;
{$ELSE}
{$I uses.inc}
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  mirror;
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
    ListView1: TKOLListView;
    MainMenu1: TKOLMainMenu;
    OpenSaveDialog1: TKOLOpenSaveDialog;
    KOLApplet1: TKOLApplet;
    Panel1: TKOLPanel;
    GroupBox1: TKOLGroupBox;
    Label1: TKOLLabel;
    Label2: TKOLLabel;
    Label3: TKOLLabel;
    Label4: TKOLLabel;
    Label5: TKOLLabel;
    Label6: TKOLLabel;
    Label7: TKOLLabel;
    Label8: TKOLLabel;
    Label9: TKOLLabel;
    EditBox1: TKOLEditBox;
    GroupBox2: TKOLGroupBox;
    Button1: TKOLButton;
    Button2: TKOLButton;
    Label10: TKOLLabel;
    Label11: TKOLLabel;
    Button3: TKOLButton;
    CheckBox1: TKOLCheckBox;
    Button4: TKOLButton;
    Button5: TKOLButton;
    OpenDirDialog1: TKOLOpenDirDialog;
    Button6: TKOLButton;
    procedure KOLForm1FormCreate(Sender: PObj);
    procedure KOLForm1N5Menu(Sender: PMenu; Item: Integer);
    procedure KOLForm1N3Menu(Sender: PMenu; Item: Integer);
    procedure Button1Click(Sender: PObj);
    procedure Button2Click(Sender: PObj);
    procedure Button3Click(Sender: PObj);
    procedure Button4Click(Sender: PObj);
    function GetOutDir: string;
    procedure Button6Click(Sender: PObj);
    procedure KOLForm1Show(Sender: PObj);
    procedure KOLForm1N2Menu(Sender: PMenu; Item: Integer);
    procedure ListView1Click(Sender: PObj);
    procedure Button5Click(Sender: PObj);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  procedure Extract(Index: Integer);
  function ExcludeTrailingBackslash(const S: string): string;
  function MakeTree(Dir: string): Boolean;
  procedure DefineSelected;
  function GetCharLast(s: string; c: Char): Integer;

var
  Form1 {$IFDEF KOL_MCK} : PForm1 {$ELSE} : TForm1 {$ENDIF} ;
  FDat: PStream;
  OutDir: string;
  ZLibHeader: Word=$9C78;
  TotalFiles: LongInt;
  TotalCompressed: LongInt;
  TotalEmpty: LongInt;
  TotalSize: LongInt;
  CurDir: string;
  Entries: array of record
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

uses Waiting, Selection, Opening;

{$IFNDEF KOL_MCK} {$R *.DFM} {$ENDIF}

{$IFDEF KOL_MCK}
{$I MainUnit_1.inc}
{$ENDIF}

{Возвращает последнюю позицию строки, по которой встречается указанный символ}
function GetCharLast(s: string; c: Char): Integer;
var
  i,t: Integer;
begin
  t:=0;
  if Length(s)=0 then t:=0 else
  for i:=1 to Length(s) do
  if s[i]=c then t:=i;
  GetCharLast:=t;
end;

function ExcludeTrailingBackslash(const S: string): string;
begin
  if S[Length(S)]='\' then Result:=Copy(S,1,Length(S)-1) else Result:=S;
end;

{Построение дерева каталогов}
function MakeTree(Dir: string): Boolean;
begin
  Result := True;
  if Length(Dir) = 0 then Exit;
  Dir := ExcludeTrailingBackslash(Dir);
  if (Length(Dir) < 3) or DirectoryExists(Dir) or (ExtractFilePath(Dir) = Dir) then Exit; // avoid 'xyz:\' problem.
  Result := MakeTree(ExtractFilePath(Dir)) and CreateDir(Dir);
end;

procedure TForm1.KOLForm1FormCreate(Sender: PObj);
begin
  ListView1.LVColAdd('Имя файла',taLeft,160);
  ListView1.LVColAdd('Путь',taLeft,160);
  ListView1.LVColAdd('Размер',taLeft,70);
  ListView1.LVColAdd('Сжатый',taLeft,54);
  GetDir(0,CurDir);
  GetDir(0,OutDir);
end;

procedure TForm1.KOLForm1N5Menu(Sender: PMenu; Item: Integer);
begin
  Form.Close;
end;

procedure TForm1.KOLForm1N3Menu(Sender: PMenu; Item: Integer);
var
  c: char;
  i,j: Integer;
  z: Integer;
  Buffer: array[1..246]of char;
  DefaultPath: Boolean;
begin
  OpenSaveDialog1.InitialDir:=CurDir;
  if OpenSaveDialog1.Execute then
  begin
    CurDir:=OpenSaveDialog1.InitialDir;
    //Спрашиваем о типе определения путей
    Form4.Form.ShowModal;
    Form4.Form.Hide;
    if Form4.RadioBox1.Checked then DefaultPath:=True
      else DefaultPath:=False;
    //Освобождение списка
    Form2.Label1.Caption:='Очищаем список...';
    Form2.Form.Show;
    Form2.Form.Update;
    ListView1.Clear;
    //Загружаем базу
    Form2.Label1.Caption:='Загружаем архив...';
    Form2.Form.Update;
    FDat:=NewReadFileStream(OpenSaveDialog1.FileName);
    FDat.Seek($08,spBegin);
    FDat.Read(TotalFiles,4);
    SetLength(Entries,TotalFiles);
    for i:=1 to TotalFiles do
    begin
      FDat.Read(Entries[i-1].Offset,4);
      FDat.Read(Entries[i-1].Size,4);
      FDat.Read(Entries[i-1].RealSize,4);
      FDat.Read(Entries[i-1].Junk_1,4);
      FDat.Read(Entries[i-1].NameLength,2);
      Entries[i-1].Name:='';
      FDat.Read(Buffer,Entries[i-1].NameLength);
      Entries[i-1].Name:=Copy(Buffer,1,Entries[i-1].NameLength);
      if Entries[i-1].Size=0 then FDat.Seek(2,spCurrent);
      FDat.Read(Entries[i-1].PathLength,2);
      Entries[i-1].Path:='';
      FDat.Read(Buffer,Entries[i-1].PathLength);
      Entries[i-1].Path:=Copy(Buffer,1,Entries[i-1].PathLength);
      if Entries[i-1].Size=0 then
      begin
        Entries[i-1].PathLength:=0;
        Entries[i-1].Path:='';
      end;
      FDat.Read(Entries[i-1].Flags_1,2);
      if Entries[i-1].Flags_1=0 then FDat.Read(Entries[i-1].Flags_2,4) else
      begin
        Entries[i-1].Flags_2:=$00;
        FDat.Seek(2,spCurrent);
      end;
    end;
    FDat.Free;
    //Формируем пути
    if not DefaultPath then
    begin
      Form2.Label1.Caption:='Переформировываем пути...';
      Form2.Form.Update;
      for i:=1 to TotalFiles do
      begin
        for z:=1 to GetCharLast(Entries[i-1].Name,'.')-1 do if Entries[i-1].Name[z]='.' then Entries[i-1].Name[z]:='\';
        Entries[i-1].Path:=ExtractFilePath(Entries[i-1].Name);
        Entries[i-1].Name:=ExtractFileName(Entries[i-1].Name);
        if Entries[i-1].Path=Entries[i-1].Name then Entries[i-1].Path:='';
        Entries[i-1].NameLength:=Length(Entries[i-1].Name);
        Entries[i-1].PathLength:=Length(Entries[i-1].Path);
      end;
    end;
    //Скрываем форму ожидания
    Form2.Label1.Caption:='Формируем список...';
    Form2.Form.Update;
    TotalCompressed:=0;
    TotalEmpty:=0;
    TotalSize:=0;
    for i:=1 to TotalFiles do
    begin
      ListView1.LVItemAdd('');
      ListView1.lvItems[i-1,0]:=Entries[i-1].Name;
      ListView1.lvItems[i-1,1]:=Entries[i-1].Path;
      ListView1.lvItems[i-1,2]:=Int2Str(Entries[i-1].RealSize);
      case Entries[i-1].Flags_2 of
      3:ListView1.lvItems[i-1,3]:='Да';
      1:ListView1.lvItems[i-1,3]:='Нет';
      0:ListView1.lvItems[i-1,3]:='N/A';
      else ListView1.lvItems[i-1,3]:='ERROR';
      end;
      if Entries[i-1].Flags_2=3 then Inc(TotalCompressed);
      if Entries[i-1].Size=0 then Inc(TotalEmpty);
      TotalSize:=TotalSize+Entries[i-1].RealSize;
    end;
    EditBox1.Text:=OpenSaveDialog1.FileName;
    Label4.Caption:=Int2Ths(TotalFiles);
    Label5.Caption:=Int2Ths(TotalCompressed);
    Label8.Caption:=Int2Ths(TotalEmpty);
    Label6.Caption:=Int2Ths(TotalSize);
    Button1.Enabled:=True;
    Button2.Enabled:=True;
    Button3.Enabled:=True;
    Button4.Enabled:=True;
    Button5.Enabled:=True;
    Button6.Enabled:=True;
    Form2.Form.Hide;
  end;
end;

procedure TForm1.Button1Click(Sender: PObj);
begin
  Form2.Label1.Caption:='Выделяем все...';
  Form2.Form.Show;
  Form2.Form.Update;
  ListView1.LVSelectAll;
  Form2.Form.Hide;
  DefineSelected;
end;

procedure TForm1.Button2Click(Sender: PObj);
var
  i: Integer;
begin
  Form2.Label1.Caption:='Выделяем только сжатые...';
  Form2.Form.Show;
  Form2.Form.Update;
  for i:=1 to ListView1.LVCount do
  if ListView1.lvItems[i-1,3]='Да' then ListView1.lvItemState[i-1]:=[lvisselect];
  Form2.Form.Hide;
  DefineSelected;
end;

procedure TForm1.Button3Click(Sender: PObj);
var
  i: Integer;
begin
  Form2.Label1.Caption:='Инвертируем выделение...';
  Form2.Form.Show;
  Form2.Form.Update;
  for i:=1 to ListView1.LVCount do
  if lvisSelect in ListView1.LVItemState[i-1] then ListView1.LVItemState[i-1]:=[] else ListView1.LVItemState[i-1]:=[lvisSelect];
  Form2.Form.Hide;
  DefineSelected;
end;

procedure TForm1.Button4Click(Sender: PObj);
var
  T: string;
  i: Integer;
  fT,z: Integer;
  Allow: Boolean;
begin
  T:=GetOutDir;
  if T='' then Exit;
  OutDir:=T;
  //Распаковываем список
  fT:=0;
  for i:=1 to ListView1.LVCount do if lvisSelect in ListView1.LVItemState[i-1] then Inc(fT);
  Form2.Form.Show;
  fDat:=NewReadFileStream(OpenSaveDialog1.FileName);
  i:=-1;
  z:=0;
  repeat
    i:=ListView1.LVNextSelected(i);
    if i>=0 then
    begin
      Inc(z);
      Form2.Label1.Caption:='Распаковка ... '+Int2Str(Round(100*(z/fT)))+'%';
      Form2.Form.Update;
      if lvisSelect in ListView1.LVItemState[i] then
      begin
        if Entries[i].Size>0 then Allow:=True else
        if not CheckBox1.Checked then Allow:=True else Allow:=False;
      end else Allow:=False;
      if Allow then Extract(i);
    end;
  until i=-1;
  fDat.Free;
  Form2.Form.Hide;
end;

function TForm1.GetOutDir: string;
begin
  OpenDirDialog1.InitialPath:=OutDir;
  if OpenDirDialog1.Execute then
  begin
    Result:=OpenDirDialog1.Path;
  end else Result:='';
end;

procedure Extract(Index: Integer);
var
  Path: string;
  fOut: PStream;
  fT: PStream;
  Allow: Boolean;
begin
  Path:=OutDir;
  if Entries[Index].PathLength>0 then
  begin
    while Entries[Index].Path[1]='\' do Delete(Entries[Index].Path,1,1);
    if Entries[Index].Path[2]=':' then Delete(Entries[Index].Path,2,1);
    Path:=Path+'\'+Entries[Index].Path;
  end else Path:=Path+'\';
  if not (Path[Length(Path)]='\')then Path:=Path+'\';
  MakeTree(Path);
  if Entries[Index].Size>0 then Allow:=True else
  if Form1.CheckBox1.Checked then Allow:=False else Allow:=True;
  if Allow then
  begin
    fOut:=NewWriteFileStream(Path+Entries[Index].Name);
    fDat.Seek(Entries[Index].Offset,spBegin);
    if Entries[Index].Flags_2=1 then Stream2Stream(fOut,fDat,Entries[Index].Size) else
    if Entries[Index].Flags_2=3 then
    begin
      fT:=NewMemoryStream;
      fT.Write(ZLibHeader,2);
      fDat.Seek($0A,spCurrent);
      Stream2Stream(fT,fDat,Entries[Index].Size-10-4);
      fT.Seek(0,spBegin);
      DecompressStreamToStream(fT,fT.Size,Entries[Index].RealSize,fOut);
      fT.Free;
    end;
    fOut.Free;
  end;
end;

procedure TForm1.Button6Click(Sender: PObj);
var
  Res,i: Integer;
  Flag_Name,Flag_Path,Flag_Shift: Boolean;
  SaveFilter,Filter,F1,F2: string;
begin
  SaveFilter:=Form3.EditBox1.Text;
  Form3.Form.ShowModal;
  Res:=Form3.Form.ModalResult;
  Form3.Form.Hide;
  if Res=2 then
  begin
    Form3.EditBox1.Text:=SaveFilter;
    Exit;
  end;
  Filter:=Form3.EditBox1.Text;
  if Form3.CheckBox1.Checked then Flag_Name:=True else Flag_Name:=False;
  if Form3.CheckBox2.Checked then Flag_Path:=True else Flag_Path:=False;
  if Form3.CheckBox3.Checked then Flag_Shift:=True else Flag_Shift:=False;
  if Flag_Shift then F1:=UpperCase(Filter) else F1:=Filter;
  for i:=1 to ListView1.LVCount do
  begin
    if Flag_Name then
    begin
      if Flag_Shift then F2:=UpperCase(Entries[i-1].Name) else F2:=Entries[i-1].Name;
      if Pos(F1,F2)>0 then ListView1.LVItemState[i-1]:=[lvisSelect];
    end;
    if Flag_Path then
    begin
      if Flag_Shift then F2:=UpperCase(Entries[i-1].Path) else F2:=Entries[i-1].Path;
      if Pos(F1,F2)>0 then ListView1.LVItemState[i-1]:=[lvisSelect];
    end;
  end;
  DefineSelected;
end;

procedure DefineSelected;
var
  i,fT: Integer;
begin
  fT:=0;
  for i:=1 to Form1.ListView1.LVCount do
  if lvisSelect in Form1.ListView1.LVItemState[i-1] then Inc(fT);
  Form1.Label11.Caption:=Int2Str(fT);
end;

procedure TForm1.KOLForm1Show(Sender: PObj);
begin
  InitAboutDialog(Form1.Form);
end;

procedure TForm1.KOLForm1N2Menu(Sender: PMenu; Item: Integer);
begin
  ShowAboutDialog('h4r Converter v1.01','21 июня 2002г.','Михаил Бесчетнов','collapse_forever@mail.ru','http://termius.narod.ru');
end;

procedure TForm1.ListView1Click(Sender: PObj);
begin
  DefineSelected;
end;

procedure TForm1.Button5Click(Sender: PObj);
var
  T: string;
  i: Integer;
  z: Integer;
  Allow: Boolean;
begin
  T:=GetOutDir;
  if T='' then Exit;
  OutDir:=T;
  //Распаковываем список
  Form2.Form.Show;
  fDat:=NewReadFileStream(OpenSaveDialog1.FileName);
  for i:=1 to ListView1.LVCount do
  begin
    Form2.Label1.Caption:='Распаковка ... '+Int2Str(Round(100*(i/ListView1.LVCount)))+'%';
    Form2.Form.Update;
    if Entries[i-1].Size>0 then Allow:=True else
    if not CheckBox1.Checked then Allow:=True else Allow:=False;
    if Allow then Extract(i-1);
  end;
  fDat.Free;
  Form2.Form.Hide;
end;

end.

