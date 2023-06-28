#include "utils.h"

#define DIRNAME_LEN (MAX_PATH + 2)

int _tmain(int argc, LPTSTR argv[])
{
	TCHAR pwdBuffer[DIRNAME_LEN];
	DWORD lenCurDir;
	lenCurDir = GetCurrentDirectory(DIRNAME_LEN, pwdBuffer);
	if (lenCurDir == 0)
		ReportError(_T("Failure getting pathname."), 1, TRUE);

	if (lenCurDir > DIRNAME_LEN)
		ReportError(_T("Pathname is too long."), 2, FALSE);

	PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), pwdBuffer);
	return 0;
}