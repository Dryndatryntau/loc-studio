object Form1: TForm1
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Heroes 4 '#353'rift'#371' redaktorius'
  ClientHeight = 574
  ClientWidth = 1244
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object Image1: TImage
    Left = 0
    Top = 216
    Width = 1244
    Height = 350
    OnMouseDown = Image1MouseDown
  end
  object Image2: TImage
    Left = 1087
    Top = 0
    Width = 157
    Height = 202
  end
  object Label1: TLabel
    Left = 926
    Top = 102
    Width = 95
    Height = 30
    Caption = '31 su 2 kvadratais,'#13#10' 32 - su 1'
  end
  object Label2: TLabel
    Left = 926
    Top = 18
    Width = 43
    Height = 15
    Caption = 'Mastelis'
  end
  object LabelFailas: TLabel
    Left = 104
    Top = 16
    Width = 3
    Height = 15
  end
  object Button1: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Atidaryti fail'#261
    TabOrder = 0
    OnClick = Button1Click
  end
  object ListBox1: TListBox
    Left = 799
    Top = 38
    Width = 121
    Height = 172
    ItemHeight = 15
    TabOrder = 1
    OnClick = ListBox1Click
  end
  object Memo1: TMemo
    Left = 0
    Top = 39
    Width = 793
    Height = 171
    TabOrder = 2
  end
  object Edit1: TEdit
    Left = 922
    Top = 39
    Width = 60
    Height = 23
    TabOrder = 3
    Text = '1'
    OnChange = Edit1Change
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 555
    Width = 1244
    Height = 19
    Panels = <>
    ExplicitTop = 547
    ExplicitWidth = 1242
  end
  object Button2: TButton
    Left = 988
    Top = 8
    Width = 93
    Height = 25
    Caption = 'Importuoti i'#353' ttf'
    TabOrder = 5
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 988
    Top = 39
    Width = 93
    Height = 25
    Caption = 'I'#353'saugoti'
    TabOrder = 6
    OnClick = Button3Click
  end
  object Edit2: TEdit
    Left = 922
    Top = 138
    Width = 60
    Height = 23
    TabOrder = 7
    Text = '31'
  end
  object Button4: TButton
    Left = 926
    Top = 167
    Width = 155
    Height = 25
    Caption = 'Generuoti visk'#261
    TabOrder = 8
    OnClick = Button4Click
  end
  object OpenDialog1: TOpenDialog
    Left = 1032
    Top = 496
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Segoe UI'
    Font.Style = []
    Left = 1080
    Top = 344
  end
end
