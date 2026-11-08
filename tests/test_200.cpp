#include <cassert>
#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <cwchar>
#include <fstream>
#include <sstream>
#include "../project/win_api.h"
#include "../project/core.h"
#include "../project/ui_manager.h"
#include "../project/effect_manager.h"
#include "../project/asset_manager.h"
#include "../save_system/save_manager.h"
extern "C" void asue_entry();
extern I64 procedure(HWND,U32,U64,I64);
extern UIManager ui;
extern GameManager game;
extern SaveManager saves;
extern EffectManager effects;
extern AssetManager assets;
extern int mouse_x,mouse_y,select_page;
extern bool board_cell(int,int,int&,int&);
extern void render();
extern void dispatch(int);
extern void simulate_step(int);
extern void load_levels();
extern bool graphics_ready;
extern int frames_painted;
static std::map<std::wstring,std::string> files;
struct File { std::wstring path; };
static int blits=0,bitmaps=0,media=0,messages=0;static int dc_serial=3,screen_presents=0,screen_draws=0,invalidations=0;
static bool shown=false,bitmap_failure=false;
static int client_width=1280,client_height=720;static unsigned long long clock_ms=100;
static WNDPROC wndproc=nullptr;
static bool integration_done=false;
static bool seen100=false,seen150=false;static bool capture_text=false;static std::vector<std::wstring> rendered_text;
static void paint(){wndproc((HWND)77,0xf,0,0);}
static void text_paint(){rendered_text.clear();capture_text=true;paint();capture_text=false;}
static bool has_text(const wchar_t*s){for(const auto&t:rendered_text)if(t.find(s)!=std::wstring::npos)return true;return false;}
static void click(int x,int y){wndproc((HWND)77,0x201,0,(I64(y)<<16)|x);}
static void integration(){
 assert(game.levels.count==200);
 for(int i=0;i<200;i++)assert(game.levels.data[i].id==i+1);
 saves.data.unlocked=150; assert(saves.save()); dispatch(1149);
 assert(game.level_id==150&&game.pots.level.id==150);
 game.pots.added=game.pots.level.waves;game.scores.score=game.pots.level.target_score;
 for(int i=0;i<300&&game.state==GameManager::PLAY;i++)simulate_step(17);
 assert(game.state==GameManager::WIN); SaveManager after150;after150.init();assert(after150.data.unlocked==151&&after150.data.completed[149]);
 paint();assert(ui.hit(470,270)==5);click(470,270);assert(game.level_id==151&&game.pots.level.id==151);
 saves=SaveManager();saves.init();dispatch(8);dispatch(1);assert(game.level_id==151);
 saves.data.unlocked=200;assert(saves.save());
 for(int id=151;id<=200;id++){
  dispatch(2);while(select_page<3)dispatch(401);while(select_page>3)dispatch(400);paint();
  int slot=id-151,x=410+(slot%10)*49,y=270+(slot/10)*49;
  assert(ui.count==52&&select_page==3&&ui.hit(x,y)==999+id);click(x,y);
  assert(game.level_id==id&&game.state==GameManager::PLAY&&game.pots.level.id==id);
 }
 game.pots.added=game.pots.level.waves;game.scores.score=game.pots.level.target_score;
 for(int i=0;i<300&&game.state==GameManager::PLAY;i++)simulate_step(17);
 assert(game.state==GameManager::WIN);paint();for(int i=0;i<ui.count;i++)assert(ui.buttons[i].action!=5);
 SaveManager after200;after200.init();assert(after200.data.unlocked==201&&after200.data.completed[199]);
 dispatch(8);dispatch(1);assert(game.level_id==200);
 integration_done=true;
}

