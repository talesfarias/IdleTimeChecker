// #-Idle Time Checker-#
// Developed by Tales Farias
// The purpose of this code is to monitor and output the total time that the current session has been idle. 
// It also counts the total of resets that have been done since the start of the application.
// This is part of my learning path into c++, so don't expect this to be bugfree :)
//
// #-READ-#
// THIS CODE AND ANY ASSOCIATED INFORMATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
// THE ENTIRE RISK OF USE, INABILITY TO USE, OR RESULTS FROM THE USE OF THIS CODE REMAINS WITH THE USER.

#include "pch.h"
#include <iostream>
#include <windows.h>
#include <iomanip> 

DWORD GetLastInputTime(void)
{
	//Retrieves the number of milliseconds that have elapsed since the system was started, up to 49.7 days. 
	//.https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-taglastinputinfo
	LASTINPUTINFO lastInput;
	lastInput.cbSize = sizeof(LASTINPUTINFO);
	BOOL bResults = GetLastInputInfo(&lastInput);
	if (!bResults)
	{
		DWORD dwError = GetLastError();
		// throw exception
		std::cout << dwError;
	}
	DWORD lastInputTime = lastInput.dwTime;
	return lastInputTime;
}

DWORD GetIdleTime(void)
{
	//.https://docs.microsoft.com/en-us/windows/desktop/api/sysinfoapi/nf-sysinfoapi-gettickcount
	DWORD dwTotalTime = GetTickCount();
	DWORD dwLastInputTime = GetLastInputTime();
	//total idle time = (total time the system is up)-(the last input)
	DWORD dwIdleTime = dwTotalTime - dwLastInputTime;
	return dwIdleTime;
}

void ClearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

int main()
{
	SetConsoleTitle(TEXT("Idle Time Checker"));
	// Get the console window to resize it
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	//Setting size to 300x140
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 300, 140, TRUE);

	//Initiating variables
	int ResetCount = 0;
	int IdleSec = 0;
	int IdleMin = 0;
	int IdleHour = 0;
	//Infinite loop
	for (;;) {
		DWORD dwLastInput = GetLastInputTime();
		DWORD dwIdleTime = GetIdleTime();
		// making IdleTime in seconds as it comes in mileseconds
		IdleSec = (dwIdleTime / 1000);
		if (IdleSec == 0)
		{
			ResetCount++;
			IdleMin = 0;
			IdleHour = 0;
		}
		if ((IdleSec / 60) > 0)
		{
			IdleMin = IdleSec / 60;
			IdleSec = IdleSec % 60;
		}
		ClearScreen();
		//making the output in the console
		std::cout << " _________________________________\n";
		std::cout << "|                                 |\n";
		std::cout << "|     Idle time: " << std::setfill('0') << std::setw(2) << IdleHour << ":";
		std::cout << std::setfill('0') << std::setw(2) << IdleMin << ":";
		std::cout << std::setfill('0') << std::setw(2) << IdleSec << std::setfill(' ') << std::setw(11) << "|\n";
		std::cout << "|        Resets: " << std::setfill('0') << std::setw(2) << ResetCount << std::setfill(' ') << std::setw(17) << "|\n";
		std::cout << "|_________________________________|\n";
		//waiting for 1sec before next iteration
		Sleep(1000);
	}
}

