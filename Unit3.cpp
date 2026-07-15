#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <windows.h>
#include <memory>

#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;

// PAGRINDINIAI PROGRAMOS KINTAMIEJI
const int buf_size = 50000;
const int point = 5; // Redagavimo tinklelio mastelis

// TRIGUBAS MASYVAS: Pirmas indeksas visada yra simbolio ID [300]
TColor senasp[300][48][48];

long int senas = -1;
long int sr_count = 0;
bool pakeistas_simbolis[300] = {false};
String s;

// Žaidimo šrifto raidės struktūra
struct sriftai {
	long int foffset;
	long int plotis;
	long int ilgis;
	long int lmargin;
	long int rmargin;
	long int srx;
	long int sry;
};
sriftai sr[300];

// Pagalbinė funkcija 4 baitų skaičiams nuskaityti
long getLong(int f) {
	long l;
	_read(f, &l, 4);
	return l;
}

// KONSTRUKTORIUS
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
}

// MYGTUKAS: ATIDARYTI FAILĄ
void __fastcall TForm1::Button1Click(TObject *Sender)
{
  if (OpenDialog1->Execute()){
	NuskaitytiFaila(Sender);
  }
}

// FUNKCIJA: FAILO NUSKAITYMAS
void __fastcall TForm1::NuskaitytiFaila(TObject *Sender)
{
  // Švariai išvalome abu grafinius laukus tamsiai mėlyna spalva
  Image1->Canvas->Brush->Color = clNavy;
  Image1->Canvas->FillRect(Image1->ClientRect);
  Image2->Canvas->Brush->Color = clNavy;
  Image2->Canvas->FillRect(Image2->ClientRect);

  senas = -1;
  Memo1->Clear();
  ListBox1->Clear();

  // Išvalome pakeitimų sekimo masyvą
  for (int i = 0; i < 300; i++) {
	pakeistas_simbolis[i] = false;
  }

  if (OpenDialog1->FileName == ""){
	Memo1->Lines->Add("Nenurodytas failas");
	return;
  }

  // Parodome pilną įkelto failo kelią ekrane jūsų sukurtame laukelyje!
  LabelFailas->Caption = OpenDialog1->FileName;

  int failas = _wopen(OpenDialog1->FileName.c_str(), _O_RDONLY | _O_BINARY);
  if (failas == -1) {
	Memo1->Lines->Add("Klaida atidarant šrifto failą!");
	return;
  }

  int i, j, x, y;
  int px = 4, py = 4, cpy = 1;
  int d = StrToInt(Edit1->Text);
  if (d <= 0) d = 2;

  std::vector<unsigned char> buf(buf_size);
  long sk;
  String eil = "";

  _lseek(failas, 7, SEEK_SET); // Praleidžiame 7 baitų žaidimo antraštę
  sr_count = 0;

  // Pradedame binarinį failo nuskaitymo ciklą
  while (_read(failas, &x, 4) > 0) {
	_lseek(failas, -4, SEEK_CUR); // Grąžiname rodyklę atgal getLong() funkcijai

	// Užfiksuojame tikslų šio simbolio offsetą faile
	sr[sr_count].foffset = _lseek(failas, 0, SEEK_CUR);

	// Įrašome į sąrašą
	ListBox1->Items->Add(eil.sprintf(L"%3d [%8X]", sr_count + 1, sr[sr_count].foffset));

	// Nuskaitome 4 pagrindinius žaidimo šrifto parametrus
	sr[sr_count].plotis = x = getLong(failas);
	sr[sr_count].ilgis = y = getLong(failas);
	sr[sr_count].lmargin = getLong(failas);
	sr[sr_count].rmargin = getLong(failas);

	// Tikriname, ar raidė telpa į dabartinę eilutę didžiajame Image1 žemėlapyje
	if ((px + (x * d) + (d * 2)) > Image1->Width) {
	  px = 4;
	  py += cpy + (d * 2);
	  cpy = 1;
	}

	if (cpy < (y * d)) cpy = y * d;

	// Išsaugome tikslias vizualias koordinates pelės paspaudimo sekimui
	sr[sr_count].srx = px;
	sr[sr_count].sry = py;

	// Nuskaitome tikslų šios raidės pikselių kiekį iš disko
	_read(failas, buf.data(), x * y);

	// Braižome raidę bendrame žemėlapyje (Image1) ir užpildome asmeninę atmintį
	for (i = 0; i < y; i++) {
	  for (j = 0; j < x; j++) {
		unsigned char raw_byte = buf[x * i + j];

		// IŠTAISYTA ČIA: Apverčiame spalvą iškart nuskaitymo metu (255 - reikšmė).
		// Žaidimo fonas (255) redaktoriuje pavirs švariu baltu fonu (0),
		// o žaidimo raidė (0) pavirs juodu tekstu (255). Akims patogus stilius sugrįžo!
		sk = 255 - raw_byte;

		// Išsaugome į šios raidės asmeninį stalčiuką trigubame masyve
		senasp[sr_count][j][i] = TColor(RGB(sk, sk, sk));

		// Nupiešiame tašką Image1 drobėje
		Image1->Canvas->Pen->Color = TColor(RGB(sk, sk, sk));
		Image1->Canvas->Brush->Color = TColor(RGB(sk, sk, sk));
		Image1->Canvas->Rectangle(px + (j * d), py + (i * d), px + (j * d) + d + 1, py + (i * d) + d + 1);
	  }
	}

	// Pastumiame x koordinates sekančiai raidei
	px = px + (x * d) + (d * 2);
	sr_count++;
  }

  long failo_dydis = _lseek(failas, 0, SEEK_END);
  _close(failas);

  Memo1->Lines->Add("Įkelta sėkmingai!");
  Memo1->Lines->Add(s.sprintf(L"Simbolių faile: %d", sr_count));
  Memo1->Lines->Add(s.sprintf(L"Failo dydis: %d B", failo_dydis));

  // Jei failas sėkmingai įkeltas, automatiškai aktyvuojame pirmo simbolio rodymą
  if (sr_count > 0) {
	ListBox1->ItemIndex = 0;
	rodyk(0);
  }

  Image1->Refresh();
}

