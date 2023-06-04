#pragma once

#include "pch.h"

#define TIMER_ID 1234

class PublishWatcher
{
public:
    enum class PublishMode
    {
        None,
        DWF,
        PDF
    };

    static void __stdcall TimerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime);
    static void ScheduleTimer();

    static void SetPublishMode(PublishMode mode);
    static PublishMode GetPublishMode();

    static int attempt;
    static DWORD procId;

    static PublishMode publishMode;
};

