#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define BUF_SIZE 512

int _tmain(int argc, TCHAR * argv[])
{
	HANDLE hWritePipe;
	HANDLE hReadPipe;

	TCHAR sendString[BUF_SIZE];
	TCHAR recvString[BUF_SIZE + 1];
	DWORD bytes;

	FILE* file = _tfopen(_T("Inherit.txt"), _T("rt"));
#ifdef _WIN64
	_ftscanf(file, _T("%lld %lld"), &hWritePipe, &hReadPipe);
#else
	_ftscanf(file, _T("%d %d"), &hWritePipe, &hReadPipe);
#endif
	fclose(file);

	while (1)
	{
		_tprintf(_T("Send: "));
		_fgetts(sendString, sizeof(sendString) / sizeof(TCHAR), stdin);
		if (lstrlen(sendString) > 0)
			sendString[lstrlen(sendString) / sizeof(TCHAR) - 1] = 0;

		WriteFile(hWritePipe, sendString, lstrlen(sendString) * sizeof(TCHAR), &bytes, NULL);

		if (!_tcscmp(sendString, _T("exit")))
			break;

		_tprintf(_T("Received: "));
		ReadFile(hReadPipe, recvString, BUF_SIZE, &bytes, NULL);
		recvString[bytes / sizeof(TCHAR)] = 0;
		_tprintf(_T("%s\n"), recvString);

		if (!_tcscmp(recvString, _T("exit")))
			break;
	}

	CloseHandle(hWritePipe);
	return 0;
}