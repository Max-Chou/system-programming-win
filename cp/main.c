/*
	Copy File with 
	1. C Standard Library
	2. Unix Implementation
	3. WriteFile and ReadFile
	4. Fast 
	5. CopyFile
*/
#ifdef _CStandard
#include <stdio.h>
#include <errno.h>

#define BUF_SIZE 256

int main(int argc, char **argv)
{
	FILE* inFile, * outFile;
	errno_t rc;
	char rec[BUF_SIZE];
	size_t bytesIn, bytesOut;
	if (argc != 3) {
		fprintf(stderr, "Usage: cp file1 file2\n");
		return 1;
	}

	// change to use fopen_s
	rc = fopen_s(&inFile, argv[1], "rb");
	if (rc != 0) {
		perror(argv[1]);
		return 2;
	}

	rc = fopen_s(&outFile, argv[2], "wb");
	if (rc != 0) {
		perror(argv[2]);
		fclose(inFile);
		return 3;
	}

	while ((bytesIn = fread(rec, 1, BUF_SIZE, inFile)) > 0) {
		bytesOut = fwrite(rec, 1, bytesIn, outFile);
		if (bytesOut != bytesIn) {
			perror("Fatal write error.");
			fclose(inFile);
			fclose(outFile);
			return 4;
		}
	}

	fclose(inFile);
	fclose(outFile);
	return 0;
}

#elif defined(_CUNIX)

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#define BUF_SIZE 8192

int main(int argc, char** argv)
{
	int fdIn;
	int fdOut;
	int bytesIn, bytesOut;
	char rec[BUF_SIZE];

	if (argc != 3) {
		fprintf(stderr, "Usage: cp file1 file2\n");
		return 1;
	}

	fdIn = _open(argv[1], O_RDONLY | O_BINARY);
	if (fdIn == -1)
	{
		perror(argv[1]);
		return 2;
	}

	fdOut = _open(argv[2], O_WRONLY | O_CREAT | O_BINARY, 0666);
	if (fdOut == -1)
	{
		perror(argv[2]);
		_close(fdIn);
		return 3;
	}

	while ((bytesIn = _read(fdIn, &rec, BUF_SIZE)) > 0) {
		bytesOut = _write(fdOut, &rec, (unsigned int)bytesIn);
		if (bytesOut != bytesIn) {
			perror("Fatal write error.");
			_close(fdIn);
			_close(fdOut);
			return 4;
		}
	}
	_close(fdIn);
	_close(fdOut);
	return 0;
}


#elif defined(_CW)

#include <Windows.h>
#include <stdio.h>
#define BUF_SIZE 16384

int main(int argc, LPTSTR argv[])
{
	HANDLE hIn, hOut;
	DWORD nIn, nOut;
	CHAR buffer[BUF_SIZE];
	if (argc != 3)
	{
		fprintf(stderr, "Usage: cp file1 file2\n");
		return 1;
	}

	hIn = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Cannot open intput file. Error: %x\n", GetLastError());
		return 2;
	}

	hOut = CreateFile(argv[2], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_REPARSE_POINT, NULL);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Cannot open output file. Error: %x\n", GetLastError());
		CloseHandle(hIn);
		return 3;
	}

	while (ReadFile(hIn, buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) {
		WriteFile(hOut, buffer, nIn, &nOut, NULL);
		if (nIn != nOut) {
			fprintf(stderr, "Fatal write error: %x\n", GetLastError());
			CloseHandle(hIn);
			CloseHandle(hOut);
			return 4;
		}
	}
	CloseHandle(hIn);
	CloseHandle(hOut);

	return 0;
}

#elif defined(_CFast)

#include <Windows.h>
#include <stdio.h>
#define BUF_SIZE 8192

int main(int argc, LPTSTR argv[])
{
	HANDLE hIn, hOut;
	DWORD nIn, nOut;
	CHAR buffer[BUF_SIZE];
	if (argc != 3)
	{
		fprintf(stderr, "Usage: cp file1 file2\n");
		return 1;
	}

	hIn = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Cannot open input file. Error: %x\n", GetLastError());
		return 2;
	}
	hOut = CreateFile(argv[2], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hOut == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Cannot open output file. Error: %x\n", GetLastError());
		CloseHandle(hIn);
		return 3;
	}

	while (ReadFile(hIn, buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) {
		WriteFile(hOut, buffer, nIn, &nOut, NULL);
		if (nIn != nOut) {
			fprintf(stderr, "Fatal write error: %x\n", GetLastError());
			CloseHandle(hIn);
			CloseHandle(hOut);
			return 4;
		}
	}
	CloseHandle(hIn);
	CloseHandle(hOut);
	return 0;
}

#else

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#define BUF_SIZE 256

int _tmain(int argc, TCHAR **argv)
{
	if (argc != 3) {
		_tprintf(stderr, _T("Usage: cp file1 file2\n"));
		return 1;
	}
	if (!CopyFile(argv[1], argv[2], FALSE)) {
		_tprintf(stderr, _T("CopyFile Error: %x\n"), GetLastError());
		return 2;
	}
	return 0;
}


#endif