// FUNKCIJA: RAIDĖS IŠDIDINIMAS REDAGAVIMUI
void TForm1::rodyk(int sr_sk)
{
  int i, j, x, y, d = point;

  if (sr_sk < 0 || sr_sk >= sr_count) return;

  // 1. Priverstinai visą Image2 plotą užliejame tamsiai mėlyna spalva (clNavy)
  // Tai akimirksniu sugrąžins mėlyną foną UŽ raidės ribų!
  Image2->Canvas->Brush->Color = clNavy;
  Image2->Canvas->Pen->Color = clNavy;
  Image2->Canvas->FillRect(Image2->ClientRect);

  x = sr[sr_sk].plotis;
  y = sr[sr_sk].ilgis;

  // 2. Braižome pačios raidės pikselius iš trigubo masyvo
  for (i = 0; i < y; i++) {
	for (j = 0; j < x; j++) {
	  TColor pikselio_spalva = senasp[sr_sk][j][i];

	  // Kadangi redaktoriuje naudojame šviesų stilių (baltas fonas, juoda raidė),
	  // o visą bendrą lauką ką tik nudažėme mėlynai:
	  // Kiekvienas sugeneruotas Tahoma taškas gaus savo tikrąją spalvą (baltą arba juodą).
	  // Tai sukurs idealų baltą kvadratą pačiai raidei, o už jos rėmų viskas liks gražiai mėlyna!
	  Image2->Canvas->Pen->Color = pikselio_spalva;
	  Image2->Canvas->Brush->Color = pikselio_spalva;
	  Image2->Canvas->Rectangle((j * d), (i * d), (j * d) + d + 1, (i * d) + d + 1);
	}
  }

  // Įsimename naują pasirinkto simbolio indeksą
  senas = sr_sk;

  // Priverstinai iškviečiame Image1 žemėlapio perpiešimą, kad pasirinktas simbolis invertuotųsi apačioje
  Edit1Change(Form1);

  Image2->Invalidate();
  Image2->Repaint();

  // Užrašome informaciją žurnale
  Memo1->Lines->Add(s.sprintf(L"Pasirinkta: ID %d, Dydis: %d x %d", sr_sk + 1, x, y));
}

