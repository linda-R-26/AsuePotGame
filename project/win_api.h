#pragma once
// 仅声明本项目使用的稳定 Win32 API；无需外部运行库。
using U32=unsigned int;using U64=unsigned long long;using I64=long long;using HANDLE=void*;using HWND=void*;using HDC=void*;using HGDIOBJ=void*;using LPCWSTR=const wchar_t*;
struct POINT{int x,y;};struct RECT{int left,top,right,bottom;};struct MSG{HWND hwnd;U32 message;U64 wParam;I64 lParam;U32 time;POINT pt;U32 lPrivate;};
struct PAINTSTRUCT{HDC hdc;int erase;RECT paint;int restore,inc;unsigned char reserved[32];};
using WNDPROC=I64(*)(HWND,U32,U64,I64);
struct WNDCLASSW{U32 style;WNDPROC proc;int cls_extra,wnd_extra;HANDLE instance,icon,cursor,brush;LPCWSTR menu,name;};
struct LARGE_INTEGER{I64 value;};
#define API(ret,name,args) extern "C" ret (*__imp_##name) args
API(void,ExitProcess,(U32));API(HANDLE,GetModuleHandleW,(LPCWSTR));API(unsigned short,RegisterClassW,(const WNDCLASSW*));API(HWND,CreateWindowExW,(U32,LPCWSTR,LPCWSTR,U32,int,int,int,int,HWND,HANDLE,HANDLE,void*));API(int,ShowWindow,(HWND,int));API(int,PeekMessageW,(MSG*,HWND,U32,U32,U32));API(int,TranslateMessage,(const MSG*));API(I64,DispatchMessageW,(const MSG*));API(I64,DefWindowProcW,(HWND,U32,U64,I64));API(void,PostQuitMessage,(int));API(int,InvalidateRect,(HWND,const RECT*,int));API(HDC,BeginPaint,(HWND,PAINTSTRUCT*));API(int,EndPaint,(HWND,const PAINTSTRUCT*));API(int,GetClientRect,(HWND,RECT*));API(int,SetWindowPos,(HWND,HWND,int,int,int,int,U32));API(I64,SetWindowLongPtrW,(HWND,int,I64));API(HANDLE,LoadCursorW,(HANDLE,LPCWSTR));
API(HDC,CreateCompatibleDC,(HDC));API(HANDLE,CreateCompatibleBitmap,(HDC,int,int));API(HGDIOBJ,SelectObject,(HDC,HGDIOBJ));API(int,DeleteObject,(HGDIOBJ));API(int,DeleteDC,(HDC));API(HANDLE,CreateSolidBrush,(U32));API(HANDLE,CreatePen,(int,int,U32));API(int,Rectangle,(HDC,int,int,int,int));API(int,RoundRect,(HDC,int,int,int,int,int,int));API(int,Ellipse,(HDC,int,int,int,int));API(int,MoveToEx,(HDC,int,int,POINT*));API(int,LineTo,(HDC,int,int));API(int,TextOutW,(HDC,int,int,LPCWSTR,int));API(U32,SetTextColor,(HDC,U32));API(int,SetBkMode,(HDC,int));API(HANDLE,CreateFontW,(int,int,int,int,int,U32,U32,U32,U32,U32,U32,U32,U32,LPCWSTR));API(int,StretchBlt,(HDC,int,int,int,int,HDC,int,int,int,int,U32));API(int,StretchDIBits,(HDC,int,int,int,int,int,int,int,int,const void*,const void*,U32,U32));API(HANDLE,LoadImageW,(HANDLE,LPCWSTR,U32,int,int,U32));
API(int,QueryPerformanceCounter,(LARGE_INTEGER*));API(int,QueryPerformanceFrequency,(LARGE_INTEGER*));API(void,Sleep,(U32));API(U32,GetModuleFileNameW,(HANDLE,wchar_t*,U32));API(U32,GetEnvironmentVariableW,(LPCWSTR,wchar_t*,U32));API(int,CreateDirectoryW,(LPCWSTR,void*));API(HANDLE,CreateFileW,(LPCWSTR,U32,U32,void*,U32,U32,HANDLE));API(int,ReadFile,(HANDLE,void*,U32,U32*,void*));API(int,WriteFile,(HANDLE,const void*,U32,U32*,void*));API(int,FlushFileBuffers,(HANDLE));API(int,CloseHandle,(HANDLE));API(int,MoveFileExW,(LPCWSTR,LPCWSTR,U32));API(int,CopyFileW,(LPCWSTR,LPCWSTR,int));API(int,MessageBoxW,(HWND,LPCWSTR,LPCWSTR,U32));API(int,GetSystemMetrics,(int));
#undef API
extern "C" int (*__imp_PlaySoundW)(LPCWSTR,HANDLE,U32);
extern "C" U32 (*__imp_mciSendStringW)(LPCWSTR,wchar_t*,U32,HWND);
extern "C" U32 (*__imp_waveOutSetVolume)(HANDLE,U32);
extern "C" int (*__imp_TransparentBlt)(HDC,int,int,int,int,HDC,int,int,int,int,U32);
extern "C" HDC (*__imp_GetDC)(HWND);
extern "C" int (*__imp_ReleaseDC)(HWND,HDC);
extern "C" int (*__imp_UpdateWindow)(HWND);
extern "C" int (*__imp_GetMessageW)(MSG*,HWND,U32,U32);
extern "C" U64 (*__imp_SetTimer)(HWND,U64,U32,void*);
extern "C" int (*__imp_KillTimer)(HWND,U64);
extern "C" U64 (*__imp_GetTickCount64)();

extern "C" int (*__imp_BitBlt)(HDC,int,int,int,int,HDC,int,int,U32);
