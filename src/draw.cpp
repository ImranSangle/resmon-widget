
#include <windows.h>

extern HBITMAP speedometer[2];
extern int x;
extern int y;
extern int view;
extern float windowWidth;

void drawPointer(HWND hwnd){

    HDC hdc = GetDC(hwnd);

    BITMAP bm;
    GetObject(speedometer[view],sizeof(bm),&bm);

    HDC tempDc = CreateCompatibleDC(hdc);
    HBITMAP bitmap =(HBITMAP) SelectObject(tempDc,speedometer[view]);

    HDC frontDc = CreateCompatibleDC(hdc);
    HBITMAP frontMap = CreateCompatibleBitmap(hdc,bm.bmWidth,bm.bmHeight);
    SelectObject(frontDc,frontMap);
    
    BitBlt(frontDc,0,0,bm.bmWidth,bm.bmHeight,tempDc,0,0,SRCCOPY);

    HPEN pen = CreatePen(PS_SOLID,(windowWidth/150)+1,RGB(255,0,0));
    HPEN oldPen =(HPEN) SelectObject(frontDc,pen);

    MoveToEx(frontDc,bm.bmWidth/2,bm.bmHeight/2,NULL);
    LineTo(frontDc,x,y);

    BitBlt(hdc,0,0,bm.bmWidth,bm.bmHeight,frontDc,0,0,SRCCOPY);

    SelectObject(frontDc,oldPen);
    DeleteObject(pen);
    DeleteDC(tempDc);
    DeleteDC(frontDc);
    DeleteObject(frontMap);
    ReleaseDC(hwnd,hdc);

}