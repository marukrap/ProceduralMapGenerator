#include "WindowsHelper.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

void centerWindow(sf::WindowHandle handle)
{
#ifdef _WIN32
	HWND hWnd = handle;

	RECT rect;
	GetClientRect(hWnd, &rect);

	LONG style = GetWindowLong(hWnd, GWL_STYLE);

	// Center the window
	int left = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
	int top = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;

	if (!(style & WS_POPUP)) // style != sf::Style::None
	{
		// NOTE: SFML centers sf::Window by default, but it is not exactly centered.

		// SM_CXSIZEFRAME, SM_CYSIZEFRAME - the thickness of the sizing border around a resizable window
		// SM_CXPADDEDBORDER - the amount of border padding for captioned windows.
		// SM_CYCAPTION - the height of the caption area.

		left -= GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
		top -= GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER);
	}

	SetWindowPos(hWnd, NULL, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// NOTE: A borderless window (sf::Style::None) works as exclusive fullscreen if the resolution is maximum,
	//       Disabling WS_POPUP flag fixes this problem

	SetWindowLong(hWnd, GWL_STYLE, style & ~WS_POPUP);
#endif
}
