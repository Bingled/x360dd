#include "stdafx.h"
#include "utils.h"
#include "extern.h"
#include "directinput.h"
#include <windows.h>
#include <windowsx.h>

bool didinit = false;
bool hhkm_ext = false;
HWND hWin = NULL;
DWORD PID = NULL;
int currentUser = -1;

extern void Initialize(DWORD idx);
extern HRESULT InitDirectInput( HWND hook, int idx, int failsafe );
extern HINSTANCE hDLL;
HHOOK hhk;
HHOOK hhkm;
int mouseposx = -1;
int mouseposy = -1;
RECT rcWind;
int middlex;
int middley;

int storex = 0;
int storey = 0;

bool lmousedown = false;
bool rmousedown = false;
bool mmousedown = false;



LRESULT CALLBACK wireKeyboardProc(int code,WPARAM wParam,LPARAM lParam)
{
	if(code == HC_ACTION) {
		DWORD vkCode = (DWORD) wParam;
		int oldUser = currentUser;
		
		if (GetForegroundWindow() != hWin || vkCode == VK_MENU) {
			return CallNextHookEx(hhk,code,wParam,lParam);
		}

        switch (vkCode) {
			case VK_F2: currentUser = 0;  break;
			case VK_F3: currentUser = 1;  break;
			case VK_F4: currentUser = 2;  break;
        }

		if (currentUser == -1)
			return CallNextHookEx(hhk,code,wParam,lParam);
		else
			if (vkCode == VK_F1) currentUser = -1;


		if (oldUser != currentUser) {
			GetWindowRect(hWin, &rcWind);
			middlex = rcWind.left+((rcWind.right-rcWind.left)/2);
			middley = rcWind.top+((rcWind.bottom-rcWind.top)/2);

			if (currentUser == -1) {
				SetCursorPos(storex, storey);
			} else {
				SetCursorPos(middlex, middley);
			}
		}
		return 1;
	}
	return CallNextHookEx(hhk,code,wParam,lParam);
}

LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {

	if (GetForegroundWindow() != hWin) return CallNextHookEx(hhkm, nCode, wParam, lParam);

	PMSLLHOOKSTRUCT p = reinterpret_cast<PMSLLHOOKSTRUCT>( lParam );

	if(currentUser != -1) {
		switch (wParam) {
			case WM_MOUSEMOVE:
				mouseposx = middlex - p->pt.x;
				mouseposy = middley - p->pt.y;
				break;
			case WM_LBUTTONDOWN: lmousedown = true;  break;
			case WM_LBUTTONUP:   lmousedown = false; break;
			case WM_MBUTTONDOWN: mmousedown = true;  break;
			case WM_MBUTTONUP:   mmousedown = false; break;
			case WM_RBUTTONDOWN: rmousedown = true;  break;
			case WM_RBUTTONUP:   rmousedown = false; break;
		}
		return 1;
	} else {
		UnhookWindowsHookEx(hhkm);
		hhkm_ext = false;
	}

	storex = p->pt.x;
	storey = p->pt.y;

	return CallNextHookEx(hhkm, nCode, wParam, lParam);
}

BOOL CALLBACK findWindowByPID(HWND hwnd, LPARAM targetPID) {
	DWORD wndPID;
	GetWindowThreadProcessId (hwnd, &wndPID);
	if (wndPID == targetPID) {
		hWin = hwnd;
		return false;
	}
	return true;
}

inline bool Initialize() 
{
	if (hWin == NULL) EnumWindows(findWindowByPID,GetCurrentProcessId());
	if (hDLL != NULL) {
		hhk = SetWindowsHookEx(WH_KEYBOARD, wireKeyboardProc, hDLL, NULL);
		hhkm = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, hDLL, NULL);
		hhkm_ext = true;
	}
	GetWindowRect(hWin, &rcWind);
	middlex = rcWind.left+((rcWind.right-rcWind.left)/2);
	middley = rcWind.top+((rcWind.bottom-rcWind.top)/2);
	return !FAILED( InitDirectInput(hWin) );
}


inline bool KeyDown(char* buffer, int key) {
	if (key < 0) return false;
	return buffer[key] & 0x80;
}

DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	if (!didinit) {
		didinit = Initialize();
	}

	if (currentUser != -1 && hDLL != NULL && !hhkm_ext) {
		hhkm = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, hDLL, NULL);
		hhkm_ext = true;
	}

	if (dwUserIndex > 2) return ERROR_DEVICE_NOT_CONNECTED;

	XINPUT_GAMEPAD &Gamepad = pState->Gamepad;
	Gamepad.wButtons = 0;
	Gamepad.bLeftTrigger = 0;
	Gamepad.bRightTrigger = 0;
	Gamepad.sThumbLX = 0;
	Gamepad.sThumbLY = 0;
	Gamepad.sThumbRX = 0;
	Gamepad.sThumbRY = 0;

	// Timestamp packet
	pState->dwPacketNumber=GetTickCount();

	// Poll data from device
	char diks[256];
	ZeroMemory( diks, sizeof(diks) );

	if (!PollKeyboard(diks)) return ERROR_SUCCESS;

	if (currentUser == dwUserIndex) {
		SetCursorPos(middlex, middley);

		if (KeyDown(diks,DIK_W)) Gamepad.sThumbLY = 32767;
		if (KeyDown(diks,DIK_S)) Gamepad.sThumbLY = -32767;
		if (KeyDown(diks,DIK_A)) Gamepad.sThumbLX = -32767;
		if (KeyDown(diks,DIK_D)) Gamepad.sThumbLX = 32767;
		if (KeyDown(diks,DIK_E)) Gamepad.wButtons |= XINPUT_GAMEPAD_X;
		if (KeyDown(diks,DIK_F)) Gamepad.wButtons |= XINPUT_GAMEPAD_Y;
		if (KeyDown(diks,DIK_C)) Gamepad.wButtons |= XINPUT_GAMEPAD_B;
		if (KeyDown(diks,DIK_SPACE)) Gamepad.wButtons |= XINPUT_GAMEPAD_A;
		if (KeyDown(diks,DIK_LCONTROL)) Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_SHOULDER;
		if (KeyDown(diks,DIK_ESCAPE)) Gamepad.wButtons |= XINPUT_GAMEPAD_START;
		if (KeyDown(diks,DIK_O)) Gamepad.wButtons |= XINPUT_GAMEPAD_START;
		if (KeyDown(diks,DIK_1)) Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
		if (KeyDown(diks,DIK_2)) Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
		if (KeyDown(diks,DIK_3)) Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
		if (KeyDown(diks,DIK_4)) Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;

		if (KeyDown(diks,DIK_LSHIFT)) {
			Gamepad.sThumbLX = clamp((-mouseposx)*1500,-32767, 32767);
			Gamepad.sThumbLY = clamp(mouseposy*1500, -32767, 32767);
		} else {
			Gamepad.sThumbRX = clamp((-mouseposx)*1500,-32767, 32767);
			Gamepad.sThumbRY = clamp(mouseposy*1500, -32767, 32767);
		}

		if (mmousedown) Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_SHOULDER;
		if (lmousedown) Gamepad.bRightTrigger = 255;
		if (rmousedown) Gamepad.bLeftTrigger = 255;
	}

	return ERROR_SUCCESS;
}

DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	if (dwUserIndex > 2) return ERROR_DEVICE_NOT_CONNECTED;
	return ERROR_SUCCESS;
}

DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{

	if (dwUserIndex > 2) return ERROR_DEVICE_NOT_CONNECTED;
	XINPUT_GAMEPAD xGamepad;
	xGamepad.bLeftTrigger = (BYTE)0xFF;
	xGamepad.bRightTrigger = (BYTE)0xFF;
	xGamepad.sThumbLX = (SHORT)0xFFFF;
	xGamepad.sThumbLY = (SHORT)0xFFFF;
	xGamepad.sThumbRX = (SHORT)0xFFFF;
	xGamepad.sThumbRY = (SHORT)0xFFFF;
	xGamepad.wButtons = (WORD)0xFFFFFFFF;

	XINPUT_VIBRATION Vibration = {NULL};

	pCapabilities->Flags = (WORD)XINPUT_FLAG_GAMEPAD;
	pCapabilities->SubType=(BYTE)XINPUT_DEVSUBTYPE_GAMEPAD;
	pCapabilities->Gamepad = xGamepad;
	pCapabilities->Vibration = Vibration;
	pCapabilities->Type = (BYTE)XINPUT_DEVTYPE_GAMEPAD;

	return ERROR_SUCCESS;
}

void WINAPI XInputEnable(BOOL enable)
{
}

DWORD WINAPI XInputGetDSoundAudioDeviceGuids
	(
	DWORD dwUserIndex,          // [in] Index of the gamer associated with the device
	GUID* pDSoundRenderGuid,    // [out] DSound device ID for render
	GUID* pDSoundCaptureGuid    // [out] DSound device ID for capture
	)
{
	if (dwUserIndex > 2) return ERROR_DEVICE_NOT_CONNECTED;
	pDSoundCaptureGuid = pDSoundRenderGuid = NULL;  // FIX: Should be GUID_NULL
	return ERROR_SUCCESS;
}

DWORD WINAPI XInputGetBatteryInformation
	(
	DWORD                       dwUserIndex,        // [in]  Index of the gamer associated with the device
	BYTE                        devType,            // [in]  Which device on this user index
	XINPUT_BATTERY_INFORMATION* pBatteryInformation // [out] Contains the level and types of batteries
	)

{

	if (dwUserIndex > 2) return ERROR_DEVICE_NOT_CONNECTED;
	pBatteryInformation->BatteryType = BATTERY_TYPE_WIRED;
	return ERROR_SUCCESS;

}

DWORD WINAPI XInputGetKeystroke
	(
	DWORD dwUserIndex,              // [in]  Index of the gamer associated with the device
	DWORD dwReserved,               // [in]  Reserved for future use
	PXINPUT_KEYSTROKE pKeystroke    // [out] Pointer to an XINPUT_KEYSTROKE structure that receives an input event.
	)
{

	if (dwUserIndex > 2) return ERROR_DEVICE_NOT_CONNECTED;
	return ERROR_EMPTY;
}