// MYGTUKAS: IMPORTUOTI IŠ TTF
void __fastcall TForm1::Button2Click(TObject *Sender)
{
  if (ListBox1->ItemIndex == -1) {
	ShowMessage("Pasirinkite raidę iš sąrašo!");
	return;
  }

  int atrinktas_simbolis = ListBox1->ItemIndex;
  senas = atrinktas_simbolis;

  if (FontDialog1->Execute()) {
	int vartotojo_poslinkis = StrToInt(Edit2->Text);
	unsigned char ansi_kodas = (unsigned char)(atrinktas_simbolis + vartotojo_poslinkis);

	LOGFONT lf;
	GetObject(FontDialog1->Font->Handle, sizeof(LOGFONT), &lf);

	CHARSETINFO csi;
	UINT codePage = CP_ACP;
	if (TranslateCharsetInfo((DWORD*)(DWORD_PTR)lf.lfCharSet, &csi, TCI_SRCCHARSET)) {
		codePage = csi.ciACP;
	}

	wchar_t raide;
	MultiByteToWideChar(codePage, 0, (char*)&ansi_kodas, 1, &raide, 1);

	std::unique_ptr<TBitmap> bmp(new TBitmap());
	bmp->Canvas->Font->Assign(FontDialog1->Font);
	bmp->Canvas->Font->Color = clWhite;
	bmp->Canvas->Brush->Color = clBlack;

	String tekstas = String(raide);
	int ttf_plotis = bmp->Canvas->TextWidth(tekstas);
	int ttf_ilgis = bmp->Canvas->TextHeight(tekstas);

	if (ttf_plotis <= 0 || ttf_plotis > 48) ttf_plotis = 12;
	if (ttf_ilgis <= 0 || ttf_ilgis > 48) ttf_ilgis = 16;

	bmp->Width = ttf_plotis;
	bmp->Height = ttf_ilgis;
	bmp->Canvas->TextOut(0, 0, tekstas);

	sr[atrinktas_simbolis].plotis = ttf_plotis;
	sr[atrinktas_simbolis].ilgis = ttf_ilgis;

	// Įrašome paraščių nustatymus (lmargin ir rmargin)
	sr[atrinktas_simbolis].lmargin = 0;

	// Tahoma 9 Bold atrastas magiškas 4px suspaudimas rankiniam režimui
	int spaustas_zingsnis = ttf_plotis - 4;
	if (spaustas_zingsnis < 4) spaustas_zingsnis = 4;
	sr[atrinktas_simbolis].rmargin = spaustas_zingsnis;

	// Nukreipiame binarinius pikselius į trigubą masyvą
	for (int i = 0; i < ttf_ilgis; i++) {
	  for (int j = 0; j < ttf_plotis; j++) {
		TColor spalva = bmp->Canvas->Pixels[j][i];
		unsigned char pilkumas = GetRValue(spalva);

		// Įrašome šviesiuoju stiliumi (juoda ant balto redaktoriui)
		senasp[atrinktas_simbolis][j][i] = (TColor)RGB(255 - pilkumas, 255 - pilkumas, 255 - pilkumas);
	  }
	}

	pakeistas_simbolis[atrinktas_simbolis] = true;
	rodyk(atrinktas_simbolis);

	// 🎯 Štai ši eilutė akimirksniu atnaujina apatinį žemėlapį Image1!
	Edit1Change(Sender);

	Memo1->Lines->Add(s.sprintf(L"TTF įkeltas: %d x %d (CodePage: %d)", ttf_plotis, ttf_ilgis, codePage));
  }
}


