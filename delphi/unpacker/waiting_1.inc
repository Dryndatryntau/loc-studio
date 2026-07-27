{ KOL MCK } // Do not remove this line!

procedure NewForm2( var Result: PForm2; AParent: PControl );
begin

  New( Result, Create );
  Result.Form := NewForm( AParent, 'Подождите, пожалуйста...' ).SetPosition( 354, 266 ).SetSize( 325, 46 );
  Result.Form.Add2AutoFree( Result );
    Result.Form.Visible := False;
    Result.Form.Style := Result.Form.Style and not (WS_MINIMIZEBOX or WS_MAXIMIZEBOX);
    Result.Form.HasCaption := False;
      Result.Form.OnClose := Result.KOLForm1Close;
    // Result.Label1.TabOrder = -1
    Result.Label1 := NewLabel( Result.Form, 'Label1' ).SetAlign ( caClient );
    Result.Label1.Font.FontStyle := [ fsBold ];
    Result.Label1.Font.FontHeight := -14;
    Result.Label1.TextAlign := taCenter;
    Result.Label1.VerticalAlign := vaCenter;
    Result.Form.CenterOnParent;

end;