struct Exit { int code; };
void fake_ExitProcess(U32 a0){throw Exit{(int)a0};}
extern "C" { decltype(__imp_ExitProcess) __imp_ExitProcess=fake_ExitProcess; }
HANDLE fake_GetModuleHandleW(LPCWSTR a0){return (HANDLE)1;}
extern "C" { decltype(__imp_GetModuleHandleW) __imp_GetModuleHandleW=fake_GetModuleHandleW; }
unsigned short fake_RegisterClassW(const WNDCLASSW* a0){wndproc=a0->proc;return 1;}
extern "C" { decltype(__imp_RegisterClassW) __imp_RegisterClassW=fake_RegisterClassW; }
HWND fake_CreateWindowExW(U32 a0, LPCWSTR a1, LPCWSTR a2, U32 a3, int a4, int a5, int a6, int a7, HWND a8, HANDLE a9, HANDLE a10, void* a11){assert(!(a3&0x10000000));return (HWND)77;}
extern "C" { decltype(__imp_CreateWindowExW) __imp_CreateWindowExW=fake_CreateWindowExW; }
int fake_ShowWindow(HWND a0, int a1){assert(graphics_ready&&ui.count==4);assert(!media);shown=true;return 1;}
extern "C" { decltype(__imp_ShowWindow) __imp_ShowWindow=fake_ShowWindow; }
int fake_PeekMessageW(MSG* a0, HWND a1, U32 a2, U32 a3, U32 a4){return 1;}
extern "C" { decltype(__imp_PeekMessageW) __imp_PeekMessageW=fake_PeekMessageW; }
int fake_TranslateMessage(const MSG* a0){return 1;}
extern "C" { decltype(__imp_TranslateMessage) __imp_TranslateMessage=fake_TranslateMessage; }
I64 fake_DispatchMessageW(const MSG* a0){return wndproc(a0->hwnd,a0->message,a0->wParam,a0->lParam);}
extern "C" { decltype(__imp_DispatchMessageW) __imp_DispatchMessageW=fake_DispatchMessageW; }
I64 fake_DefWindowProcW(HWND a0, U32 a1, U64 a2, I64 a3){return 1;}
extern "C" { decltype(__imp_DefWindowProcW) __imp_DefWindowProcW=fake_DefWindowProcW; }
void fake_PostQuitMessage(int a0){return;}
extern "C" { decltype(__imp_PostQuitMessage) __imp_PostQuitMessage=fake_PostQuitMessage; }
int fake_InvalidateRect(HWND a0, const RECT* a1, int a2){assert(!a2);invalidations++;return 1;}
extern "C" { decltype(__imp_InvalidateRect) __imp_InvalidateRect=fake_InvalidateRect; }
HDC fake_BeginPaint(HWND a0, PAINTSTRUCT* a1){a1->hdc=(HDC)2;return (HDC)2;}
extern "C" { decltype(__imp_BeginPaint) __imp_BeginPaint=fake_BeginPaint; }
int fake_EndPaint(HWND a0, const PAINTSTRUCT* a1){return 1;}
extern "C" { decltype(__imp_EndPaint) __imp_EndPaint=fake_EndPaint; }
int fake_GetClientRect(HWND a0, RECT* a1){*a1={0,0,client_width,client_height};return 1;}
extern "C" { decltype(__imp_GetClientRect) __imp_GetClientRect=fake_GetClientRect; }
int fake_SetWindowPos(HWND a0, HWND a1, int a2, int a3, int a4, int a5, U32 a6){return 1;}
extern "C" { decltype(__imp_SetWindowPos) __imp_SetWindowPos=fake_SetWindowPos; }
I64 fake_SetWindowLongPtrW(HWND a0, int a1, I64 a2){return 1;}
extern "C" { decltype(__imp_SetWindowLongPtrW) __imp_SetWindowLongPtrW=fake_SetWindowLongPtrW; }
HANDLE fake_LoadCursorW(HANDLE a0, LPCWSTR a1){return (HANDLE)1;}
extern "C" { decltype(__imp_LoadCursorW) __imp_LoadCursorW=fake_LoadCursorW; }
HDC fake_CreateCompatibleDC(HDC a0){return (HDC)(U64)dc_serial++;}
extern "C" { decltype(__imp_CreateCompatibleDC) __imp_CreateCompatibleDC=fake_CreateCompatibleDC; }
HANDLE fake_CreateCompatibleBitmap(HDC a0, int a1, int a2){assert(a0==(HDC)2);bitmaps++;return bitmap_failure?nullptr:(HANDLE)5;}
extern "C" { decltype(__imp_CreateCompatibleBitmap) __imp_CreateCompatibleBitmap=fake_CreateCompatibleBitmap; }
HGDIOBJ fake_SelectObject(HDC a0, HGDIOBJ a1){return (HANDLE)1;}
extern "C" { decltype(__imp_SelectObject) __imp_SelectObject=fake_SelectObject; }
int fake_DeleteObject(HGDIOBJ a0){return 1;}
extern "C" { decltype(__imp_DeleteObject) __imp_DeleteObject=fake_DeleteObject; }
int fake_DeleteDC(HDC a0){return 1;}
extern "C" { decltype(__imp_DeleteDC) __imp_DeleteDC=fake_DeleteDC; }
HANDLE fake_CreateSolidBrush(U32 a0){return (HANDLE)1;}
extern "C" { decltype(__imp_CreateSolidBrush) __imp_CreateSolidBrush=fake_CreateSolidBrush; }
HANDLE fake_CreatePen(int a0, int a1, U32 a2){return (HANDLE)1;}
extern "C" { decltype(__imp_CreatePen) __imp_CreatePen=fake_CreatePen; }
int fake_Rectangle(HDC a0, int a1, int a2, int a3, int a4){assert(a0!=(HDC)2);return 1;}
extern "C" { decltype(__imp_Rectangle) __imp_Rectangle=fake_Rectangle; }
int fake_RoundRect(HDC a0, int a1, int a2, int a3, int a4, int a5, int a6){return 1;}
extern "C" { decltype(__imp_RoundRect) __imp_RoundRect=fake_RoundRect; }
int fake_Ellipse(HDC a0, int a1, int a2, int a3, int a4){return 1;}
extern "C" { decltype(__imp_Ellipse) __imp_Ellipse=fake_Ellipse; }
int fake_MoveToEx(HDC a0, int a1, int a2, POINT* a3){return 1;}
extern "C" { decltype(__imp_MoveToEx) __imp_MoveToEx=fake_MoveToEx; }
int fake_LineTo(HDC a0, int a1, int a2){return 1;}
extern "C" { decltype(__imp_LineTo) __imp_LineTo=fake_LineTo; }
int fake_TextOutW(HDC a0, int a1, int a2, LPCWSTR a3, int a4){if(a4==3&&a3[0]==L'1'&&a3[1]==L'0'&&a3[2]==L'0'){assert(a1==839&&a2==458);seen100=true;}if(a4==3&&a3[0]==L'1'&&a3[1]==L'5'&&a3[2]==L'0'){assert(a1==839&&a2==458);seen150=true;}if(capture_text)rendered_text.emplace_back(a3,a4);return 1;}
extern "C" { decltype(__imp_TextOutW) __imp_TextOutW=fake_TextOutW; }
U32 fake_SetTextColor(HDC a0, U32 a1){return 1;}
extern "C" { decltype(__imp_SetTextColor) __imp_SetTextColor=fake_SetTextColor; }
int fake_SetBkMode(HDC a0, int a1){return 1;}
extern "C" { decltype(__imp_SetBkMode) __imp_SetBkMode=fake_SetBkMode; }
HANDLE fake_CreateFontW(int a0, int a1, int a2, int a3, int a4, U32 a5, U32 a6, U32 a7, U32 a8, U32 a9, U32 a10, U32 a11, U32 a12, LPCWSTR a13){return (HANDLE)1;}
extern "C" { decltype(__imp_CreateFontW) __imp_CreateFontW=fake_CreateFontW; }
int fake_StretchBlt(HDC a0, int a1, int a2, int a3, int a4, HDC a5, int a6, int a7, int a8, int a9, U32 a10){assert(a0!=(HDC)2);assert(a5!=nullptr);assert(ui.count>0);blits++;return 1;}
extern "C" { decltype(__imp_StretchBlt) __imp_StretchBlt=fake_StretchBlt; }
int fake_StretchDIBits(HDC a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, const void* a9, const void* a10, U32 a11, U32 a12){return 1;}
extern "C" { decltype(__imp_StretchDIBits) __imp_StretchDIBits=fake_StretchDIBits; }
HANDLE fake_LoadImageW(HANDLE a0, LPCWSTR a1, U32 a2, int a3, int a4, U32 a5){return nullptr;}
extern "C" { decltype(__imp_LoadImageW) __imp_LoadImageW=fake_LoadImageW; }
int fake_QueryPerformanceCounter(LARGE_INTEGER* a0){return 1;}
extern "C" { decltype(__imp_QueryPerformanceCounter) __imp_QueryPerformanceCounter=fake_QueryPerformanceCounter; }
int fake_QueryPerformanceFrequency(LARGE_INTEGER* a0){return 1;}
extern "C" { decltype(__imp_QueryPerformanceFrequency) __imp_QueryPerformanceFrequency=fake_QueryPerformanceFrequency; }
void fake_Sleep(U32 a0){return;}
extern "C" { decltype(__imp_Sleep) __imp_Sleep=fake_Sleep; }
U32 fake_GetModuleFileNameW(HANDLE a0, wchar_t* a1, U32 a2){std::wcscpy(a1,L"C:\\test\\ASuePotGame.exe");return std::wcslen(a1);}
extern "C" { decltype(__imp_GetModuleFileNameW) __imp_GetModuleFileNameW=fake_GetModuleFileNameW; }
U32 fake_GetEnvironmentVariableW(LPCWSTR a0, wchar_t* a1, U32 a2){std::wcscpy(a1,L"APPDATA");return 7;}
extern "C" { decltype(__imp_GetEnvironmentVariableW) __imp_GetEnvironmentVariableW=fake_GetEnvironmentVariableW; }
int fake_CreateDirectoryW(LPCWSTR a0, void* a1){return 1;}
extern "C" { decltype(__imp_CreateDirectoryW) __imp_CreateDirectoryW=fake_CreateDirectoryW; }
HANDLE fake_CreateFileW(LPCWSTR a0, U32 a1, U32 a2, void* a3, U32 a4, U32 a5, HANDLE a6){if(a4==3&&!files.count(a0))return (HANDLE)-1;if(a4==2)files[a0]="";return new File{a0};}
extern "C" { decltype(__imp_CreateFileW) __imp_CreateFileW=fake_CreateFileW; }
int fake_ReadFile(HANDLE a0, void* a1, U32 a2, U32* a3, void* a4){auto&s=files[((File*)a0)->path];*a3=s.size()<a2?s.size():a2;for(unsigned i=0;i<*a3;i++)((char*)a1)[i]=s[i];return 1;}
extern "C" { decltype(__imp_ReadFile) __imp_ReadFile=fake_ReadFile; }
int fake_WriteFile(HANDLE a0, const void* a1, U32 a2, U32* a3, void* a4){files[((File*)a0)->path]+=std::string((const char*)a1,a2);*a3=a2;return 1;}
extern "C" { decltype(__imp_WriteFile) __imp_WriteFile=fake_WriteFile; }
int fake_FlushFileBuffers(HANDLE a0){return 1;}
extern "C" { decltype(__imp_FlushFileBuffers) __imp_FlushFileBuffers=fake_FlushFileBuffers; }
int fake_CloseHandle(HANDLE a0){delete (File*)a0;return 1;}
extern "C" { decltype(__imp_CloseHandle) __imp_CloseHandle=fake_CloseHandle; }
int fake_MoveFileExW(LPCWSTR a0, LPCWSTR a1, U32 a2){files[a1]=files[a0];files.erase(a0);return 1;}
extern "C" { decltype(__imp_MoveFileExW) __imp_MoveFileExW=fake_MoveFileExW; }
int fake_CopyFileW(LPCWSTR a0, LPCWSTR a1, int a2){files[a1]=files[a0];return 1;}
extern "C" { decltype(__imp_CopyFileW) __imp_CopyFileW=fake_CopyFileW; }
int fake_MessageBoxW(HWND a0, LPCWSTR a1, LPCWSTR a2, U32 a3){return 1;}
extern "C" { decltype(__imp_MessageBoxW) __imp_MessageBoxW=fake_MessageBoxW; }
int fake_GetSystemMetrics(int a0){return a0?1080:1920;}
extern "C" { decltype(__imp_GetSystemMetrics) __imp_GetSystemMetrics=fake_GetSystemMetrics; }
int fake_PlaySoundW(LPCWSTR a0, HANDLE a1, U32 a2){return 1;}
extern "C" { decltype(__imp_PlaySoundW) __imp_PlaySoundW=fake_PlaySoundW; }
U32 fake_mciSendStringW(LPCWSTR a0, wchar_t* a1, U32 a2, HWND a3){media++;return 0;}
extern "C" { decltype(__imp_mciSendStringW) __imp_mciSendStringW=fake_mciSendStringW; }
U32 fake_waveOutSetVolume(HANDLE a0, U32 a1){return 1;}
extern "C" { decltype(__imp_waveOutSetVolume) __imp_waveOutSetVolume=fake_waveOutSetVolume; }
int fake_TransparentBlt(HDC a0, int a1, int a2, int a3, int a4, HDC a5, int a6, int a7, int a8, int a9, U32 a10){assert(a3==50&&a4==42&&a8==58&&a9==49);return 1;}
extern "C" { decltype(__imp_TransparentBlt) __imp_TransparentBlt=fake_TransparentBlt; }
HDC fake_GetDC(HWND a0){assert(!shown);return (HDC)2;}
extern "C" { decltype(__imp_GetDC) __imp_GetDC=fake_GetDC; }
int fake_ReleaseDC(HWND a0, HDC a1){return 1;}
extern "C" { decltype(__imp_ReleaseDC) __imp_ReleaseDC=fake_ReleaseDC; }
int fake_UpdateWindow(HWND a0){assert(shown);wndproc(a0,0xf,0,0);assert(ui.count==4);return 1;}
extern "C" { decltype(__imp_UpdateWindow) __imp_UpdateWindow=fake_UpdateWindow; }
int fake_GetMessageW(MSG* a0, HWND a1, U32 a2, U32 a3){assert(shown&&blits>=1);if(messages==0){assert(ui.count==4);a0->message=0x201;a0->lParam=(300ll<<16)|470;a0->wParam=0;}
 else if(messages==1){assert(game.state==GameManager::PLAY);a0->message=0x113;a0->wParam=1;}
 else if(messages==2){a0->message=0xf;a0->wParam=0;}
 else if(messages==3){a0->message=0x100;a0->wParam=27;}
 else if(messages==4){assert(game.state==GameManager::PAUSE);integration();game.state=GameManager::PAUSE;int old=invalidations;wndproc((HWND)77,0x113,1,0);assert(invalidations==old);int bm=bitmaps,bp=screen_presents;for(int i=0;i<500;i++)wndproc((HWND)77,0xf,0,0);assert(bitmaps==bm&&screen_presents==bp+500);client_width=1024;client_height=768;wndproc((HWND)77,0x5,0,0);wndproc((HWND)77,0xf,0,0);assert(bitmaps==bm+1);wndproc((HWND)77,0x200,0,(608ll<<16)|954);int cx,cy;assert(board_cell(mouse_x,mouse_y,cx,cy)&&cx==14&&cy==0);return 0;}
 messages++;a0->hwnd=(HWND)77;return 1;}
