{ KOL MCK } // Do not remove this line!

procedure NewForm3( var Result: PForm3; AParent: PControl );
begin

  New( Result, Create );
  Result.Form := NewForm( AParent, 'Выделение по маске' ).SetPosition( 192, 107 ).SetSize( 293, 158 );
  Result.Form.Add2AutoFree( Result );
    Result.Form.Visible := False;
    Result.Form.Style := Result.Form.Style and not (WS_MINIMIZEBOX or WS_MAXIMIZEBOX);
    DeleteMenu( GetSystemMenu( Result.Form.GetWindowHandle, False ), SC_CLOSE, MF_BYCOMMAND );
      Result.Form.OnClose := Result.KOLForm1Close;

    Result.Timer1 := NewTimer( 50 );
  Result.Form.Add2AutoFree( Result.Timer1 );
      Result.Timer1.OnTimer := Result.Timer1Timer;
    // Result.Label1.TabOrder = -1
    Result.Label1 := NewLabel( Result.Form, 'Выделить строки, содержащие подстроку:' ).SetPosition( 8, 8 ).SetSize( 273, 0 );
    Result.Label1.Font.Color := clBlack;
    Result.Label1.Font.FontHeight := -11;
    // Result.EditBox1.TabOrder = 0
    Result.EditBox1 := NewEditBox( Result.Form, [  ] ).SetPosition( 8, 32 ).SetSize( 273, 0 );
    Result.EditBox1.Color := clWindow;
    Result.EditBox1.Font.Color := clBlack;
    Result.EditBox1.Font.FontHeight := -11;
    // Result.CheckBox1.TabOrder = 1
    Result.CheckBox1 := NewCheckBox( Result.Form, 'Искать в значении Имени' ).SetPosition( 8, 56 ).SetSize( 201, 0 );
    Result.CheckBox1.Font.FontHeight := -11;
    Result.CheckBox1.Checked := TRUE;
    // Result.CheckBox2.TabOrder = 2
    Result.CheckBox2 := NewCheckBox( Result.Form, 'Искать в значении Пути' ).SetPosition( 8, 80 ).SetSize( 201, 0 );
    Result.CheckBox2.Font.FontHeight := -11;
    // Result.Button1.TabOrder = 3
    Result.Button1 := NewButton( Result.Form, 'Выделить' ).SetPosition( 216, 80 );
    Result.Button1.Font.FontHeight := -11;
      Result.Button1.OnClick := Result.Button1Click;
    // Result.Button2.TabOrder = 4
    Result.Button2 := NewButton( Result.Form, 'Отмена' ).SetPosition( 216, 104 );
    Result.Button2.Font.FontHeight := -11;
      Result.Button2.OnClick := Result.Button2Click;
    // Result.CheckBox3.TabOrder = 5
    Result.CheckBox3 := NewCheckBox( Result.Form, 'Игнорировать регистр' ).SetPosition( 8, 104 ).SetSize( 201, 0 );
    Result.CheckBox3.Font.FontHeight := -11;
    Result.CheckBox3.Checked := TRUE;
    Result.Timer1.Enabled := True;
    Result.Form.CenterOnParent;

end;

