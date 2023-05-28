#include "pch.h"

static WNDPROC g_mainAutoCADWndProc = NULL;

LRESULT __stdcall AcadWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_COMMAND)
    {
        MessageBox(NULL,
            L"This is a message",
            L"Message Box Title",
            MB_OK | MB_ICONINFORMATION);
    }

    return CallWindowProc(g_mainAutoCADWndProc, hwnd, msg, wParam, lParam);
}

void Test()
{
    g_mainAutoCADWndProc = (WNDPROC)SetWindowLongPtr(acedGetAcadFrame()->m_hWnd, GWLP_WNDPROC, (LONG_PTR)AcadWndProc);

}

void FindWindow(HWND hwnd)
{

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

