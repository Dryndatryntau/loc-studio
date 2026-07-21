object Form4: TForm4
  Left = 0
  Top = 0
  Caption = 'Heroes studio'
  ClientHeight = 541
  ClientWidth = 792
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Menu = MainMenu1
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  TextHeight = 15
  object ComboFilter: TComboBox
    Left = 0
    Top = 0
    Width = 792
    Height = 23
    Align = alTop
    Style = csDropDownList
    TabOrder = 0
    ExplicitWidth = 718
  end
  object ListViewFiles: TListView
    Left = 0
    Top = 23
    Width = 792
    Height = 492
    Align = alClient
    Columns = <
      item
      end
      item
      end
      item
      end
      item
      end
      item
      end>
    OwnerData = True
    RowSelect = True
    TabOrder = 1
    ViewStyle = vsReport
    OnData = ListViewFilesData
    ExplicitWidth = 724
    ExplicitHeight = 418
  end
  object ProgressBar1: TProgressBar
    Left = 0
    Top = 515
    Width = 792
    Height = 26
    Align = alBottom
    TabOrder = 2
    Visible = False
    ExplicitLeft = 200
    ExplicitTop = 507
    ExplicitWidth = 150
  end
  object MainMenu1: TMainMenu
    Left = 80
    Top = 24
    object Failas1: TMenuItem
      Caption = 'Failas'
      object Ieiti1: TMenuItem
        Caption = 'Atidaryti'
        OnClick = Ieiti1Click
      end
      object Ieiti2: TMenuItem
        Caption = 'I'#353'eiti'
      end
    end
    object Veiksla1: TMenuItem
      Caption = 'Veiksmai'
      object Ipakuotipassirinkt1: TMenuItem
        Caption = 'I'#353'pakuoti passirinkt'#261
        OnClick = Ipakuotivisk1Click
      end
      object Ipakuotivisk1: TMenuItem
        Caption = 'I'#353'pakuoti visk'#261
        OnClick = Ipakuotivisk1Click
      end
    end
    object MenuPlugins: TMenuItem
      Caption = 'Plugins'
    end
    object Help1: TMenuItem
      Caption = 'Help'
      object Apie1: TMenuItem
        Caption = 'Apie'
        OnClick = Apie1Click
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = 'Heroes IV archyvai (*.h4r)|*.h4r|Visi failai (*.*)|*.*'
    Left = 232
    Top = 16
  end
end
