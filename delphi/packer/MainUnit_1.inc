{ KOL MCK } // Do not remove this line!
const N1 = 0;
const N3 = 1;
const N6 = 2;
const N5 = 3;
const N2 = 4;


procedure NewForm1( var Result: PForm1; AParent: PControl );
begin

  New( Result, Create );
  Result.Form := NewForm( AParent, 'HOM&M IV h4r packer v1.0' ).SetPosition( 335, 132 ).SetSize( 396, 425 );
  Result.Form.Add2AutoFree( Result );
    Result.Form.Font.FontHeight := -11;
    Result.Form.Border := 0;

    Result.MainMenu1 := NewMenu( Result.Form, 0, [ 'Файлы', '('
      , 'Загрузить папку с ресурсами...', '-'
      , 'Сохранить ресурсы в новый архив...', '-', 'Выход', ')'
      , 'О Программе...', '' ], nil );
    Result.MainMenu1.AssignEvents( 1, [ Result.KOLForm1N3Menu ] );
    Result.MainMenu1.ItemEnabled[ 2 ] := False;
    Result.MainMenu1.AssignEvents( 2, [ Result.MainMenu1N6Menu ] );
    Result.MainMenu1.AssignEvents( 3, [ Result.KOLForm1N5Menu ] );
    Result.MainMenu1.AssignEvents( 4, [ Result.KOLForm1N2Menu ] );

    Result.OpenDirDialog1 := NewOpenDirDialog( '', [  ] );
  Result.Form.Add2AutoFree( Result.OpenDirDialog1 );

    Result.OpenSaveDialog1 := NewOpenSaveDialog( '', '', [  ] );
      Result.OpenSaveDialog1.Filter := 'HOM&M IV archives (*.h4r)|*.h4r';
      Result.OpenSaveDialog1.OpenDialog := FALSE;
      Result.OpenSaveDialog1.DefExtension := 'h4r';
  Result.Form.Add2AutoFree( Result.OpenSaveDialog1 );
    // Result.Label1.TabOrder = -1
    Result.Label1 := NewLabel( Result.Form, 'Список файлов:' ).SetAlign ( caTop ).SetSize( 0, 14 );
    // Result.ListBox1.TabOrder = 0
    Result.ListBox1 := NewListBox( Result.Form, [ loNoIntegralHeight ] ).SetAlign ( caClient );
    Result.ListBox1.Color := clWindow;
    Result.ListBox1.CurIndex := 0;
    // Result.Panel1.TabOrder = 1
    Result.Panel1 := NewPanel( Result.Form, esNone ).SetAlign ( caBottom ).SetSize( 0, 35 );
    Result.Panel1.Border := 2;
    // Result.Label3.TabOrder = -1
      Result.Label3 := NewLabel( Result.Panel1, 'Общий размер:' ).SetPosition( 8, 19 ).SetSize( 81, 17 );
    // Result.Label5.TabOrder = -1
      Result.Label5 := NewLabel( Result.Panel1, '0' ).SetPosition( 96, 19 ).SetSize( 113, 17 );
    Result.Label5.Font.Color := clNavy;
    // Result.Label4.TabOrder = -1
      Result.Label4 := NewLabel( Result.Panel1, '0' ).SetPosition( 96, 3 ).SetSize( 113, 17 );
    Result.Label4.Font.Color := clNavy;
    // Result.Label2.TabOrder = -1
      Result.Label2 := NewLabel( Result.Panel1, 'Всего файлов:' ).SetPosition( 8, 3 ).SetSize( 81, 17 );
      Result.OpenDirDialog1.WndOwner := Result.Form.GetWindowHandle;
      Result.OpenSaveDialog1.WndOwner := Result.Form.GetWindowHandle;
    Result.Form.CenterOnParent;
    Result.KOLForm1FormCreate( Result );

end;

