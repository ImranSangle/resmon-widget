#include <iostream>
#include <windef.h>
#include <windows.h>
#include <math.h>
#include <thread>
#include <windowsx.h>

//local headers---------
#include "workers.h"
#include "calculations.h"
#include "draw.h"

#define BITSMALL 150 
#define BITMEDIUM 250 
#define BITLARGE 400 

//variable used across all translation
int x;
int y;
int view = 0;
HBITMAP resourceS[2];
HBITMAP resourceM[2];
HBITMAP resourceL[2];
HBITMAP speedometer[2];
float windowWidth = BITSMALL;
float windowHeight = BITSMALL;

//static variables only for this translation
static bool quit = false;
static bool move = false;
static HICON windowIcon;
static HMENU popup;
static HRGN MainWindowRegn;
static HWND mainWindow;

NOTIFYICONDATAW nid;

void loadBitmaps(){
    windowIcon = (HICON)LoadImageW(NULL,L"./resources/icon.ico",IMAGE_ICON,0,0,LR_LOADFROMFILE);
    resourceS[0] = (HBITMAP)LoadImageW(NULL,L"./resources/speedometer.bmp",IMAGE_BITMAP,BITSMALL,BITSMALL,LR_LOADFROMFILE);
    resourceS[1] = (HBITMAP)LoadImageW(NULL,L"./resources/speedometerCpu.bmp",IMAGE_BITMAP,BITSMALL,BITSMALL,LR_LOADFROMFILE);
    resourceM[0] = (HBITMAP)LoadImageW(NULL,L"./resources/speedometer.bmp",IMAGE_BITMAP,BITMEDIUM,BITMEDIUM,LR_LOADFROMFILE);
    resourceM[1] = (HBITMAP)LoadImageW(NULL,L"./resources/speedometerCpu.bmp",IMAGE_BITMAP,BITMEDIUM,BITMEDIUM,LR_LOADFROMFILE);
    resourceL[0] = (HBITMAP)LoadImageW(NULL,L"./resources/speedometer.bmp",IMAGE_BITMAP,BITLARGE,BITLARGE,LR_LOADFROMFILE);
    resourceL[1] = (HBITMAP)LoadImageW(NULL,L"./resources/speedometerCpu.bmp",IMAGE_BITMAP,BITLARGE,BITLARGE,LR_LOADFROMFILE);
}

void assignBitmaps(const int& size){
  switch(size){
    case BITSMALL:
     speedometer[0] = resourceS[0];
     speedometer[1] = resourceS[1];
    break;
    case BITMEDIUM:
     speedometer[0] = resourceM[0];
     speedometer[1] = resourceM[1];
    break;
    case BITLARGE:
     speedometer[0] = resourceL[0];
     speedometer[1] = resourceL[1];
    break;
  }
}

void setRegion(HWND hwnd){
    DeleteObject(MainWindowRegn);
    MainWindowRegn = CreateEllipticRgn(0, 0, windowWidth,windowHeight);
    SetWindowRgn(hwnd, MainWindowRegn, TRUE);
}

void resizeWidget(HWND hwnd,const int& size){
    windowWidth = size;
    windowHeight = size;
    assignBitmaps(size);
    setRegion(hwnd);
    SetWindowPos(hwnd,NULL,0,0,windowWidth,windowHeight,SWP_NOMOVE);
//  SendMessageW(hwnd,WM_MOUSEMOVE,MK_LBUTTON,0);
}

void SetupSystemTrayIcon(HWND hwnd, HINSTANCE hInstance) {
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hwnd;
    nid.uID = 1; // Unique ID for the icon
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.hIcon = windowIcon;// LoadIconW(hInstance,IDI_APPLICATION); // Load an icon
    nid.uCallbackMessage = WM_APP; // Custom message to handle tray icon events
    lstrcpyW(nid.szTip,L"RamWidget"); // Tooltip text for the icon

    Shell_NotifyIconW(NIM_ADD, &nid);
}

