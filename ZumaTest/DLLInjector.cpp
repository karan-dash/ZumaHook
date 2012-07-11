#include "DLLInjector.h"
#include "windows.h"
#include <Tlhelp32.h>
#include <vector>
#include <string>

bool EnablePrivileges(char* privileges) {
	HANDLE token;
	LUID luid;
	TOKEN_PRIVILEGES tokenPrivileges;
	ZeroMemory(&tokenPrivileges, sizeof(tokenPrivileges));

	if(!OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY), &token)) {
		return false;
	}

	if(!LookupPrivilegeValue(NULL, privileges, &luid)) {
		CloseHandle(token);
		return false;
	}

	tokenPrivileges.PrivilegeCount = 1;
	tokenPrivileges.Privileges[0].Luid = luid;
	tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	bool result = AdjustTokenPrivileges(token, FALSE, &tokenPrivileges, sizeof(tokenPrivileges), NULL, NULL);
	CloseHandle(token);
	return result;
}

HMODULE remoteLoadLibrary(DWORD processId, LPCTSTR libraryName) {
	HANDLE processHandle;
	HMODULE libraryHandle = NULL;

	processHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
		FALSE, processId);
	if (processHandle) {
		HANDLE    threadHandle;
		SIZE_T    bytesToWrite;
		SIZE_T    bytesWritten;
		LPVOID    remoteLibraryName;

		bytesToWrite = (strlen(libraryName)+1)*sizeof(TCHAR);

		remoteLibraryName = VirtualAllocEx(processHandle, NULL, bytesToWrite, MEM_COMMIT, PAGE_READWRITE);
		if (remoteLibraryName) {
			if (WriteProcessMemory(processHandle, remoteLibraryName, libraryName, bytesToWrite, &bytesWritten) && bytesWritten == bytesToWrite) {
				threadHandle = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, remoteLibraryName, 0, NULL);
				if (threadHandle) {
					if (WaitForSingleObject(threadHandle, INFINITE) == WAIT_OBJECT_0) {
						GetExitCodeThread(threadHandle, (LPDWORD)&libraryHandle);
					}
					CloseHandle(threadHandle);
				}
			}
			VirtualFreeEx(processHandle, remoteLibraryName, 0, MEM_RELEASE);
		}	
		CloseHandle(processHandle);
	}
	return libraryHandle;
}

bool Inject(char* dllPath, char* processName) {
	std::vector<std::string> processNames; 
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!EnablePrivileges(SE_DEBUG_NAME)) {
		return false;
	}
	HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL); 
	if(Process32First(hTool32, &pe32)) {
		while(Process32Next(hTool32, &pe32)) {
			processNames.push_back(pe32.szExeFile); 
			if(strcmp(pe32.szExeFile, processName) == 0) {
				char fullPath[MAX_PATH];
				char dirPath[MAX_PATH];
				GetCurrentDirectory(MAX_PATH, dirPath); 
				sprintf_s(fullPath, MAX_PATH, "%s\\%s", dirPath, dllPath); 
				return remoteLoadLibrary(pe32.th32ProcessID, fullPath) != NULL;
				break;
			}
		}
	}
	CloseHandle(hTool32);
	return false;
}
