#include <windows.h>

#define IDR_ACCELERATOR1 109
//Win proc
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
//Starts the timer
void ActivateTimer(HWND hwnd);
//Shut down windows
bool MySystemShutdown();

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );
HINSTANCE				hGlobalInstance;
static unsigned int		iShutDownTime_inSeconds; //Timer value
HWND         	hwnd ;

char szBuffer[50]; //Temp buffer
BOOL bTemp; //Temp boolean	  

BOOL bShowLzLog = false;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
  PSTR szCmdLine, int iCmdShow)
{
  static TCHAR	szAppName[] = TEXT ("F4 catcher") ;

  MSG          	msg ;
  WNDCLASS     	wndclass ;
  HACCEL			hAccel ; //handle for the accelerators table

  wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
  wndclass.lpfnWndProc   = WndProc ;
  wndclass.cbClsExtra    = 0 ;
  wndclass.cbWndExtra    = 0 ;
  wndclass.hInstance     = hInstance ;
  wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
  wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
  wndclass.lpszMenuName  = NULL ;
  wndclass.lpszClassName = szAppName ;

  if (!RegisterClass (&wndclass))
  {
    MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
      szAppName, MB_ICONERROR) ;
    return 0 ;
  }
  hwnd = CreateWindow (szAppName,                  	// window class name
    TEXT ("F4 catcher"),	    // window caption
    WS_OVERLAPPEDWINDOW,        	// window style
    CW_USEDEFAULT,              	// initial x position
    CW_USEDEFAULT,              	// initial y position
    200,              			// initial x size
    100,              			// initial y size
    NULL,             			// parent window handle
    NULL,                       	// window menu handle
    hInstance,                  	// program instance handle
    NULL) ;                     	// creation parameters

  ShowWindow (hwnd, iCmdShow) ;
  UpdateWindow (hwnd) ;

  //Load the accelerators table
  hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1) ) ;

  //Retrieve the applications instance
  HINSTANCE instance = GetModuleHandle(NULL);
  //Set a global Windows Hook to capture keystrokes using the function declared above
  HHOOK test1 = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, instance,0);

  while (GetMessage (&msg, NULL, 0, 0))
  {
    if (!TranslateAccelerator (hwnd, hAccel, &msg))
    {
      TranslateMessage (&msg) ;
      DispatchMessage (&msg) ;
    }
  }
}

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
  char pressedKey;
  HWND hwndForeground;
  
  // If code is less than zero, the hook procedure must return the value returned by CallNextHookEx
  if(nCode < 0)
    CallNextHookEx( NULL, nCode, wParam, lParam);

  // Declare a pointer to the KBDLLHOOKSTRUCTdsad
  KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
  
  switch( wParam )
  {
  case WM_KEYDOWN: // When the key has been pressed and released
    {
      //get the key code
      pressedKey = (char)pKeyBoard->vkCode;
    }
    break;
  default:
    return CallNextHookEx( NULL, nCode, wParam, lParam );
  }

  //do something with the pressed key here
  if(pressedKey == VK_F4)
  {
    HWND hwndLzLog = FindWindowA(NULL, "LZ-log by LZ1ABC"); // Find LZ-log

    // Do nothing if Lz-Log is not running
    if(hwndLzLog == NULL)
      return CallNextHookEx( NULL, nCode, wParam, lParam);

    hwndForeground = GetForegroundWindow();

    // Do nothing if Lz-Log has the focus already
    if(hwndForeground == hwndLzLog)
      return CallNextHookEx( NULL, nCode, wParam, lParam);
   

    DWORD dwForeThread = GetWindowThreadProcessId(GetForegroundWindow(), 0); // what process owns foreground window?
    DWORD dwAppThread = GetCurrentThreadId(); // what process is our window owned by?

    if(dwForeThread != dwAppThread) // Our process doesn't own the foreground window 
    {
      AttachThreadInput(dwForeThread, dwAppThread, TRUE); // let's become a part of this the process that owns the foreground window so we can bring our window to the front
      BringWindowToTop(hwndLzLog);
      ShowWindow(hwndLzLog, SW_RESTORE);
      AttachThreadInput(dwForeThread, dwAppThread, FALSE); // ok, we're done bringing our window to the front so let's detach now
    }
    else
    {
      BringWindowToTop(hwndLzLog);
      ShowWindow(hwndLzLog, SW_RESTORE);
    }
  }

  //according to winapi all functions which implement a hook must return by calling next hook
  return CallNextHookEx( NULL, nCode, wParam, lParam);
}



LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  HDC					  hdc;
  PAINTSTRUCT	  ps;
  RECT					rect;
 
  switch (message)
  {
  case WM_CREATE:
    return 0 ;

  case WM_PAINT:
    
    hdc = BeginPaint (hwnd, &ps) ;    

    GetClientRect (hwnd, &rect) ;
    TextOut(hdc, 0,0, 
            TEXT("Will interrupt all F4 presses"),
            strlen("Will interrupt all F4 presses"));
    TextOut(hdc, 0,30, 
            TEXT("and will try to set focus on \"LZ-log by LZ1ABC\""),
            strlen("and will try to set focus on \"LZ-log by LZ1ABC\""));

    EndPaint (hwnd, &ps) ;
    return 0 ;
 
  case WM_KEYDOWN:
    return 0;

  case WM_DESTROY:
    PostQuitMessage (0) ;
    return 0 ;
  }
  return DefWindowProc (hwnd, message, wParam, lParam) ;
}