// MYGTUKAS: IŠSAUGOTI PAKEITIMUS Į FAILĄ
void __fastcall TForm1::Button3Click(TObject *Sender)
{
  if (sr_count <= 0) {
	ShowMessage("Nėra ko išsaugoti. Pirmiausia įkelkite failą ir sugeneruokite šriftą!");
	return;
  }

  String fname = OpenDialog1->FileName + ".bak";
  DeleteFile(fname);

  // Sukuriame atsarginę originalo kopiją prieš perrašymą
  if (!RenameFile(OpenDialog1->FileName, fname)) {
	ShowMessage("Klaida: Nepavyko sukurti .bak failo atsarginės kopijos!");
	return;
  }

  // Atidarome seną kopiją skaitymui (kad paimtume 7 baitų originalų žaidimo headerį)
  int fstr_in = _wopen(fname.c_str(), _O_RDONLY | _O_BINARY);
  // Sukuriame visiškai naują švarią šrifto versiją diske įrašymui
  int fstr_out = _wopen(OpenDialog1->FileName.c_str(), _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);

  if (fstr_in == -1 || fstr_out == -1){
	if (fstr_in != -1) _close(fstr_in);
	ShowMessage("Klaida atidarant binarinius failus išsaugojimui!");
	return;
  }

  // 1 ŽINGSNIS: Perskaitome ir nukopijuojame originalius 7 baitų žaidimo antraštės duomenis
  char header_buf[7];
  _read(fstr_in, header_buf, 7);
  _write(fstr_out, header_buf, 7);
  _close(fstr_in); // Seno failo mums daugiau nebeprireiks!

  // 2 ŽINGSNIS: Sukame masinį binarinio rašymo ciklą per VISUS 225 (arba daugiau) simbolius iškart
  Memo1->Lines->Add("=== FAILO ĮRAŠYMAS Į DISKĄ PALEISTAS ===");

  // Didelis dinaminis buferis, kad joks milžiniškas šrifto dydis nesulaužytų atminties (120 KB atsargai)
  std::vector<char> write_buf(128000);

  for (int k = 0; k < sr_count; k++) {
	// Pasižymime, kur naujajame faile prasidės šis konkretus simbolis (atnaujiname offsetus ateities paspaudimams)
	sr[k].foffset = _lseek(fstr_out, 0, SEEK_CUR);

	int p = sr[k].plotis;
	int h = sr[k].ilgis;
	int lm = sr[k].lmargin;
	int rm = sr[k].rmargin;

	// Saugiai surašome 4 pagrindinius 32-bit (4 baitų) parametrus į rašymo buferį Little Endian formatu
	write_buf[0] = p & 0xFF;  write_buf[1] = (p >> 8) & 0xFF;  write_buf[2] = (p >> 16) & 0xFF;  write_buf[3] = (p >> 24) & 0xFF;
	write_buf[4] = h & 0xFF;  write_buf[5] = (h >> 8) & 0xFF;  write_buf[6] = (h >> 16) & 0xFF;  write_buf[7] = (h >> 24) & 0xFF;
	write_buf[8] = lm & 0xFF; write_buf[9] = (lm >> 8) & 0xFF; write_buf[10] = (lm >> 16) & 0xFF; write_buf[11] = (lm >> 24) & 0xFF;
	write_buf[12] = rm & 0xFF; write_buf[13] = (rm >> 8) & 0xFF; write_buf[14] = (rm >> 16) & 0xFF; write_buf[15] = (rm >> 24) & 0xFF;

	// Užpildome buferį konkrečios raidės pikseliais iš mūsų trigubo masyvo
	int byte_index = 16; // Pradedame iškart po 16 baitų antraštės
	for (int i = 0; i < h; i++) {
	  for (int j = 0; j < p; j++) {
		// Paimame spalvą iš redaktoriaus šviesaus stiliaus (juoda ant balto)
		unsigned char pikselis = GetRValue(senasp[k][j][i]);

		// Invertuojame atgal žaidimui: redaktoriaus juoda (0) faile privalo virsti žaidimo balta (255)
		write_buf[byte_index] = 255 - pikselis;
		byte_index++;
	  }
	}

	// Žaibiškai vienu ypu išspaudžiame visą sugeneruotą simbolį į kietąjį diską
	_write(fstr_out, write_buf.data(), byte_index);
  }

  _close(fstr_out);

  // Perkrauname ką tik sugeneruotą failą, kad programos vaizdas automatiškai sinchronizuotųsi
  NuskaitytiFaila(Sender);

  Memo1->Lines->Add("Išsaugota sėkmingai! Naujas šrifto failas sugeneruotas be priekaištų.");
  ShowMessage("Šrifto failas sėkmingai atnaujintas kietajame diske!");
}



