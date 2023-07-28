#ifndef UNICODE
#define UNICODE
#endif 
#define TRAY_ICON_ID 1
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

bool bubblePopperOpen = false;
int ActivationKeyCode = 34;
LPCWSTR programPath;
std::string programPathString;
LPCWSTR iconPath;


bool IsWindowMinimized(HWND hWnd) {
    return IsIconic(hWnd) != 0;
}

HWND GetWindowHandleByTitle(const wchar_t* windowName) {
    return FindWindowW(NULL, windowName);
}

bool IsWindowOpen(LPCWSTR windowName) {
    HWND hWnd = FindWindow(NULL, windowName);
    return (hWnd != NULL);
}
void MinimizeWindow(HWND hWnd) {
    ShowWindow(hWnd, SW_MINIMIZE);
}

// Function to restore a window given its hWnd handle
void RestoreWindow(HWND hWnd) {
    ShowWindow(hWnd, SW_RESTORE);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        // Extract keyboard event information from the 'lParam' parameter
        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        
        // Process the keyboard event here
        // Example: check for a key press event (wParam == WM_KEYDOWN) and print the virtual key code
        if (wParam == WM_KEYDOWN) {
            DWORD keyCode = kbStruct->vkCode;
            

            
            
            if(keyCode == ActivationKeyCode)
            {
                std::cout << keyCode << std::endl;
                if(IsWindowOpen(L"BubblePopper"))
                {
                    std::cout << "Its Open" << std::endl;
                    if(IsWindowMinimized(GetWindowHandleByTitle(L"BubblePopper")))
                    {
                        RestoreWindow(GetWindowHandleByTitle(L"BubblePopper"));
                    }
                    else
                    {
                        MinimizeWindow(GetWindowHandleByTitle(L"BubblePopper"));
                    }
                }
                else
                {
                    bubblePopperOpen = true;
                    ShellExecute(NULL, L"open", programPath, NULL, NULL, SW_SHOWNORMAL);
                    std::cout << "Created " << std::endl;
                }
            }
        }
    }

    // Call the next hook in the hook chain
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    bool running = true;

    std::ifstream optionsFile("options.txt");
    std::string s;
    getline(optionsFile, s);
    s.clear();
    getline(optionsFile, s);
    std::wstring stemp = std::wstring(s.begin(), s.end());
    LPCWSTR sw = stemp.c_str();
    programPath = sw;
    programPathString = s.c_str();
    s.clear();
    getline(optionsFile, s);
    std::wstring stemp2 = std::wstring(s.begin(), s.end());
    LPCWSTR sw2 = stemp2.c_str();
    iconPath = sw2;
    s.clear();
    getline(optionsFile, s);
    try
    {
        ActivationKeyCode = stoi(s); 
    }
    catch(const std::exception& e)
    {
        ActivationKeyCode = 34;
    }
    
    

    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    if (hook == NULL) {
        std::cerr << "Failed to set up keyboard hook." << std::endl;
        return 1;
    }
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"The Bubble Popper Manager";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"The Bubble Popper Manager",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        return 0;
    }

    // Initialize the system tray icon.
    SendMessage(hwnd, WM_CREATE, 0, 0);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
        {
            // Load the icon for the system tray.

            // Set up the NOTIFYICONDATAW structure (note the 'W' suffix).
            NOTIFYICONDATAW nid = {};
            nid.cbSize = sizeof(nid);
            nid.hWnd = hwnd;
            nid.uID = TRAY_ICON_ID;
            nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
            nid.uCallbackMessage = WM_USER + 1; // Custom notification message
            nid.hIcon = (HICON)LoadImageW(NULL, iconPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
            lstrcpyW(nid.szTip, L"Bubble Popper Manager"); // Use lstrcpyW for wide-character strings.

            // Add the icon to the system tray.
            Shell_NotifyIconW(NIM_ADD, &nid); // Use Shell_NotifyIconW for wide-character strings.

            // Show the window in the background (optional).
            ShowWindow(hwnd, SW_HIDE);

            break;
        }
        case WM_USER + 1:
        {
            // Handle tray icon messages (e.g., mouse click events, context menu).

            // For example, you could show a context menu when the user right-clicks the tray icon.
            
            if (lParam == WM_LBUTTONDOWN) {
                // Left button clicked
                bubblePopperOpen = false;
                HWND bp = FindWindow(NULL, L"BubblePopper");
                MinimizeWindow(bp);
            }
            if (lParam == WM_RBUTTONDOWN) {
                SendMessage(hwnd, WM_DESTROY, NULL, NULL);
            }

            break;
        }
        case WM_DESTROY:
        {
            // Remove the system tray icon when the window is closed.
            NOTIFYICONDATAW nid = {};
            nid.cbSize = sizeof(nid);
            nid.hWnd = hwnd;
            nid.uID = TRAY_ICON_ID;

            Shell_NotifyIconW(NIM_DELETE, &nid); // Use Shell_NotifyIconW for wide-character strings.
            CoUninitialize();
            // Exit the application.
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam); // Use DefWindowProcW for wide-character strings.
    }

    return 0;
}
