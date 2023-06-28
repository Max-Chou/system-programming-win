#pragma once

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#ifdef WIN32

#ifdef _STATICLIB
#if defined (__cplusplus)
#define LIBSPEC extern "C"
#else
#define LIBSPEC
#endif 
#endif

#if defined(UTILS_EXPORTS)
#define LIBSPEC __declspec (dllexport)
#elif defined(__cplusplus)
#define LIBSPEC extern "C" __declspec (dllimport)
#else
#define LIBSPEC __declspec (dllimport)
#endif
#endif

/* Limits and constants */
#define EMPTY _T("")
#define YES _T("y")
#define NO _T("n")
#define CR 0x0D
#define LF 0x0A
#define TSIZE sizeof (TCHAR)

#define TYPE_FILE 1
#define TYPE_DIR 2
#define TYPE_DOT 3

#define MAX_OPTIONS 20
#define MAX_ARG 1000
#define MAX_COMMAND_LINE MAX_PATH+50
#define MAX_PATH_LONG 32767
#define MAX_NAME 256

/* commonly used functions. */
LIBSPEC BOOL ConsolePrompt(LPCTSTR, LPTSTR, DWORD, BOOL);
LIBSPEC BOOL PrintStrings(HANDLE, ...);
LIBSPEC BOOL PrintMsg(HANDLE, LPCTSTR);
LIBSPEC VOID ReportError(LPCTSTR, DWORD, BOOL);
LIBSPEC VOID ReportException(LPCTSTR, DWORD);
LIBSPEC DWORD Options(int, LPCTSTR*, LPCTSTR, ...);
LIBSPEC LPTSTR SkipArg(LPTSTR, int, int, LPTSTR argv[]);
//LIBSPEC BOOL WindowsVersionOK(DWORD, DWORD);
LIBSPEC VOID GetArgs(LPCTSTR, int*, LPTSTR*);


#ifdef _UNICODE /* This declaration had to be added. */
#define _tstrrchr wcsrchr
#else
#define _tstrrchr strrchr
#endif

#ifdef _UNICODE /* This declaration had to be added. */
#define _tstrstr wcsstr
#else
#define _tstrstr strstr
#endif

#ifdef _UNICODE /* This declaration had to be added. */
#define _memtchr wmemchr
#else
#define _memtchr memchr
#endif