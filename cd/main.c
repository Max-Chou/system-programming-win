#include "utils.h"

#define DIRNAME_LEN MAX_PATH + 2

int _tmain(int argc, LPTSTR argv[])
{
	if (!SetCurrentDirectory(argv[1]))
		ReportError(_T("SetCurrentDirectory error."), 1, TRUE);
	return 0;
}