extern "C" { decltype(__imp_GetMessageW) __imp_GetMessageW=fake_GetMessageW; }
U64 fake_SetTimer(HWND a0, U64 a1, U32 a2, void* a3){assert(shown&&frames_painted>=1);int old=invalidations;wndproc(a0,0x113,1,0);assert(invalidations==old);return 1;}
extern "C" { decltype(__imp_SetTimer) __imp_SetTimer=fake_SetTimer; }
int fake_KillTimer(HWND a0, U64 a1){return 1;}
extern "C" { decltype(__imp_KillTimer) __imp_KillTimer=fake_KillTimer; }
U64 fake_GetTickCount64(){clock_ms+=17;return clock_ms;}
extern "C" { decltype(__imp_GetTickCount64) __imp_GetTickCount64=fake_GetTickCount64; }

int fake_BitBlt(HDC dest,int,int,int,int,HDC src,int,int,U32){assert(dest==(HDC)2&&src!=(HDC)2);screen_presents++;return 1;}
extern "C" { decltype(__imp_BitBlt) __imp_BitBlt=fake_BitBlt; }
int main(){
 for(int i=1;i<=200;i++){char local[80];sprintf(local,"ASuePotGame/levels/level_%02d.json",i);std::ifstream input(local);assert(input);std::stringstream bytes;bytes<<input.rdbuf();wchar_t suffix[30];std::swprintf(suffix,30,L"level_%02d.json",i);files[std::wstring(L"C:\\test\\levels\\")+suffix]=bytes.str();}

 std::ifstream manifest_file("ASuePotGame/levels/campaign.json");assert(manifest_file);std::stringstream manifest_bytes;manifest_bytes<<manifest_file.rdbuf();files[L"C:\\test\\levels\\campaign.json"]=manifest_bytes.str();

 // Windows 路径字符串按运行代码输出匹配。

 try{asue_entry();}catch(Exit e){assert(e.code==0);}
 assert(integration_done&&blits>=2&&bitmaps==3&&messages==4&&media==0&&screen_presents>=2);

 bitmap_failure=true;shown=false;graphics_ready=false;frames_painted=0;bool failed=false;try{asue_entry();}catch(Exit e){assert(e.code==1);failed=true;}assert(failed&&!shown);
 printf("PASS: 200 levels loaded; 150 completion unlocks and resumes 151; 151-200 clickable on page 4; 200 completion persists and Continue clamps to 200. Mock Win32 only.\n");
}
