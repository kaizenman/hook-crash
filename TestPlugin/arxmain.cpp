#include "pch.h"

#include <iostream>
#include <vector>

static WNDPROC g_pTreeWndProc = NULL;
static WNDPROC g_mainProc = NULL;
static HHOOK g_hHook = NULL;

#define MY_MESSAGE WM_USER + 1

#define NewDataShortcut 0x5237
#define DialogClassName L"#32770"

class MyDialog : public CDialog
{
public:
    MyDialog() : CDialog() {}

    virtual BOOL OnInitDialog() override {
        CDialog::OnInitDialog();

        // Create a button
        CButton* myButton = new CButton();
        myButton->Create(_T("My Button"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            CRect(10, 10, 100, 30), this, 1);

        // Create a static text
        CStatic* myStatic = new CStatic();
        myStatic->Create(_T("My Static Text"), WS_CHILD | WS_VISIBLE,
            CRect(10, 50, 200, 70), this);

        return TRUE;
    }
};

/*

This is because hook procedures can be called very frequently and also because they run within the context of the thread that triggered the hook event.

The MessageBox() function creates a modal dialog box and enters a message loop until the user closes the dialog box. This means that while the message box is displayed, your application will continue to process messages. This can potentially lead to re-entrancy problems if your hook procedure gets called again before the first call has returned.

This is why it's recommended to post a message to your own window procedure, and then display the message box or dialog when handling that message. This makes sure you're not inside the hook procedure when starting a new message loop.

In general, you need to be very careful when calling any function that might not return immediately from a hook procedure.


*/


void ShowDialog()
{
    //AFX_MANAGE_STATE(AfxGetStaticModuleState())
   /* MessageBox(NULL,
        L"This is a message",
        L"Message Box Title",
        MB_OK | MB_ICONINFORMATION);
        */
    MyDialog dlg;
    dlg.DoModal();
}

LRESULT __stdcall HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HCBT_ACTIVATE)
    {
        HWND hWnd = (HWND)wParam;
        TCHAR szClassName[256];
        GetClassName(hWnd, szClassName, 256);
        if (!_tcscmp(szClassName, DialogClassName)) {
            UnhookWindowsHookEx(g_hHook);
            g_hHook = NULL;

            DWORD lastError = GetLastError();
            std::cout << lastError << '\n';

            CMDIFrameWnd* pFrame = acedGetAcadFrame();
            HWND hFrame = pFrame->GetSafeHwnd();
            PostMessage(hFrame, MY_MESSAGE, 0, 0);
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}



LRESULT __stdcall TreeWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int wID = LOWORD(wParam);

    if (uMsg == WM_GETTEXT) {
        std::cout << "here\n";
    }
    else if (uMsg == WM_COMMAND && wID == NewDataShortcut) {
        g_hHook = SetWindowsHookEx(WH_CBT, HookProc, NULL, GetCurrentThreadId());
    }

    return CallWindowProc(g_pTreeWndProc, hWnd, uMsg, wParam, lParam);
}

LRESULT __stdcall MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == MY_MESSAGE) {
        ShowDialog();
    }

    return CallWindowProc(g_mainProc, hWnd, uMsg, wParam, lParam);
}


void FindTreeWindow()
{
    CMDIFrameWnd* pFrame = acedGetAcadFrame();
    if (pFrame) {
        HWND hWnd = pFrame->GetSafeHwnd();

        g_mainProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)MainWndProc);

        std::vector<HWND> children;
        EnumChildWindows(hWnd, [](HWND hwnd, LPARAM lParam) -> BOOL {
            std::vector<HWND>* children = (std::vector<HWND>*)lParam;
            children->push_back(hwnd);
            return TRUE;
        }, (LPARAM)&children);

        for (HWND hwnd : children) {
            if (NULL == hwnd) {
                continue;
            }

            HWND hToolSpace = GetWindow(hwnd, GW_CHILD);
            HWND hProspector = GetDlgItem(hToolSpace, 0xE900);
            HWND hUpper = GetDlgItem(hProspector, 0xE900);
            HWND hLower = GetDlgItem(hProspector, 0xE910);

            HWND hTree = GetDlgItem(hUpper, 0xE900);
            if (NULL == hTree)
                continue;
            
            std::vector<HWND> treeChildren;
            EnumChildWindows(hTree, [](HWND hwnd, LPARAM lParam) -> BOOL {
                std::vector<HWND>* children = (std::vector<HWND>*)lParam;
                children->push_back(hwnd);
                return TRUE;
            }, (LPARAM)&treeChildren);

            TCHAR szText[256];
            GetClassName(hTree, szText, 256);
            if (0 == wcscmp(szText, L"SysTreeView32")) {
                if (NULL != g_pTreeWndProc) {
                    SetWindowLongPtr(hTree, GWLP_WNDPROC, (LONG_PTR)g_pTreeWndProc);
                }

                g_pTreeWndProc = (WNDPROC)SetWindowLongPtr(hTree, GWLP_WNDPROC, (LONG_PTR)TreeWndProc);
            }
        }
    }
}


void Test()
{
    MessageBox(NULL,
        L"This is a message",
        L"Message Box Title",
        MB_OK | MB_ICONINFORMATION);
    // FindTreeWindow();
}

extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
    switch (msg) {
    case AcRx::kInitAppMsg:
        acrxDynamicLinker->unlockApplication(appId);
        acrxDynamicLinker->registerAppMDIAware(appId);
        acedRegCmds->addCommand(L"TESTCIVIL", L"testcommand", L"testcommand", ACRX_CMD_MODAL, Test);

        break;
    case AcRx::kUnloadAppMsg:

        break;
    }
    return AcRx::kRetOK;
}

