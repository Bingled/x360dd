#include "stdafx.h"
#include "extern.h"
#include "directinput.h"
#include "utils.h"

LPDIRECTINPUT8	g_pDI = NULL ;
LPDIRECTINPUTDEVICE8	g_lpDIKeyboard = NULL; 

HWND hHookWnd = NULL;

VOID FreeDirectInput()
{
	if (g_lpDIKeyboard) {
		g_lpDIKeyboard->Unacquire();
		g_lpDIKeyboard->Release();
		g_lpDIKeyboard = NULL;
	}
	if ( g_pDI ) {
		g_pDI->Release();
		g_pDI = NULL;
	}
}

bool PollKeyboard( char * buffer)
{
    HRESULT  hr; 
    hr = g_lpDIKeyboard->GetDeviceState(256,buffer); 
	
    if FAILED(hr) 
    {

		while ( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			hr = g_lpDIKeyboard->Acquire();

		return false;
    }
	return true;
}

HRESULT InitDirectInput( HWND hDlg )
{
    HRESULT hr;
	DWORD mode = DISCL_FOREGROUND;
  
    FreeDirectInput();

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (VOID**)&g_pDI, NULL ) ) )
	{
		return hr;
	}
        

	//Create keyboard
	if( FAILED( hr = g_pDI->CreateDevice(GUID_SysKeyboard, &g_lpDIKeyboard, NULL) ) )
	{
		return hr;
	}
	
	// Set keyboard data format
	if( FAILED( hr = g_lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard) ) )
	{
		return hr;
	}

	if( FAILED( hr = g_lpDIKeyboard->SetCooperativeLevel(hDlg, mode | DISCL_NONEXCLUSIVE) ) )
	{
		return hr;
	}

	if (g_lpDIKeyboard) g_lpDIKeyboard->Acquire();

	return S_OK;
}
