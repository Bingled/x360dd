#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif

#ifndef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WINXP
#endif

#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800

// Windows Header Files:
#include <windows.h>
#include <math.h>

#include <time.h>
#include <fstream>

#include <dinput.h>
#include <xinput.h>

#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#endif



