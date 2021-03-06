#include <windows.h>

#define PROCESSID 5675

BOOL InjectDLL(DWORD dwProcessId, LPCSTR lpszDLLPath)
{
	HANDLE  hProcess, hThread;
	LPVOID  lpBaseAddr, lpFuncAddr;
	DWORD   dwMemSize, dwExitCode;
	BOOL    bSuccess = FALSE;
	HMODULE hUserDLL;

	if((hProcess = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION
		|PROCESS_VM_WRITE|PROCESS_VM_READ, FALSE, dwProcessId)))
	{
		dwMemSize = lstrlen(lpszDLLPath) + 1;
		if((lpBaseAddr = VirtualAllocEx(hProcess, NULL, dwMemSize, MEM_COMMIT, PAGE_READWRITE)))
		{
			if(WriteProcessMemory(hProcess, lpBaseAddr, lpszDLLPath, dwMemSize, NULL))
			{
				if((hUserDLL = LoadLibrary(TEXT("kernel32.dll"))))
				{
					if((lpFuncAddr = GetProcAddress(hUserDLL, TEXT("LoadLibraryA"))))
					{
						if((hThread = CreateRemoteThread(hProcess, NULL, 0, lpFuncAddr, lpBaseAddr, 0, NULL)))
						{
							WaitForSingleObject(hThread, INFINITE);
							if(GetExitCodeThread(hThread, &dwExitCode)) {
								bSuccess = (dwExitCode != 0) ? TRUE : FALSE;
							}
							CloseHandle(hThread);
						}
					}
					FreeLibrary(hUserDLL);
				}
			}
			VirtualFreeEx(hProcess, lpBaseAddr, 0, MEM_RELEASE);
		}
		CloseHandle(hProcess);
	}

	return bSuccess;
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	if(InjectDLL(PROCESSID, "test.dll")) {
		MessageBox(NULL, TEXT("DLL Injected!"), TEXT("DLL Injector"), MB_OK);
	}else {
		MessageBox(NULL, TEXT("Couldn't inject DLL"), TEXT("DLL Injector"), MB_OK | MB_ICONERROR);
	}

	return 0;
}

// updated 08/08/2017, 9:11 AM GMT + 3
