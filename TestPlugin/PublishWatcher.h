#pragma once

#include "pch.h"

#define TIMER_ID 1234

class PublishWatcher
{
public:
    static void __stdcall TimerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime);
    static void ScheduleTimer();

    static int attempt;
    static DWORD procId;
};

