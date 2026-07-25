object Form2: TForm2
  Left = 0
  Top = 0
  Caption = 'Teksto eksportas ir importas'
  ClientHeight = 226
  ClientWidth = 394
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object Button1: TButton
    Left = 128
    Top = 48
    Width = 137
    Height = 25
    Caption = 'Eksportuoti tekst'#261
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 128
    Top = 88
    Width = 137
    Height = 25
    Caption = 'Importuoti tekst'#261
    TabOrder = 1
    OnClick = Button2Click
  end
  object OpenDialog1: TOpenDialog
    Left = 136
    Top = 120
  end
  object SaveDialog1: TSaveDialog
    Left = 192
    Top = 120
  end
end
