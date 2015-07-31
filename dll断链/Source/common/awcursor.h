#pragma once
#include <winbase.h>
#include <winuser.h>
#include <windef.h>



class AutoWaitCursor
	{
public:
	AutoWaitCursor()
		{
		if (main_threadid == 0)
			{
			main_threadid = GetCurrentThreadId();
			DWORD threadid = 0;
			CreateThread(NULL, 1000, timer_thread, 0, 0, &threadid);
			prev_hook = SetWindowsHookEx(WH_GETMESSAGE, getmessage_hook, 0, main_threadid);
			}

		ticks = 5; // start the timer
		prev_cursor = 0;
		}
	~AutoWaitCursor()
		{
		ticks = 0; // stop timer
		if (prev_cursor)
			{
			// restore cursor
			POINT pt;
			GetCursorPos(&pt);
			SetCursorPos(pt.x, pt.y);
			}
		}
private:
	static DWORD WINAPI timer_thread(LPVOID lpParameter)
		{
		HCURSOR wait_cursor = LoadCursor(NULL, IDC_WAIT);
		// according to MSDN, need to do API call before AttachThreadInput
		AttachThreadInput(GetCurrentThreadId(), main_threadid, true);
		for (;;)
			{
			Sleep(25); // milliseconds
			if (ticks > 0 && --ticks == 0)
				{
				prev_cursor = SetCursor(wait_cursor);
				}
			}
		}
	static LRESULT CALLBACK getmessage_hook(int code, WPARAM wParam, LPARAM lParam)
		{
		ticks = 0; // stop timer
		return CallNextHookEx(prev_hook, code, wParam, lParam);
		}	
	static DWORD main_threadid;
	static HHOOK prev_hook;
	static int volatile ticks;
	static HCURSOR volatile prev_cursor;
	};
DWORD AutoWaitCursor::main_threadid = 0;
HHOOK AutoWaitCursor::prev_hook = 0;
int volatile AutoWaitCursor::ticks = 0;
HCURSOR volatile AutoWaitCursor::prev_cursor = 0;
