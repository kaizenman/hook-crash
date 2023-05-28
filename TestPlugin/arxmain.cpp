#include "pch.h"

static WNDPROC g_mainProc = NULL;
static WNDPROC g_pageWndProc = NULL;

static WNDPROC g_controlWndProc1= NULL;
static WNDPROC g_controlWndProc2 = NULL;
static WNDPROC g_controlWndProc3 = NULL;
static WNDPROC g_comboWndProc = NULL;
static WNDPROC g_listBoxWndProc = NULL;

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

LRESULT __stdcall PageWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        std::cout << "test";
    }
    return CallWindowProc(g_pageWndProc, hWnd, uMsg, wParam, lParam);
}

LRESULT __stdcall ControlWndProc1(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        std::cout << "test";
    }
    return CallWindowProc(g_controlWndProc1, hWnd, uMsg, wParam, lParam);
}

LRESULT __stdcall ControlWndProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        std::cout << "test";
    }
    return CallWindowProc(g_controlWndProc2, hWnd, uMsg, wParam, lParam);
}

LRESULT __stdcall ControlWndProc3(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        std::cout << "test";
    }
    return CallWindowProc(g_controlWndProc3, hWnd, uMsg, wParam, lParam);
}

LRESULT __stdcall ComboWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        std::cout << "test";
    }
    return CallWindowProc(g_comboWndProc, hWnd, uMsg, wParam, lParam);
}

LRESULT __stdcall ListBoxWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        std::cout << "test";
    }
    return CallWindowProc(g_listBoxWndProc, hWnd, uMsg, wParam, lParam);
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

