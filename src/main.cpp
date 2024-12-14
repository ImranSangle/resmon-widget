#include <iostream>
#include <windows.h>
#include <math.h>
#include <thread>

//local headers---------
#include "workers.h"
#include "calculations.h"
#include "draw.h"

static HICON windowIcon;
static HMENU popup;
static HRGN MainWindowRegn;

//variable used across all translation
 int x;
 int y;
 int view = 0;
 HBITMAP speedometer[2];
 float windowWidth = 150;
 float windowHeight = 150;

//static variables only for this translation
static bool quit = false;
static bool move = false;

NOTIFYICONDATAW nid;

void loadBitmaps(){
    DeleteObject(windowIcon);
    DeleteObject(speedometer);
    windowIcon = (HICON)LoadImageW(NULL,L"./resources/icon.ico",IMAGE_ICON,0,0,LR_LOADFROMFILE);
    speedometer[0] = (HBITMAP)LoadImageW(NULL,L"./resources/speedometer.bmp",IMAGE_BITMAP,windowWidth,windowHeight,LR_LOADFROMFILE);
    speedometer[1] = (HBITMAP)LoadImageW(NULL,L"./resources/speedometerCpu.bmp",IMAGE_BITMAP,windowWidth,windowHeight,LR_LOADFROMFILE);
}

void setRegion(HWND hwnd){
    DeleteObject(MainWindowRegn);
    MainWindowRegn = CreateEllipticRgn(0, 0, windowWidth,windowHeight);
    SetWindowRgn(hwnd, MainWindowRegn, TRUE);
}

void resizeWidget(HWND hwnd,const int& size){
    windowWidth = size;
    windowHeight = size;
    loadBitmaps();
    setRegion(hwnd);
    SendMessageW(hwnd,WM_MOUSEMOVE,MK_LBUTTON,0);
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
            resizeWidget(hwnd,150);
            break;
            case 4:
            resizeWidget(hwnd,250);
            break;
            case 5:
            resizeWidget(hwnd,400);
            break;
            case 6:
            view = 0;
            break;
            case 7:
            view = 1;
            break;
        }
        break;
        case WM_MOUSEMOVE:
        if(wp == MK_LBUTTON){
            move = true;
            POINT pt;
            GetCursorPos(&pt);
            SetWindowPos(hwnd,NULL,pt.x-(windowWidth/2),pt.y-(windowHeight/2),windowWidth,windowHeight,SWP_NOZORDER);
        }else{
            move = false;
        }
        break;
        /* case WM_PAINT:
        //drawPointer(hwnd);
        break; */
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


int WINAPI WinMain(HINSTANCE instance,HINSTANCE prevInstance,LPSTR argv,int ncmdShow){

    loadBitmaps();

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
    
    HWND mainWindow =  CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW ,L"mainWindowClass",L"",WS_VISIBLE | WS_POPUP,683,384,windowWidth,windowHeight,NULL,NULL,NULL,NULL);

    setRegion(mainWindow);

    MSG msg;

    while(quit == false){

        while(PeekMessageW(&msg,0,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        updateValue(mainWindow);
        if(move != true){
        Sleep(20);
        }
        drawPointer(mainWindow);
    }

     DestroyWindow(mainWindow);

    return 0;
}
