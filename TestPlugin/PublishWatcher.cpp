#include "pch.h"

int PublishWatcher::attempt = 0;
DWORD PublishWatcher::procId = -1;

namespace Detail {
    DWORD FindChildProc(DWORD parent, std::wstring const& name)
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (Process32First(snapshot, &entry) == TRUE)
        {
            while (Process32Next(snapshot, &entry) == TRUE)
            {
                if (entry.th32ParentProcessID == parent)
                {
                    if (_tcsicmp(name.c_str(), entry.szExeFile) == 0)
                    {
                        CloseHandle(snapshot);
                        return entry.th32ProcessID;
                    }
                }
            }
        }

        CloseHandle(snapshot);
        return -1;
    }
}


void PublishWatcher::ScheduleTimer()
{
    if (acedGetAcadFrame())
    {
        ::KillTimer(acedGetAcadFrame()->m_hWnd, TIMER_ID);
        ::SetTimer(acedGetAcadFrame()->m_hWnd, TIMER_ID, 500, TimerProc);
    }
}

void __stdcall PublishWatcher::TimerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
    constexpr int kAttempts = 50;
    constexpr int delay = 500;

    if (procId == -1)
    {
        if (attempt > kAttempts)
        {
            ::KillTimer(acedGetAcadFrame()->m_hWnd, TIMER_ID);
            std::cout << "Error";
            return;
        }
        
        ++attempt;
    
        procId = Detail::FindChildProc(GetCurrentProcessId(), L"acad.exe");
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | READ_CONTROL, TRUE, procId);
    if (hProcess)
    {
        DWORD exitCode;
        if (GetExitCodeProcess(hProcess, &exitCode))
        {
            if (exitCode == STILL_ACTIVE)
            {
                if (Detail::FindChildProc(GetCurrentProcessId(), L"acad.exe") != -1)
                    return;
            }
        }
    }

    procId = -1;
    ::KillTimer(acedGetAcadFrame()->m_hWnd, TIMER_ID);

    // Do something
}
