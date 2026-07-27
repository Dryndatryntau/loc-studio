{ KOL MCK } // Do not remove this line!
const N1 = 0;
const N3 = 1;
const N5 = 2;
const N2 = 3;


procedure NewForm1( var Result: PForm1; AParent: PControl );
begin

  New( Result, Create );
  Result.Form := NewForm( AParent, 'h4r Converter v1.01 (Heroes of Might & Magic IV)' ).SetPosition( 175, 167 ).SetSize( 765, 388 );
  Result.Form.Add2AutoFree( Result );
      Result.Form.OnShow := Result.KOLForm1Show;

    Result.MainMenu1 := NewMenu( Result.Form, 0, [ 'Файл', '('
      , 'Открыть архив...', '-', 'Выход', ')', 'О Программе...'
      , '' ], nil );
    Result.MainMenu1.AssignEvents( 1, [ Result.KOLForm1N3Menu ] );
    Result.MainMenu1.AssignEvents( 2, [ Result.KOLForm1N5Menu ] );
    Result.MainMenu1.AssignEvents( 3, [ Result.KOLForm1N2Menu ] );

    Result.OpenSaveDialog1 := NewOpenSaveDialog( '', '', [  ] );
      Result.OpenSaveDialog1.Filter := 'HOM&M IV Archives (*.h4r)|*.h4r';
      Result.OpenSaveDialog1.DefExtension := 'h4r';
  Result.Form.Add2AutoFree( Result.OpenSaveDialog1 );

    Result.OpenDirDialog1 := NewOpenDirDialog( 'Укажите путь для распаковки', [  ] );
      Result.OpenDirDialog1.CenterOnScreen := TRUE;
  Result.Form.Add2AutoFree( Result.OpenDirDialog1 );
    // Result.ListView1.TabOrder = 0
    Result.ListView1 := NewListView( Result.Form, lvsDetail, [ lvoMultiselect, lvoRowSelect ], nil, nil, nil ).SetAlign ( caClient );
    Result.ListView1.Color := clWindow;
      Result.ListView1.OnClick := Result.ListView1Click;
    // Result.Panel1.TabOrder = 4
    Result.Panel1 := NewPanel( Result.Form, esNone ).SetAlign ( caRight ).SetSize( 189, 0 );
    Result.Panel1.Border := 0;
    // Result.GroupBox2.TabOrder = 1
      Result.GroupBox2 := NewGroupBox( Result.Panel1, ' Действия ' ).SetPosition( 2, 120 ).SetSize( 183, 217 );
    Result.GroupBox2.TabStop := FALSE;
    Result.GroupBox2.Font.FontHeight := -11;
      Result.GroupBox2.Border := 2;
    // Result.Label10.TabOrder = -1
        Result.Label10 := NewLabel( Result.GroupBox2, 'Выделено:' ).SetPosition( 8, 112 ).SetSize( 0, 17 );
    Result.Label10.Font.Color := clBlack;
    Result.Label10.VerticalAlign := vaCenter;
    // Result.Label11.TabOrder = -1
        Result.Label11 := NewLabel( Result.GroupBox2, '0' ).SetPosition( 72, 112 ).SetSize( 105, 17 );
    Result.Label11.Font.Color := clNavy;
    Result.Label11.VerticalAlign := vaCenter;
    // Result.Button1.TabOrder = 0
        Result.Button1 := NewButton( Result.GroupBox2, 'Выделить все' ).SetPosition( 8, 16 ).SetSize( 169, 0 );
        Result.Button1.Enabled := False;
      Result.Button1.OnClick := Result.Button1Click;
    // Result.Button2.TabOrder = 1
        Result.Button2 := NewButton( Result.GroupBox2, 'Выделить только сжатые' ).SetPosition( 8, 40 ).SetSize( 169, 0 );
        Result.Button2.Enabled := False;
      Result.Button2.OnClick := Result.Button2Click;
    // Result.CheckBox1.TabOrder = 2
        Result.CheckBox1 := NewCheckBox( Result.GroupBox2, 'Игнорировать пустые файлы' ).SetPosition( 8, 131 ).SetSize( 169, 0 );
        Result.CheckBox1.Checked := TRUE;
    // Result.Button3.TabOrder = 2
        Result.Button3 := NewButton( Result.GroupBox2, 'Инвертировать выделение' ).SetPosition( 8, 64 ).SetSize( 169, 0 );
        Result.Button3.Enabled := False;
      Result.Button3.OnClick := Result.Button3Click;
    // Result.Button4.TabOrder = 3
        Result.Button4 := NewButton( Result.GroupBox2, 'Распаковать выделенное в...' ).SetPosition( 8, 160 ).SetSize( 169, 0 );
        Result.Button4.Enabled := False;
      Result.Button4.OnClick := Result.Button4Click;
    // Result.Button5.TabOrder = 4
        Result.Button5 := NewButton( Result.GroupBox2, 'Распаковать все в...' ).SetPosition( 8, 184 ).SetSize( 169, 0 );
        Result.Button5.Enabled := False;
      Result.Button5.OnClick := Result.Button5Click;
    // Result.Button6.TabOrder = 5
        Result.Button6 := NewButton( Result.GroupBox2, 'Выделение по маске...' ).SetPosition( 8, 88 ).SetSize( 169, 0 );
        Result.Button6.Enabled := False;
      Result.Button6.OnClick := Result.Button6Click;
    // Result.GroupBox1.TabOrder = 3
      Result.GroupBox1 := NewGroupBox( Result.Panel1, ' Текущий архив ' ).SetPosition( 2, 0 ).SetSize( 183, 115 );
    Result.GroupBox1.TabStop := FALSE;
    Result.GroupBox1.Font.FontHeight := -11;
      Result.GroupBox1.Border := 2;
    // Result.Label6.TabOrder = -1
        Result.Label6 := NewLabel( Result.GroupBox1, '0' ).SetPosition( 96, 96 ).SetSize( 0, 17 );
    Result.Label6.Font.Color := clNavy;
    // Result.Label2.TabOrder = -1
        Result.Label2 := NewLabel( Result.GroupBox1, 'Сжатых:' ).SetPosition( 8, 64 ).SetSize( 81, 17 );
    Result.Label2.Font.Color := clBlack;
    // Result.Label3.TabOrder = -1
        Result.Label3 := NewLabel( Result.GroupBox1, 'Общий размер:' ).SetPosition( 8, 96 ).SetSize( 81, 17 );
    Result.Label3.Font.Color := clBlack;
    // Result.Label4.TabOrder = -1
        Result.Label4 := NewLabel( Result.GroupBox1, '0' ).SetPosition( 96, 48 ).SetSize( 0, 17 );
    Result.Label4.Font.Color := clNavy;
    // Result.Label5.TabOrder = -1
        Result.Label5 := NewLabel( Result.GroupBox1, '0' ).SetPosition( 96, 64 ).SetSize( 0, 17 );
    Result.Label5.Font.Color := clNavy;
    // Result.Label1.TabOrder = -1
        Result.Label1 := NewLabel( Result.GroupBox1, 'Всего файлов:' ).SetPosition( 8, 48 ).SetSize( 81, 17 );
    Result.Label1.Font.Color := clBlack;
    // Result.Label7.TabOrder = -1
        Result.Label7 := NewLabel( Result.GroupBox1, 'Пустых:' ).SetPosition( 8, 80 ).SetSize( 81, 17 );
    Result.Label7.Font.Color := clBlack;
    // Result.Label8.TabOrder = -1
        Result.Label8 := NewLabel( Result.GroupBox1, '0' ).SetPosition( 96, 80 ).SetSize( 0, 17 );
    Result.Label8.Font.Color := clNavy;
    // Result.Label9.TabOrder = -1
        Result.Label9 := NewLabel( Result.GroupBox1, 'Архив:' ).SetPosition( 8, 24 ).SetSize( 33, 17 );
    Result.Label9.Font.Color := clBlack;
    Result.Label9.VerticalAlign := vaCenter;
    // Result.EditBox1.TabOrder = 0
        Result.EditBox1 := NewEditBox( Result.GroupBox1, [ eoReadonly ] ).SetPosition( 48, 24 ).SetSize( 129, 0 );
    Result.EditBox1.Color := clBtnFace;
    Result.EditBox1.Font.Color := clNavy;
        Result.EditBox1.Text := '<не открыт>';
      Result.OpenSaveDialog1.WndOwner := Result.Form.GetWindowHandle;
      Result.OpenDirDialog1.WndOwner := Result.Form.GetWindowHandle;
    Result.Form.CenterOnParent;
    Result.KOLForm1FormCreate( Result );

end;

