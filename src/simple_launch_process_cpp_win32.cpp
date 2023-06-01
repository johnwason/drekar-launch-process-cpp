#include <windows.h>
#include <iostream>
#include <thread>
#include <assert.h>

#include "simple_launch_process_cpp/simple_launch_process_cpp.h"
// Create a class with a message only window and a simple message loop

namespace simple_launch_process_cpp
{
namespace detail
{

class WaitForExitImpl
{
public:

    static HWND m_hwnd;
    WaitForExitImpl()  {
        assert(impl_counter == 0);
        impl_counter++;
        m_hwnd = 0;
    }
    ~WaitForExitImpl() { 
        SendClose(); 
        impl_counter--;          
    }

    static int impl_counter;

    // Create the message window
    BOOL Create(LPCSTR pszClassName, LPCSTR pszWindowName)
    {
        WNDCLASSEXA wcex = { sizeof(WNDCLASSEXA) };
        wcex.cbSize = sizeof(WNDCLASSEXA);
        //wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WaitForExitImpl::WndProc;
        wcex.hInstance = GetModuleHandleA(NULL);
        //wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        //wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = pszClassName;
        if (!RegisterClassExA(&wcex))
            return FALSE;
        m_hwnd = CreateWindowExA(0, pszClassName, pszWindowName, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, GetModuleHandleA(NULL), this);
        BOOL res = (m_hwnd != NULL);
        return res;
    }

    void SendClose()
    {
        if (m_hwnd == NULL) return;
        PostMessageA(m_hwnd, WM_CLOSE, 0, 0);
    }

    // Destroy the message window
    void Destroy()
    {
        if (m_hwnd != NULL)
        {
            DestroyWindow(m_hwnd);
            m_hwnd = NULL;
        }
    }

    // Message window procedure
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        WaitForExitImpl* pThis = NULL;
        
        if (uMsg == WM_NCCREATE)
        {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            pThis = reinterpret_cast<WaitForExitImpl*>(lpcs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = reinterpret_cast<WaitForExitImpl*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (pThis != NULL)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProcA(hwnd, uMsg, wParam, lParam);
        }
    }

    // Message handler
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
            OnDestroy();
            break;
        case WM_CLOSE:
            DestroyWindow(m_hwnd);
            break;
        default:
            return DefWindowProcA(m_hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }

    
    // Called when the window is destroyed
    void OnDestroy()
    {
        PostQuitMessage(0);
    }

    void RunMessageLoop()
    {
        // Wait for the message window to be destroyed
        MSG msg;
        while (GetMessageA(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    // Create a function to override console control events and destroy the window

    static BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
    {
        if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT || dwCtrlType == CTRL_CLOSE_EVENT)
        {
            PostMessage(m_hwnd, WM_CLOSE, 0, 0);
            return TRUE;
        }
        return FALSE;
    }

    // Create function to set console control handler

    BOOL SetConsoleHandlerRoutine()
    {
        return SetConsoleCtrlHandler(WaitForExitImpl::ConsoleHandlerRoutine, TRUE);
    }

};

HWND WaitForExitImpl::m_hwnd = 0;
int WaitForExitImpl::impl_counter = 0;
}

// Implement WaitForExit

CWaitForExit::CWaitForExit()
{
    impl = new detail::WaitForExitImpl();
}

// TODO: better storage of thread
std::thread wait_exit_thread;
CWaitForExit::~CWaitForExit()
{
    impl->SendClose();
    if (wait_exit_thread.joinable())
        wait_exit_thread.join();
    delete impl;
    
}


bool CWaitForExit::CallbackWaitForExit(void (*exit_callback)())
{
    wait_exit_thread = std::thread([this,exit_callback] {
        WaitForExit();
        exit_callback();
    });
    return true;  
}

void CWaitForExit::WaitForExit()
{
    // Create the message window
    if (!impl->Create("simple_launch_process", "simple_launch_process"))
    {
        std::cerr << "Failed to create message window" << std::endl;
        return;
    }

    // Set the console control handler
    if (!impl->SetConsoleHandlerRoutine())
    {
        std::cerr << "Failed to set console control handler" << std::endl;
        return;
    }

    // Run the message loop
    impl->RunMessageLoop();    
}

}

