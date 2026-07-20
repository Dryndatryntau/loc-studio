unit SPCAboutDialog;
interface
uses kol, windows, shellapi;

  procedure InitAboutDialog(fForm: PControl);
  procedure ShowAboutDialog(PName,PDate,PAuthor,PEMail,PWeb: string);
  procedure OkButtonClick(Sender: PObj);
  procedure LabelWebClick(Sender: PObj);

var
  AboutForm: PControl;
  OkButton: PControl;
  AboutLabelTitle,
  LLabelDate,RLabelDate,
  LLabelAuthor,RLabelAuthor,
  LLabelEMail,RLabelEMail,
  LLabelWeb,RLabelWeb: PControl;

implementation

procedure LabelWebClick;
begin
  ShellExecuteA(AboutForm.Handle,nil,PChar(RLabelWeb.Caption),'','',SW_SHOW);
end;

procedure LabelEmailClick;
begin
  ShellExecuteA(AboutForm.Handle,nil,PChar('mailto:'+RLabelEmail.Caption),'','',SW_SHOW);
end;

procedure OkButtonClick;
begin
  AboutForm.ModalResult:=1;
  AboutForm.Hide;
end;

procedure ShowAboutDialog;
begin
  AboutLabelTitle.Caption:=PName;
  RLabelDate.Caption:=PDate;
  RLabelAuthor.Caption:=PAuthor;
  RLabelEMail.Caption:=PEMail;
  RLabelWeb.Caption:=PWeb;
  AboutForm.ShowModal;
end;

procedure InitAboutDialog(fForm: PControl);
begin
//Инициализация формы
  AboutForm:=NewForm(fForm,'О Программе...');
  AboutForm.Visible:=False;
  AboutForm.Width:=240;
  AboutForm.Height:=150;
  AboutForm.CenterOnParent;
  AboutForm.CanResize:=False;
  AboutForm.ClsStyle:=AboutForm.ClsStyle xor CS_NOCLOSE;
  AboutForm.Style:=AboutForm.Style xor WS_MINIMIZEBOX xor WS_MAXIMIZEBOX or WS_CHILDWINDOW xor WS_CAPTION;

//Создание заголовка
  AboutLabelTitle:=NewLabel(AboutForm,'');
  AboutLabelTitle.Align:=caTop;
  AboutLabelTitle.Height:=22;
  AboutLabelTitle.Font.FontHeight:=-14;
  AboutLabelTitle.Font.Color:=clGreen;
  AboutLabelTitle.Font.FontStyle:=[fsBold];
  AboutLabelTitle.TextAlign:=taCenter;
  AboutLabelTitle.VerticalAlign:=vaCenter;
//Создание надписи даты
  LLabelDate:=NewLabel(AboutForm,'Дата:');
  LLabelDate.Font.FontHeight:=-11;
  LLabelDate.Left:=2;
  LLabelDate.Top:=26;
  LLabelDate.Width:=40;
  RLabelDate:=NewLabel(AboutForm,'');
  RLabelDate.Font.Color:=clNavy;
  RLabelDate.Font.FontHeight:=-11;
  RLabelDate.Left:=42;
  RLabelDate.Top:=26;
  RLabelDate.Width:=200;
//Создание надписи автора
  LLabelAuthor:=NewLabel(AboutForm,'Автор:');
  LLabelAuthor.Font.FontHeight:=-11;
  LLabelAuthor.Left:=2;
  LLabelAuthor.Top:=48;
  LLabelAuthor.Width:=40;
  RLabelAuthor:=NewLabel(AboutForm,'');
  RLabelAuthor.Font.Color:=clNavy;
  RLabelAuthor.Font.FontHeight:=-11;
  RLabelAuthor.Left:=42;
  RLabelAuthor.Top:=48;
  RLabelAuthor.Width:=200;
//Создание надписи автора
  LLabelEMail:=NewLabel(AboutForm,'E-mail:');
  LLabelEMail.Font.FontHeight:=-11;
  LLabelEMail.Left:=2;
  LLabelEMail.Top:=70;
  LLabelEMail.Width:=40;
  RLabelEMail:=NewLabel(AboutForm,'');
  RLabelEMail.Font.Color:=clNavy;
  RLabelEMail.Font.FontHeight:=-11;
  RLabelEMail.Left:=42;
  RLabelEMail.Top:=70;
  RLabelEMail.Width:=200;
  RLabelEMail.Height:=17;
  RLabelEMail.OnClick:=TOnEvent(MakeMethod(nil,@LabelEmailClick));
  RLabelEMail.Font.Color:=$00FF0000;
  RLabelEMail.Font.FontStyle:=[fsUnderline];
  RLabelEMail.Cursor:=45;
//Создание надписи автора
  LLabelWeb:=NewLabel(AboutForm,'Web:');
  LLabelWeb.Font.FontHeight:=-11;
  LLabelWeb.Left:=2;
  LLabelWeb.Top:=92;
  LLabelWeb.Width:=40;
  RLabelWeb:=NewLabel(AboutForm,'');
  RLabelWeb.Font.Color:=clNavy;
  RLabelWeb.Font.FontHeight:=-11;
  RLabelWeb.Left:=42;
  RLabelWeb.Top:=92;
  RLabelWeb.Width:=200;
  RLabelWeb.Height:=17;
  RLabelWeb.OnClick:=TOnEvent(MakeMethod(nil,@LabelWebClick));
  RLabelWeb.Font.Color:=$00FF0000;
  RLabelWeb.Font.FontStyle:=[fsUnderline];
  RLabelWeb.Cursor:=45;
//Создание кнопки закрытия
  OkButton:=NewButton(AboutForm,'Ok');
  OkButton.Font.FontHeight:=-11;
  OkButton.OnClick:=TOnEvent(MakeMethod(nil,@OkButtonClick));
  OkButton.Top:=AboutForm.ClientHeight-OkButton.Height-8;
  OkButton.Left:=AboutForm.ClientWidth div 2-OkButton.Width div 2;
end;
begin
end.
