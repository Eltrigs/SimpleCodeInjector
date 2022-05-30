// SimpleCodeInjector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD getProcID(const char* strProcessName)
{
	DWORD dwProcessID = 0;

	//Takes a snapshot of the specified processes, as well as the heaps, modules, 
	//and threads used by these processes.
	//HANDLE is just a pointer to a void type
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);


	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 processEntry;
		processEntry.dwSize = sizeof(processEntry);

		//Process32First retrieves information about the first process 
		//encountered in a system snapshot.
		if (Process32First(hSnapshot, &processEntry))
		{
			do
			{
				//_wcsicmp performs a case-insensitive comparison of strings
				//between const wchar_t *string1, *string2
				if (!_stricmp(processEntry.szExeFile, strProcessName))
				{
					dwProcessID = processEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &processEntry));
		}
	}
	CloseHandle(hSnapshot);
	return dwProcessID;
}

int main()
{
	const char* strDLLPath = "D:\\Programmikood\\AssaultCubeInternal\\Debug\\AssaultCubeInternal.dll";
	const char* strProcessName = "ac_client.exe";
	DWORD dwProcessID = 0;


	//Attempt to find the process ID for strProcessName (the program may not be open yet)
	while (!dwProcessID)
	{
		dwProcessID = getProcID(strProcessName);
		Sleep(30);
	}

	//hProcess is most likely a pointer to the process in kernel space
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessID);

	if (hProcess && hProcess != INVALID_HANDLE_VALUE)
	{
		void* ptrLocation = VirtualAllocEx(hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (ptrLocation)
		{
			WriteProcessMemory(hProcess, ptrLocation, strDLLPath, strlen(strDLLPath) + 1, 0);
		}

		HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, ptrLocation, 0, 0);

		if (hThread)
		{
			CloseHandle(hThread);
		}

		if (hProcess)
		{
			CloseHandle(hProcess);
		}
		return 0;
	}
}
