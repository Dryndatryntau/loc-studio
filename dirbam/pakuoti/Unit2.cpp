#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>

#pragma package(smart_init)
#pragma resource "*.dfm"

TForm2 *Form2;

__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner)
{
}

// 🚀 MYGTUKAS 1: LENTELIŲ EKSPORTAS ( Tik verčiamas tekstas )
void __fastcall TForm2::Button1Click(TObject *Sender)
{
  if (!OpenDialog1->Execute()) return;

  SaveDialog1->FileName = "isvestas_tekstas.txt";
  if (!SaveDialog1->Execute()) return;

  int f_in = _wopen(OpenDialog1->FileName.c_str(), _O_RDONLY | _O_BINARY);
  if (f_in == -1) {
	ShowMessage("Klaida atidarant žaidimo binarinį failą!");
	return;
  }

  int f_out = _wopen(SaveDialog1->FileName.c_str(),
	_O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);

  if (f_out == -1) {
	_close(f_in);
	ShowMessage("Klaida sukuriant išvesties tekstinį failą!");
	return;
  }

  unsigned char bom[] = {0xEF, 0xBB, 0xBF};
  _write(f_out, bom, 3);

  int bendras_eiluciu_kiekis = 0;
  _read(f_in, &bendras_eiluciu_kiekis, 4);

  int eksportuota_zodziu = 0;

  for (int r = 0; r < bendras_eiluciu_kiekis; r++) {
	unsigned short int stulpeliu_kiekis = 0;
	if (_read(f_in, &stulpeliu_kiekis, 2) <= 0) break;

	for (int c = 0; c < stulpeliu_kiekis; c++) {
	  unsigned short int zodzio_ilgis = 0;
	  _read(f_in, &zodzio_ilgis, 2);

	  std::vector<char> zodzio_buf(zodzio_ilgis + 1, 0);
	  if (zodzio_ilgis > 0) {
		_read(f_in, zodzio_buf.data(), zodzio_ilgis);
	  }

	  int w_len = MultiByteToWideChar(1257, 0, zodzio_buf.data(), -1, NULL, 0);
	  std::vector<wchar_t> w_zodis(w_len);
	  MultiByteToWideChar(1257, 0, zodzio_buf.data(), -1, w_zodis.data(), w_len);
	  String tikrasis_tekstas = String(w_zodis.data());

	  if (tikrasis_tekstas.Trim() == "") continue;

	  if (c == 0 && tikrasis_tekstas.Length() > 0) {
		wchar_t pirmas_char = tikrasis_tekstas[1];
		if (pirmas_char >= 'a' && pirmas_char <= 'z') {
		  continue;
		}
	  }

	  String isvestis = "L_" + IntToStr(r) + "_" + IntToStr(c) + "=" + tikrasis_tekstas + "\r\n";

	  UTF8String utf8_isvestis = UTF8String(isvestis);
	  _write(f_out, utf8_isvestis.c_str(), utf8_isvestis.Length());
	  eksportuota_zodziu++;
	}
  }

  _close(f_in);
  _close(f_out);

  ShowMessage(String().sprintf(L"Sėkmė! Eksportuota %d tik vertimui skirtų eilučių.", eksportuota_zodziu));
}