void appendMenu(HWND hwnd){

    popup = CreatePopupMenu();
    HMENU sizeMenu = CreateMenu();
    HMENU viewMenu = CreateMenu();

    AppendMenuW(popup,MF_POPUP,(UINT_PTR)viewMenu,L"View");
    AppendMenuW(viewMenu,MF_STRING,6,L"RAM");
    AppendMenuW(viewMenu,MF_STRING,7,L"CPU");


    AppendMenuW(popup,MF_POPUP,(UINT_PTR)sizeMenu,L"Size");
    AppendMenuW(sizeMenu,MF_STRING,3,L"Small");
    AppendMenuW(sizeMenu,MF_STRING,4,L"Medium");
    AppendMenuW(sizeMenu,MF_STRING,5,L"Large");

    AppendMenuW(popup,MF_STRING,1,L"Minimize");
    AppendMenuW(popup,MF_STRING,2,L"Quit");
}

LRESULT CALLBACK windowProcedure(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
  switch(msg){
    case WM_NCHITTEST:
     { 
       if(GetAsyncKeyState(VK_LBUTTON) & 0x8000){
         POINT pt;
         pt.x = GET_X_LPARAM(lp);
         pt.y = GET_Y_LPARAM(lp);
         return HTCAPTION;
       }

       return HTCLIENT;

     } 
    break;
    case WM_COMMAND:
      switch(wp){
          case 1:
           ShowWindow(hwnd,SW_HIDE);
          break;
          case 2:
           Shell_NotifyIconW(NIM_DELETE, &nid);
           PostQuitMessage(0);
           quit = true;
          break;
          case 3:
           resizeWidget(hwnd,BITSMALL);
          break;
          case 4:
           resizeWidget(hwnd,BITMEDIUM);
          break;
          case 5:
           resizeWidget(hwnd,BITLARGE);
          break;
          case 6:
           view = 0;
          break;
          case 7:
           view = 1;
          break;
      }
    break;
    case WM_APP:
      if (lp == WM_LBUTTONUP)
      {
          ShowWindow(hwnd, SW_SHOWNORMAL); 
      }
      if(lp == WM_RBUTTONDOWN){
          POINT pt;
          GetCursorPos(&pt);
          TrackPopupMenu(popup,TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.x,pt.y,0,hwnd,NULL);
      }
      if(lp == WM_MOUSEMOVE){
         
      }
    break;
    case WM_CREATE:
     appendMenu(hwnd);
     SetupSystemTrayIcon(hwnd, ((LPCREATESTRUCT)lp)->hInstance);
    break;
    case WM_CLOSE:
     ShowWindow(hwnd,SW_HIDE);
    break;
    case WM_CONTEXTMENU:
     TrackPopupMenu(popup,TPM_LEFTALIGN,LOWORD(lp),HIWORD(lp),0,hwnd,NULL);
    break;
    case WM_DESTROY:
     PostQuitMessage(0);
     quit = true;
    break;
    default:
     return DefWindowProc(hwnd,msg,wp,lp);
  }

    return (LRESULT) NULL;
}

void workerProcedure(){
  
  while(quit == false){
    updateValue(mainWindow);
    drawPointer(mainWindow);
    Sleep(16);
  }
}


int WINAPI WinMain(HINSTANCE instance,HINSTANCE prevInstance,LPSTR argv,int ncmdShow){

    loadBitmaps();
    assignBitmaps(BITSMALL);

    std::thread procThread(processorThread);
    
    WNDCLASSW wc = {0};
    wc.hbrBackground = (HBRUSH) GetStockObject(GRAY_BRUSH);
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.lpszClassName = L"mainWindowClass";
    wc.lpfnWndProc = windowProcedure;
    wc.hIcon = windowIcon;

    if(!RegisterClassW(&wc)){
      std::cout<<"failed to register MainWindow Class"<<std::endl;
      return 1;
    }
    
    mainWindow =  CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW ,L"mainWindowClass",L"",WS_VISIBLE | WS_POPUP,683,384,windowWidth,windowHeight,NULL,NULL,NULL,NULL);

    setRegion(mainWindow);

    std::thread worker(workerProcedure);

    MSG msg;

    while(GetMessage(&msg,0,0,0)){
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    DestroyWindow(mainWindow);

    return 0;
}
