object Form3: TForm3
  Left = 0
  Top = 0
  Caption = 'Heroes of Might and magic licalization studio'
  ClientHeight = 606
  ClientWidth = 931
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object PageControl1: TPageControl
    Left = 0
    Top = 41
    Width = 931
    Height = 565
    ActivePage = TabArch
    Align = alClient
    TabOrder = 0
    ExplicitWidth = 929
    ExplicitHeight = 557
    object TabFonts: TTabSheet
      Caption = #352'riftai'
      object ListFonts: TListBox
        Left = 0
        Top = 0
        Width = 150
        Height = 535
        Align = alLeft
        ItemHeight = 15
        TabOrder = 0
        OnClick = ListFontsClick
        ExplicitHeight = 527
      end
      object PanelFontsRight: TPanel
        Left = 150
        Top = 0
        Width = 773
        Height = 535
        Align = alClient
        TabOrder = 1
        ExplicitWidth = 771
        ExplicitHeight = 527
        object Image1: TImage
          Left = 616
          Top = 0
          Width = 153
          Height = 153
          Proportional = True
          Stretch = True
        end
        object Image2: TImage
          Left = 1
          Top = 159
          Width = 771
          Height = 375
          Align = alBottom
          OnMouseDown = Image2MouseDown
        end
        object BtnOpenFont: TButton
          Left = 32
          Top = 16
          Width = 97
          Height = 25
          Caption = 'Atidaryti '#353'rift'#261
          TabOrder = 0
          OnClick = BtnOpenFontClick
        end
        object BtnSaveFont: TButton
          Left = 32
          Top = 47
          Width = 97
          Height = 25
          Caption = 'I'#353'saugoti '#353'rift'#261
          TabOrder = 1
        end
      end
    end
    object TabText: TTabSheet
      Caption = 'Tekstai'
      ImageIndex = 1
      object StringGrid1: TStringGrid
        Left = 0
        Top = 0
        Width = 923
        Height = 535
        Align = alClient
        TabOrder = 0
      end
    end
    object TabArch: TTabSheet
      Caption = 'Archyvai'
      ImageIndex = 2
      object Button1: TButton
        Left = 72
        Top = 104
        Width = 75
        Height = 25
        Caption = 'Button1'
        TabOrder = 0
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 931
    Height = 41
    Align = alTop
    TabOrder = 1
    ExplicitWidth = 929
    object ComboGame: TComboBox
      Left = 4
      Top = 12
      Width = 145
      Height = 23
      Style = csDropDownList
      TabOrder = 0
    end
    object ComboLang: TComboBox
      Left = 176
      Top = 12
      Width = 145
      Height = 23
      Style = csDropDownList
      TabOrder = 1
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 656
    Top = 65528
  end
  object SaveDialog1: TSaveDialog
    Left = 728
  end
end
