#include "stdafx.h"
#include "extern.h"
#include "directinput.h"

HINSTANCE hDLL;
extern void InitConfig();
extern VOID FreeDirectInput(int idx);


BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved )
{
	hDLL = hModule;
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);


#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	}

	return TRUE;
}