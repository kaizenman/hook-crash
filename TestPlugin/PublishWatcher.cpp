#include "pch.h"

#define ACAD_VERSION 2023

int PublishWatcher::attempt = 0;
DWORD PublishWatcher::procId = -1;
PublishWatcher::PublishMode PublishWatcher::publishMode = PublishWatcher::PublishMode::DWF;


#if ACAD_VERSION >= 2023
#define PUBLISH_BACKGROUND_MULTISHEET_ON_DWF_PROCESS     L"acad.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_ON_PDF_PROCESS     L"Das.Local.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_ON_PDF_SUBPROCESS  L"accoreconsole.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_OFF_PROCESS        L"Das.Local.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_OFF_SUBPROCESS     L"accoreconsole.exe"
#elif ACAD_VERSION >= 2021
#define PUBLISH_BACKGROUND_MULTISHEET_ON_PROCESS         L"acad.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_ON_SUBPROCESS      L"accefsubprocess.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_OFF_PROCESS        L"Das.Local.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_OFF_SUBPROCESS     L"accoreconsole.exe"
#else
#define PUBLISH_BACKGROUND_MULTISHEET_ON_PROCESS         L"acad.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_ON_SUBPROCESS      L"acwebbrowser.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_OFF_PROCESS        L"Das.Local.exe"
#define PUBLISH_BACKGROUND_MULTISHEET_OFF_SUBPROCESS     L"accoreconsole.exe"
#endif


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

void PublishWatcher::SetPublishMode(PublishMode mode)
{
    publishMode = mode;
}

PublishWatcher::PublishMode PublishWatcher::GetPublishMode()
{
    return publishMode;
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
    int kAttempts = 50;
    int delay = 500;

    std::wstring processName;
    std::wstring subProcessName;

    // test
    bool multisheet = true;

    if (multisheet)
    {
#if ACAD_VERSION >= 2023
        if (publishMode == PublishMode::DWF)
        {
            processName = PUBLISH_BACKGROUND_MULTISHEET_ON_DWF_PROCESS;
        }
        else
        {
            processName = PUBLISH_BACKGROUND_MULTISHEET_ON_PDF_PROCESS;
            subProcessName = PUBLISH_BACKGROUND_MULTISHEET_ON_PDF_SUBPROCESS;
        }
#else
        processName = PUBLISH_BACKGROUND_MULTISHEET_ON_PROCESS;
        subProcessName = PUBLISH_BACKGROUND_MULTISHEET_ON_SUBPROCESS;
#endif
    }
    else
    {
        processName = PUBLISH_BACKGROUND_MULTISHEET_OFF_PROCESS;
        subProcessName = PUBLISH_BACKGROUND_MULTISHEET_OFF_SUBPROCESS;
    }

    if (procId == -1)
    {
        if (attempt > kAttempts)
        {
            ::KillTimer(acedGetAcadFrame()->m_hWnd, TIMER_ID);
            MessageBox(NULL, L"Publish Error!!!", L"Message Box Title", MB_OK | MB_ICONINFORMATION);
            return;
        }

        ++attempt;

        procId = Detail::FindChildProc(GetCurrentProcessId(), processName);
        if (-1 == procId)
            return;

        if (publishMode == PublishMode::PDF)
        {
            if (-1 == Detail::FindChildProc(procId, subProcessName))
            {
                procId = -1;
                return;
            }
        }
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | READ_CONTROL, TRUE, procId);
    if (NULL != hProcess)
    {
        DWORD exitCode = 0;
        GetExitCodeProcess(hProcess, &exitCode);
        CloseHandle(hProcess);
        if (exitCode == STILL_ACTIVE)
        {
            if (-1 != Detail::FindChildProc(GetCurrentProcessId(), processName))
                return;
            if (publishMode == PublishMode::PDF)
            {
                if (-1 != Detail::FindChildProc(procId, subProcessName))
                    return;
            }
        }

    }

    attempt = 0;
    procId = -1;
    ::KillTimer(acedGetAcadFrame()->m_hWnd, TIMER_ID);

    // End publish
    Reactors::GetPublishReactor().RealOnEndPublish();

}