// 🚀 MYGTUKAS 2: LENTELIŲ IMPORTAS ( Universalus koduotės šliuzas )
void __fastcall TForm2::Button2Click(TObject *Sender)
{
  OpenDialog1->Title = "Pasirinkite išverstą TXT failą";
  if (!OpenDialog1->Execute()) return;
  String txt_failas = OpenDialog1->FileName;

  OpenDialog1->Title = "Pasirinkite ORIGINALŲ žaidimo failą (.bak)";
  if (!OpenDialog1->Execute()) return;
  String orig_failas = OpenDialog1->FileName;

  String naujas_failas = orig_failas + ".new";
  int f_out = _wopen(naujas_failas.c_str(),
	_O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);

  if (f_out == -1) {
	ShowMessage("Klaida sukuriant naują binarinį failą!");
	return;
  }

  int f_orig = _wopen(orig_failas.c_str(), _O_RDONLY | _O_BINARY);
  if (f_orig == -1) {
	_close(f_out);
	ShowMessage("Klaida atidarant originalą!");
	return;
  }
  long orig_dydis = _lseek(f_orig, 0, SEEK_END);
  _lseek(f_orig, 0, SEEK_SET);

  std::vector<char> orig_buf(orig_dydis);
  _read(f_orig, orig_buf.data(), orig_dydis);
  _close(f_orig);

  int f_txt = _wopen(txt_failas.c_str(), _O_RDONLY | _O_BINARY);
  _lseek(f_txt, 0, SEEK_SET);

  char bom, bom2, bom3;
  _read(f_txt, &bom, 1); _read(f_txt, &bom2, 1); _read(f_txt, &bom3, 1);
  if (!((unsigned char)bom == 0xEF && (unsigned char)bom2 == 0xBB && (unsigned char)bom3 == 0xBF)) {
	_lseek(f_txt, 0, SEEK_SET);
  }

  std::vector<UnicodeString> txt_eilutes;
  std::vector<char> eil_buf;
  char b;
  while (_read(f_txt, &b, 1) > 0) {
	if (b != '\n' && b != '\r') {
	  eil_buf.push_back(b);
	  continue;
	}
	if (b == '\r') continue;
	eil_buf.push_back(0);

	int w_len = MultiByteToWideChar(CP_UTF8, 0, eil_buf.data(), -1, NULL, 0);
	std::vector<wchar_t> w_buf(w_len);
	MultiByteToWideChar(CP_UTF8, 0, eil_buf.data(), -1, w_buf.data(), w_len);

	txt_eilutes.push_back(UnicodeString(w_buf.data()));
	eil_buf.clear();
  }
  _close(f_txt);

  long dabartinis_pos_orig = 0;
  _write(f_out, &orig_buf[dabartinis_pos_orig], 4);

  int bendras_eiluciu_kiekis = *(int*)&orig_buf[dabartinis_pos_orig];
  dabartinis_pos_orig += 4;

  int pakeista = 0;

  for (int r = 0; r < bendras_eiluciu_kiekis; r++) {
	if (dabartinis_pos_orig >= orig_dydis) break;

	_write(f_out, &orig_buf[dabartinis_pos_orig], 2);
	unsigned short int stulpeliu_kiekis = *(unsigned short int*)&orig_buf[dabartinis_pos_orig];
	dabartinis_pos_orig += 2;

	for (int c = 0; c < stulpeliu_kiekis; c++) {
	  unsigned short int senas_ilgis = *(unsigned short int*)&orig_buf[dabartinis_pos_orig];
	  dabartinis_pos_orig += 2;

	  UnicodeString ieskomas_raktas = "L_" + IntToStr(r) + "_" + IntToStr(c) + "=";
	  UnicodeString grynasis_vertimas = "";
	  bool rasta_txt = false;

	  for (size_t k = 0; k < txt_eilutes.size(); k++) {
		// 🎯 SUTAISYTA ČIA: Pakeista iš Java funkcijų į grynus C++ Embarcadero metodus!
		if (txt_eilutes[k].Pos(ieskomas_raktas) == 1) {
		  grynasis_vertimas = txt_eilutes[k].SubString(ieskomas_raktas.Length() + 1, txt_eilutes[k].Length() - ieskomas_raktas.Length());
		  rasta_txt = true;
		  break;
		}
	  }

	  if (!rasta_txt) {
		_write(f_out, &senas_ilgis, 2);
		if (senas_ilgis > 0) {
		  _write(f_out, &orig_buf[dabartinis_pos_orig], senas_ilgis);
		  dabartinis_pos_orig += senas_ilgis;
		}
		continue;
	  }

	  // 🎯 UNIVERSALUS KODAVIMO ŠLIUZAS (Sutvarko ir didžiąją Ž, ir bet kurį pasaulio simbolį!)
	  int b_len = WideCharToMultiByte(1257, 0, grynasis_vertimas.c_str(), -1, NULL, 0, NULL, NULL);
	  std::vector<char> gamyklinis_baltic_buf(b_len);
	  WideCharToMultiByte(1257, 0, grynasis_vertimas.c_str(), -1, gamyklinis_baltic_buf.data(), b_len, NULL, NULL);

	  unsigned short int naujas_ilgis = (b_len > 0) ? (b_len - 1) : 0;

	  _write(f_out, &naujas_ilgis, 2);
	  if (naujas_ilgis > 0) {
		_write(f_out, gamyklinis_baltic_buf.data(), naujas_ilgis);
	  }

	  dabartinis_pos_orig += senas_ilgis;
	  pakeista++;
	}
  }

  long likutis = orig_dydis - dabartinis_pos_orig;
  if (likutis > 0 && dabartinis_pos_orig < orig_dydis) {
	_write(f_out, &orig_buf[dabartinis_pos_orig], likutis);
  }

  _close(f_out);
  ShowMessage(String().sprintf(L"Sėkmė! Universalus lentelių importas baigtas. Sėkmingai suleisti %d žodžiai.", pakeista));
}

