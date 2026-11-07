#pragma once
#include <windows.h>

HANDLE hConsole;

void InitConsole()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void MoveCursor(SHORT x, SHORT y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;

	SetConsoleCursorPosition(hConsole, coord);
}

// 알아두면 좋은 콘솔창 크기조절
// system("mode con: cols=205 lines=35");