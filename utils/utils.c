#include "utils.h"

/* Process a string to turn it into a command line. */
void GetArgs(LPCTSTR Command, int* pArgc, LPTSTR argstr[])
{
	int i, icm = 0;
	DWORD ic = 0;

	for (i = 0; ic < _tcslen(Command); i++) {
		while (ic < _tcslen(Command) && Command[ic] != _T(' ') && Command[ic] != _T('\t')) {
			argstr[i][icm] = Command[ic];
			ic++;
			icm++;
		}
		argstr[i][icm] = _T('\0');
		while (ic < _tcslen(Command) && (Command[ic] == _T(' ') || Command[ic] == _T('\t')))
			ic++;
		icm = 0;
	}

	if (pArgc != NULL) *pArgc = i;
	return;
}

/* argv is the command line.
	The options, if any, start with a '-' in argv[1], argv[2], ...
	OptStr is a text string containing all possible options,
	in one-to-one correspondence with the addresses of Boolean variables
	in the variable argument list (...).
	These flags are set if and only if the corresponding option
	character occurs in argv[1], argv[2], ...
	The return value is the argv index of the first argument beyond the option.
*/
DWORD Options(int argc, LPCTSTR argv[], LPCTSTR OptStr, ...)
{
	va_list pFlagList;
	LPBOOL pFlag;
	int iFlag = 0, iArg;

	va_start(pFlagList, OptStr);
	while ((pFlag = va_arg(pFlagList, LPBOOL)) != NULL && iFlag < (int)_tcslen(OptStr)) {
		*pFlag = FALSE;
		for (iArg = 1; !(*pFlag) && iArg < argc && argv[iArg][0] == _T('-'); iArg++)
			*pFlag = _memtchr(argv[iArg], OptStr[iFlag], _tcslen(argv[iArg])) != NULL;
		iFlag++;
	}
	va_end(pFlagList);

	for (iArg = 1; iArg < argc && argv[iArg][0] == _T('-'); iArg++);

	return iArg;
}

/*
	argc, argv[] are the normal argument count and value array taken from a command line
	argn: The argument number that you want to position to
	cLine: The current command line

	Returns: A pointer within cLine, pointing to the first, non-white space, character in the argument
			NULL in case of error.
*/
LPTSTR SkipArg(LPTSTR cLine, int argn, int argc, LPTSTR argv[])
{
	LPTSTR pArg = cLine, cEnd = pArg + _tcslen(cLine);
	int iArg;

	if (argn >= argc) return NULL;

	for (iArg = 0; iArg < argn && pArg < cEnd; iArg++)
	{
		if ('"' == *pArg)
		{
			pArg += _tcslen(argv[iArg]) + 2; /* Skip over the argument and the enclosing quotes */
		}
		else
		{
			pArg += _tcslen(argv[iArg]); /* Skip over the argument */
		}

		while ((pArg < cEnd) && ((_T(' ') == *pArg) || (_T('\t')) == *pArg)) pArg++;
	}

	if (pArg >= cEnd) return NULL;
	return pArg;
}

/* Write the messages to the output handle. Frequently hOut
	will be standard out or error, but this is not required.
	Use WriteConsole (to handle Unicode) first, as the
	output will normally be the console. If that fails, use WriteFile.

	hOut: Handle for output file.
	... : Variable argument list containing TCHAR strings.
		The list must be terminated with NULL.
*/
BOOL PrintStrings(HANDLE hOut, ...)
{
	DWORD msgLen, count;
	LPCTSTR pMsg;
	va_list pMsgList;
	va_start(pMsgList, hOut);
	while ((pMsg = va_arg(pMsgList, LPCTSTR)) != NULL) {
		msgLen = lstrlen(pMsg);
		if (!WriteConsole(hOut, pMsg, msgLen, &count, NULL) && !WriteFile(hOut, pMsg, msgLen * sizeof(TCHAR), &count, NULL)) {
			va_end(pMsgList);
			return FALSE;
		}
	}
	va_end(pMsgList);
	return TRUE;
}

/* For convenience only - Single message version of PrintStrings so that
	you do not have to remember the NULL arg list terminator.

	hOut:   Handle of output file
	pMsg:   Single message to output.
*/
BOOL PrintMsg(HANDLE hOut, LPCTSTR pMsg)
{
	return PrintStrings(hOut, pMsg, NULL);
}

/* Prompt the user at the console and get a response
	which can be up to maxChar generic characters.

	pPromptMessage:		Message displayed to user.
	pResponse:			Programmer-supplied buffer that receives the response.
	maxChar:			Maximum size of the user buffer, measured as generic characters.
	echo:				Do not display the user's response if this flag is FALSE. 
*/
BOOL ConsolePrompt(LPCTSTR pPromptMsg, LPTSTR pResponse, DWORD maxChar, BOOL echo)
{
	HANDLE hIn, hOut;
	DWORD charIn, echoFlag;
	BOOL success;
	hIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	hOut = CreateFile(_T("CONOUT$"), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	echoFlag = echo ? ENABLE_ECHO_INPUT : 0;

	success = SetConsoleMode(hIn, ENABLE_LINE_INPUT | echoFlag | ENABLE_PROCESSED_INPUT)
		&& SetConsoleMode(hOut, ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_PROCESSED_OUTPUT)
		&& PrintStrings(hOut, pPromptMsg, NULL)
		&& ReadConsole(hIn, pResponse, maxChar - 2, &charIn, NULL);

	/* Replace the CR-LF by the null character. */
	if (success)
		pResponse[charIn - 2] = _T('\0');
	else
		ReportError(_T("ConsolePrompt failure."), 0, TRUE);

	CloseHandle(hIn);
	CloseHandle(hOut);

	return success;
}

/*
	General-purpose function for reporting system errors.
	Obtain the error number and convert it to the system error message.
	Display this information and the user-specified message to the standard error device.
	userMessage:				Message to be displayed to standard error device.
	exitCode:					0 - Return.
								> 0 - ExitProcess with this code.

	printErrorMessage:			Display the last system error message if this flag is set.
*/
void ReportError(LPCTSTR userMessage, DWORD exitCode, BOOL printErrorMessage)
{
	DWORD eMsgLen, errNum = GetLastError();
	LPTSTR lpvSysMsg;
	_ftprintf(stderr, _T("%s\n"), userMessage);
	if (printErrorMessage) {
		eMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errNum, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpvSysMsg, 0, NULL);
		if (eMsgLen > 0)
		{
			_ftprintf(stderr, _T("%s\n"), lpvSysMsg);
		}
		else
		{
			_ftprintf(stderr, _T("Last Error Number; %d.\n"), errNum);
		}

		if (lpvSysMsg != NULL) LocalFree(lpvSysMsg);

	}

	if (exitCode > 0)
		ExitProcess(exitCode);

	return;
}