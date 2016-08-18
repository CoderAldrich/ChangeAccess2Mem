// ChangeAccess2Mem.cpp : 定义控制台应用程序的入口点。
// Change memory access of any process, use as memory breakpoint

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <shlwapi.h>
#include <stdio.h>


#pragma comment(lib, "shlwapi.lib")

//  Forward declarations:
HANDLE GetProcessHandlebyName( WCHAR *wzProcesssName);
void printError( TCHAR* msg );

void main( )
{
	HANDLE hProcess = 0;
	DWORD dwBaseAddress = 0;
	DWORD dwSize = 0;
	DWORD dwOldProtect = 0;
	WCHAR wzProcessName[MAX_PATH];

	// input process name:
	printf("Input process name: ");
	wscanf_s(L"%ls", wzProcessName, MAX_PATH - 1);
	printf("\n");

	// input memory base address:
	printf("Input memory address to change access: ");
	scanf_s("%x", &dwBaseAddress, 1);
	printf("\n");

	// input memory size:
	printf("Input memory size: ");
	scanf_s("%x", &dwSize, 1);
	printf("\n");

	// Get handle of process
	hProcess = GetProcessHandlebyName(wzProcessName);
	
	DWORD dwNewProtect = PAGE_NOACCESS;
	// set memory Protection to PAGE_NOACCESS
	if (0 == VirtualProtectEx(hProcess, (LPVOID)dwBaseAddress, dwSize, dwNewProtect, &dwOldProtect))
	{
		printError(TEXT("VirtualProtectEx failed!"));
	}
	
	CloseHandle( hProcess);
	system("pause");
}

// Get process HANDLE by name
HANDLE GetProcessHandlebyName( WCHAR *wzProcesssName)
{
	HANDLE hProcessSnap;
	HANDLE hProcess = NULL;
	PROCESSENTRY32 pe32;


	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		printError( TEXT("CreateToolhelp32Snapshot (of processes)") );
		return( FALSE );
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		printError( TEXT("Process32First") ); // show cause of failure
		CloseHandle( hProcessSnap );          // clean the snapshot object
		return( FALSE );
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		if (StrStrIW(pe32.szExeFile, wzProcesssName))
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION, FALSE, pe32.th32ProcessID);
			if (NULL == hProcess)
			{
				printError(TEXT("OpenProcess failed!"));
			}
			break;
		}

	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );
	return hProcess;
}

void printError( TCHAR* msg )
{
	DWORD eNum;
	eNum = GetLastError( );
	// Display the message
	_tprintf( TEXT("\nWARNING: %s failed with error %d \n"), msg, eNum );
}