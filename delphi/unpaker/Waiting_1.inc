{ KOL MCK } // Do not remove this line!

procedure NewForm2( var Result: PForm2; AParent: PControl );
begin

  New( Result, Create );
  Result.Form := NewForm( AParent, 'Form2' ).SetPosition( 266, 189 ).SetSize( 320, 66 );
  Result.Form.Add2AutoFree( Result );
    Result.Form.Visible := False;
    Result.Form.HasCaption := False;
      Result.Form.OnClose := Result.KOLForm1Close;
    // Result.Label1.TabOrder = -1
    Result.Label1 := NewLabel( Result.Form, 'Открываем архив...' ).SetAlign ( caClient );
    Result.Label1.Font.Color := clBlack;
    Result.Label1.Font.FontStyle := [ fsBold ];
    Result.Label1.Font.FontHeight := -16;
    Result.Label1.Font.FontName := 'Arial';
    Result.Label1.TextAlign := taCenter;
    Result.Label1.VerticalAlign := vaCenter;
    Result.Form.CenterOnParent.CanResize := False;

end;

