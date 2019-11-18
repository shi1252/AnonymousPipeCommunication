#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define BUF_SIZE 512

int _tmain(int argc, TCHAR * argv[])
{
	HANDLE hReadPipe, hWritePipe;
	HANDLE hReadPipe2, hWritePipe2;

	TCHAR sendString[BUF_SIZE];
	TCHAR recvString[BUF_SIZE + 1];

	DWORD bytes;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	CreatePipe(&hReadPipe2, &hWritePipe2, &sa, 0);

	FILE* file = _tfopen(_T("Inherit.txt"), _T("wt"));
#ifdef _WIN64
	_ftprintf(file, _T("%lld %lld"), hWritePipe, hReadPipe2);
#else
	_ftprintf(file, _T("%d %d"), hWritePipe, hReadPipe2);
#endif
	fclose(file);

	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi;
	TCHAR command[] = _T("AnonymousPipeChild.exe");

	si.cb = sizeof(si);

	CreateProcess(NULL, command, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	CloseHandle(pi.hThread);

	while (1)
	{
		_tprintf(_T("Received: "));
		ReadFile(hReadPipe, recvString, BUF_SIZE, &bytes, NULL);
		recvString[bytes / sizeof(TCHAR)] = 0;
		_tprintf(_T("%s\n"), recvString);

		if (!_tcscmp(recvString, _T("exit")))
			break;

		_tprintf(_T("Send: "));
		_fgetts(sendString, sizeof(sendString) / sizeof(TCHAR), stdin);
		if (lstrlen(sendString) > 0)
			sendString[lstrlen(sendString) / sizeof(TCHAR) - 1] = 0;

		WriteFile(hWritePipe2, sendString, lstrlen(sendString) * sizeof(TCHAR), &bytes, NULL);

		if (!_tcscmp(sendString, _T("exit")))
			break;
	}

	CloseHandle(hReadPipe);
	CloseHandle(hWritePipe);
	CloseHandle(pi.hProcess);
	return 0;
}