#include "pch.h"

static WNDPROC g_mainProc = NULL;
static WNDPROC g_treeWndProc = NULL;

static HWINEVENTHOOK hWinEventHook = NULL;

static HWND hDialog = NULL;

/*
MessageBox(NULL,
    L"This is a message",
    L"Message Box Title",
    MB_OK | MB_ICONINFORMATION);    
*/


LRESULT __stdcall MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        std::cout << "test";
    }

    return CallWindowProc(g_mainProc, hWnd, uMsg, wParam, lParam);
}

void MyBeginPublish(PublishWatcher::PublishMode mode)
{
    std::cout << "MyBeginPublish";


    PublishWatcher::ScheduleTimer();
}

void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD dwEventThread,
    DWORD dwmsEventTime
);

LRESULT __stdcall TreeWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int wID = LOWORD(wParam);
    int Publish_DWF = 170;
    int Publish_PDF = 1249;

    if (uMsg == WM_COMMAND && (wID == Publish_DWF || wID == Publish_PDF))
    {
        // Begin publish
        PublishWatcher::PublishMode mode = PublishWatcher::PublishMode::None;

        if (wID == Publish_DWF)
            mode = PublishWatcher::PublishMode::DWF;
        else if (wID == Publish_PDF)
            mode = PublishWatcher::PublishMode::PDF;        

        PublishWatcher::SetPublishMode(mode);
        MyBeginPublish(mode);
        /*
        if (wID == Publish_DWF)
        {
            MyBeginPublish(mode);
        }
        else if (wID == Publish_PDF)
        {
            hWinEventHook = SetWinEventHook(
                EVENT_MIN, EVENT_MAX,
                NULL,
                WinEventProc,
                GetCurrentProcessId(), 0,
                WINEVENT_OUTOFCONTEXT
            );

            if (!hWinEventHook) {
                acutPrintf(_T("\nFailed to set windows event hook."));
            }
        }
        */
        LRESULT result = 0;
        result = CallWindowProc(g_treeWndProc, hWnd, uMsg, wParam, lParam);

        return result; 
    }

    return CallWindowProc(g_treeWndProc, hWnd, uMsg, wParam, lParam);
}

void FindMyWindow();


void Test()
{
    CMDIFrameWnd* pFrame = acedGetAcadFrame();
    if (pFrame) {
        HWND hWnd = pFrame->GetSafeHwnd();
        g_mainProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)MainWndProc);
    }

    FindMyWindow();

    // publish to pdf is started after the dialog is closed

}

struct EnumChildParam
{
    int count = 0;
    BOOL hasDirectUIHWND = FALSE;
};

BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
{
    EnumChildParam* param = (EnumChildParam*)lParam;

    TCHAR className[100];
    GetClassName(hwndChild, className, sizeof(className) / sizeof(TCHAR));

    if (_tcscmp(className, _T("DirectUIHWND")) == 0)
    {
        param->hasDirectUIHWND = TRUE;
    }

    param->count++;

    return !(param->hasDirectUIHWND && param->count > 15);
}

LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_NCDESTROY:
        if (hWnd == hDialog)
        {
            if (PublishWatcher::GetPublishMode() == PublishWatcher::PublishMode::PDF)
            {
                MyBeginPublish(PublishWatcher::PublishMode::PDF);
            }

            // Unhook the event hook
            UnhookWinEvent(hWinEventHook);
            hWinEventHook = NULL;

            // Clear hDialog when the dialog is destroyed
            hDialog = NULL;
        }
        break;
    }

    // Call the original window procedure for unhandled messages
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD dwEventThread,
    DWORD dwmsEventTime
)
{
    if (idObject == OBJID_WINDOW)
    {
        TCHAR className[100];
        GetClassName(hwnd, className, sizeof(className) / sizeof(TCHAR));

        if (event == EVENT_OBJECT_CREATE)
        {
            if (_tcscmp(className, _T("#32770")) == 0)  // #32770 is the class for a dialog box
            {
                EnumChildParam param{ 0, FALSE };
                EnumChildWindows(hwnd, EnumChildProc, (LPARAM)&param);
                if (param.count == 15 && param.hasDirectUIHWND)
                {
                    hDialog = hwnd;
                    SetWindowSubclass(hDialog, SubclassProc, 0, 0);
                }
            }
        } 
    }

}


void FindMyWindow()
{
    CMDIFrameWnd* framewnd = acedGetAcadFrame();
    HWND hWnd = framewnd->GetSafeHwnd();

    CDockState state;
    framewnd->GetDockState(state);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* barinfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        if (NULL == barinfo)
            continue;

        CControlBar* bar = framewnd->GetControlBar(barinfo->m_nBarID);
        if (NULL == bar)
            continue;

        HWND hWnd = bar->m_hWnd;

        int iFirstTab = 1;
        while (0 == GetDlgItem(hWnd, iFirstTab) && iFirstTab < 25)
        {
            iFirstTab++;
        }

        HWND hPane = GetDlgItem(hWnd, iFirstTab);
        if (NULL == hPane)
            continue;

        std::vector<HWND> children;
        EnumChildWindows(hPane, [](HWND hwnd, LPARAM lParam) -> BOOL {
            std::vector<HWND>* children = (std::vector<HWND>*)lParam;
            children->push_back(hwnd);
            return TRUE;
        }, (LPARAM)&children);

        for (int i = 0; i < children.size(); ++i) {
            HWND hItem = children[i];
            if (NULL == hItem)
                continue;

            std::vector<HWND> subChildren;
            EnumChildWindows(hItem, [](HWND hwnd, LPARAM lParam) -> BOOL {
                std::vector<HWND>* children = (std::vector<HWND>*)lParam;
                children->push_back(hwnd);
                return TRUE;
            }, (LPARAM)&subChildren);

            for (int j = 0; j < subChildren.size(); ++j) {
                HWND hTree = subChildren[j];
                if (NULL == hTree)
                    continue;

                TCHAR szText[256];
                GetClassName(hTree, szText, 256);
                if (0 == wcscmp(szText, L"SysTreeView32")) {
                    if (NULL != g_treeWndProc) {
                        SetWindowLongPtr(hTree, GWLP_WNDPROC, (LONG_PTR)g_treeWndProc);
                    }

                    g_treeWndProc = (WNDPROC)SetWindowLongPtr(hTree, GWLP_WNDPROC, (LONG_PTR)TreeWndProc);
                }
            }
        }
    }

}


extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
    switch (msg) {
    case AcRx::kInitAppMsg:
        acrxDynamicLinker->unlockApplication(appId);
        acrxDynamicLinker->registerAppMDIAware(appId);
        acedRegCmds->addCommand(L"TESTACAD", L"test", L"test", ACRX_CMD_MODAL, Test);

        Reactors::Init();   

        break;
    case AcRx::kUnloadAppMsg:
        acedRegCmds->removeGroup(L"TESTACAD");
        Reactors::Shutdown();

        break;
    }
    return AcRx::kRetOK;
}

