{ KOL MCK } // Do not remove this line!

procedure NewForm4( var Result: PForm4; AParent: PControl );
begin

  New( Result, Create );
  Result.Form := NewForm( AParent, 'Выберите тип формарования пути' ).SetPosition( 192, 114 ).SetSize( 351, 329 );
  Result.Form.Add2AutoFree( Result );
    Result.Form.Visible := False;
    Result.Form.Style := Result.Form.Style and not (WS_MINIMIZEBOX or WS_MAXIMIZEBOX);
    DeleteMenu( GetSystemMenu( Result.Form.GetWindowHandle, False ), SC_CLOSE, MF_BYCOMMAND );
      Result.Form.OnClose := Result.KOLForm1Close;
    // Result.RadioBox2.TabOrder = 0
    Result.RadioBox2 := NewRadioBox( Result.Form, 'Сформировывать пути из имен файлов (рекомендуется)' ).SetPosition( 8, 8 ).SetSize( 329, 0 );
    Result.RadioBox2.Font.FontHeight := -11;
      Result.RadioBox2.OnClick := Result.RadioBox2Click;
    Result.RadioBox2.SetRadioChecked;
    // Result.RadioBox1.TabOrder = 1
    Result.RadioBox1 := NewRadioBox( Result.Form, 'Использовать стандартные значения пути для файлов' ).SetPosition( 8, 32 ).SetSize( 329, 0 );
    Result.RadioBox1.Font.FontHeight := -11;
      Result.RadioBox1.OnClick := Result.RadioBox1Click;
    // Result.GroupBox1.TabOrder = 2
    Result.GroupBox1 := NewGroupBox( Result.Form, ' Описание ' ).SetPosition( 8, 56 ).SetSize( 329, 209 );
    Result.GroupBox1.TabStop := FALSE;
    Result.GroupBox1.Font.FontHeight := -11;
    // Result.Memo1.TabOrder = 0
      Result.Memo1 := NewEditBox( Result.GroupBox1, [ eoMultiline, eoNoHScroll, eoNoVScroll, eoReadonly ] ).SetPosition( 8, 16 ).SetSize( 313, 89 );
    Result.Memo1.Color := clBtnFace;
      Result.Memo1.Text := ''
 + 'При распаковке значения пути будут формироваться из имени файла, путем установки'
 + ' разделителей. Рекомендую использовать именно этот вариант.'+#13+#10
;
    // Result.GroupBox2.TabOrder = 1
      Result.GroupBox2 := NewGroupBox( Result.GroupBox1, ' Пример ' ).SetPosition( 8, 104 ).SetSize( 313, 97 );
    Result.GroupBox2.TabStop := FALSE;
    // Result.Label4.TabOrder = -1
        Result.Label4 := NewLabel( Result.GroupBox2, 'C:\Heroes 4\Assets\layers\button' ).SetPosition( 120, 32 ).SetSize( 185, 17 );
    Result.Label4.Font.Color := clNavy;
    // Result.Label2.TabOrder = -1
        Result.Label2 := NewLabel( Result.GroupBox2, 'Путь файла в архиве:' ).SetPosition( 8, 32 ).SetSize( 113, 17 );
    Result.Label2.Font.Color := clBlack;
    // Result.Label3.TabOrder = -1
        Result.Label3 := NewLabel( Result.GroupBox2, 'layers.button.cancel.h4r' ).SetPosition( 120, 16 ).SetSize( 185, 17 );
    Result.Label3.Font.Color := clNavy;
    // Result.Label1.TabOrder = -1
        Result.Label1 := NewLabel( Result.GroupBox2, 'Имя файла в архиве:' ).SetPosition( 8, 16 ).SetSize( 113, 17 );
    Result.Label1.Font.Color := clBlack;
    // Result.Label5.TabOrder = -1
        Result.Label5 := NewLabel( Result.GroupBox2, 'Конечный путь файла при распаковке:' ).SetPosition( 8, 48 ).SetSize( 201, 17 );
    Result.Label5.Font.Color := clBlack;
    // Result.EditBox1.TabOrder = 0
        Result.EditBox1 := NewEditBox( Result.GroupBox2, [ eoReadonly ] ).SetPosition( 16, 65 ).SetSize( 281, 0 );
    Result.EditBox1.Color := clWindow;
    Result.EditBox1.Font.Color := clBlack;
        Result.EditBox1.Text := 'layers\button\cancel.h4r';
    // Result.Button1.TabOrder = 3
    Result.Button1 := NewButton( Result.Form, 'Готово' ).SetPosition( 136, 272 );
    Result.Button1.Font.FontHeight := -11;
      Result.Button1.OnClick := Result.Button1Click;

end;


