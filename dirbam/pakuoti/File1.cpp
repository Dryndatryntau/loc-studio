#include <vcl.h>
#include <windows.h>
#pragma hdrstop

// Átraukiame jûsø formos antraðtæ
#include "Unit2.h"

#pragma argsused

// Ði funkcija yra standartinë DLL inicializacija, jà paliekame, kaip sugeneravo C++ Builder
int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    return 1;
}

// ============================================================================
// EKSPOORTUOJAMOS ÁSKIEPIO FUNKCIJOS
// ============================================================================

// 1. Funkcija, kuri gràþina áskiepio pavadinimà meniu sàraðui
// extern "C" ir __stdcall garantuoja, kad pavadinimas iðliks ðvarus ("GetPluginName")
extern "C" __declspec(dllexport) const char* __stdcall GetPluginName()
{
    return "Text import/export";
}

// 2. Funkcija, kurià paspaudus meniu, atidarys jûsø formà
extern "C" __declspec(dllexport) void __stdcall RunPlugin()
{
    // Sukuriame formà atmintyje (TForm2 paimta ið Unit2.h)
    TForm2 *PluginForm = new TForm2(Application);
    try
    {
        // Parodome formà kaip modaliná (pagrindiná) langà virð Unpacker'io
        PluginForm->ShowModal();
    }
    __finally
    {
        // Vartotojui uþdarius langà, saugiai iðvalome atmintá
        delete PluginForm;
    }
}