// PELĖS PIEŠIMAS: Rankinis taisymas ant Image2 tinklelio


void __fastcall TForm1::Image1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	{
  int sk = -1;
  int dd = StrToInt(Edit1->Text);
  if (dd <= 0) dd = 2; // Saugus mastelis

  // Sukame ciklą per visus nuskaitytus simbolius
  for (int i = 0; i < sr_count; i++) {
	// IŠTAISYTA ČIA: Pridedame 4 pikselių nuokrypį, nes Image1 braižymas prasideda nuo px=4 ir py=4
	int r_kaire = sr[i].srx;
	int r_desine = sr[i].srx + (sr[i].plotis * dd) + 4;
	int r_virsus = sr[i].sry;
	int r_apacia = sr[i].sry + (sr[i].ilgis * dd) + 4;

	// Tikriname, ar pelės paspaudimas pataikė į šio simbolio rėmus ekrane
	if (X >= r_kaire && X <= r_desine && Y >= r_virsus && Y <= r_apacia)
	{
	  sk = i;
	  break; // Radome – stabdome paiešką!
	}
  }

  // Jei pelė rado simbolį, akimirksniu jį aktyvuojame
  if (sk != -1) {
	ListBox1->ItemIndex = sk; // Pažymime eilutę sąraše
	rodyk(sk);                // Išdidiname didžiajame tinklelyje
}
	}



}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1Change(TObject *Sender)
{
 if (Edit1->Text.Trim() == "" || StrToIntDef(Edit1->Text, 0) <= 0) return;
  if (sr_count <= 0) return;

  // 1. Švariai išvalome bendrąjį žemėlapį Image1 tamsiai mėlyna fono spalva
  Image1->Canvas->Brush->Color = clNavy;
  Image1->Canvas->Brush->Style = bsSolid;
  Image1->Canvas->FillRect(Image1->ClientRect);

  int px = 4, py = 4, cpy = 1;
  int d = StrToInt(Edit1->Text);

  // Išjungiame Pen rėmelius žaibiškam greičiui be GDI resursų rijimo
  Image1->Canvas->Pen->Style = psClear;

  // 2. Sukame ciklą per visus simbolius ir perpiešiame iš operatyviosios atminties
  for (int k = 0; k < sr_count; k++) {
	int x = sr[k].plotis;
	int y = sr[k].ilgis;

	// Perskaičiuojame vizualias koordinates žemėlapyje
	if ((px + (x * d) + (d * 2)) > Image1->Width) {
	  px = 4;
	  py += cpy + (d * 2);
	  cpy = 1;
	}

	if (cpy < (y * d)) cpy = y * d;

	// Atnaujiname koordinates struktūroje pelės paspaudimui
	sr[k].srx = px;
	sr[k].sry = py;

	// 3. Pikselių braižymo ciklas su automatiniu pasirinkto simbolio invertavimu
	for (int i = 0; i < y; i++) {
	  for (int j = 0; j < x; j++) {
		TColor pikselio_spalva = senasp[k][j][i];

		// 🎯 JŪSŲ NORIMAS INVERTAVIMAS: Jeigu šis simbolis (k) yra pasirinktas (lygus kintamajam "senas"),
		// mes priverstinai apverčiame jo spalvas ekrane!
		// Baltas fonas pavirs juodu kvadratėliu, o juoda raidė nušvis baltai.
		if (k == senas) {
		  if (pikselio_spalva == (TColor)RGB(255, 255, 255)) {
			pikselio_spalva = (TColor)RGB(0, 0, 0);       // Baltas fonas virsta juodu
		  } else if (pikselio_spalva == (TColor)RGB(0, 0, 0)) {
			pikselio_spalva = (TColor)RGB(255, 255, 255); // Juoda raidė virsta balta
		  }
		}

		Image1->Canvas->Brush->Color = pikselio_spalva;
		Image1->Canvas->FillRect(TRect(px + (j * d), py + (i * d), px + (j * d) + d, py + (i * d) + d));
	  }
	}

	px = px + (x * d) + (d * 2);
  }

  Image1->Refresh();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
 if (sr_count <= 0) {
	ShowMessage("Pirmiausia įkelkite originalų žaidimo šrifto failą!");
	return;
  }

  if (FontDialog1->Execute()) {
	// SUGRAŽINAME POSLINKI IŠ LAUKELIO (31), kad raidės pergamente atsistotų į savo tikrąsias vietas!
	int vartotojo_poslinkis = StrToInt(Edit2->Text);

	LOGFONT lf;
	GetObject(FontDialog1->Font->Handle, sizeof(LOGFONT), &lf);

	CHARSETINFO csi;
	UINT codePage = CP_ACP;
	if (TranslateCharsetInfo((DWORD*)(DWORD_PTR)lf.lfCharSet, &csi, TCI_SRCCHARSET)) {
		codePage = csi.ciACP;
	}

	Memo1->Lines->Add("=== MASINIS GENERAVIMAS SU EKSTREMALU TARPU IR POSLINKIU ===");

	std::unique_ptr<TBitmap> bmp(new TBitmap());
	bmp->Canvas->Font->Assign(FontDialog1->Font);
	bmp->Canvas->Font->Color = clWhite;
	bmp->Canvas->Brush->Color = clBlack;

	for (int i = 0; i < sr_count; i++) {
	  unsigned char ansi_kodas = (unsigned char)(i + vartotojo_poslinkis);

	  wchar_t raide;
	  MultiByteToWideChar(codePage, 0, (char*)&ansi_kodas, 1, &raide, 1);
	  String tekstas = String(raide);

	  int ttf_plotis = bmp->Canvas->TextWidth(tekstas);
	  int ttf_ilgis = bmp->Canvas->TextHeight(tekstas);

	  if (ttf_plotis <= 0 || ttf_plotis > 48) ttf_plotis = 12;
	  if (ttf_ilgis <= 0 || ttf_ilgis > 48) ttf_ilgis = 16;

	  bmp->Width = ttf_plotis;
	  bmp->Height = ttf_ilgis;

	  bmp->Canvas->Brush->Color = clBlack;
	  bmp->Canvas->FillRect(TRect(0, 0, ttf_plotis, ttf_ilgis));
	  bmp->Canvas->TextOut(0, 0, tekstas);

	  // Nustatome naujuosius matmenis pagal Tahoma 9 Bold
	  sr[i].plotis = ttf_plotis;
	  sr[i].ilgis = ttf_ilgis;

	  // =========================================================================
	  // GALAUS SAUGIKLIS: EKSTREMALUS 1 PIKSELIO TARPAS (iš jūsų sėkmingo kodo!)
	  // =========================================================================
	  if (i == 0 || i == 1) {
		sr[i].plotis  = 1;
		sr[i].lmargin = 0;
		sr[i].rmargin = 1; // Liepiame žaidimui sekančią raidę stumti vos per 1 pikselį

		bmp->Width = 1;
		bmp->Canvas->Brush->Color = clBlack;
		bmp->Canvas->FillRect(TRect(0, 0, 1, ttf_ilgis));
		ttf_plotis = 1;
	  }
	  // =========================================================================

	  // Perkeliame tikruosius pikselius į trigubą masyvą idealia jūsų rankinio importo tvarka
	  for (int i_c = 0; i_c < ttf_ilgis; i_c++) {
		for (int j = 0; j < ttf_plotis; j++) {
		  TColor spalva = bmp->Canvas->Pixels[j][i_c];
		  unsigned char pilkumas = GetRValue(spalva);
		  senasp[i][j][i_c] = (TColor)RGB(255 - pilkumas, 255 - pilkumas, 255 - pilkumas);
		}
	  }

	  pakeistas_simbolis[i] = true;
	}

	senas = 0;
	rodyk(0);
	Edit1Change(Sender);

	ShowMessage("Masinis šriftas sugeneruotas! Poslinkis suderintas su tarpais. Išsaugokite.");
  }
} // <-- Šis skliaustas uždaro pačią "Button4Click" funkciją!

//---------------------------------------------------------------------------

void __fastcall TForm1::ListBox1Click(TObject *Sender)
{
  if (ListBox1->ItemIndex != -1) {
	rodyk(ListBox1->ItemIndex);
  }
}
//---------------------------------------------------------------------------




