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
 assert(game.levels.count==150);
 dispatch(8);dispatch(2);paint();assert(ui.hit(460,265)==0&&ui.hit(845,460)==0);int prior=game.level_id;dispatch(1001);assert(game.level_id==prior);
 dispatch(401);text_paint();assert(select_page==1&&has_text(L"页 2 / 3")&&ui.hit(410,270)==0&&ui.hit(845,460)==0);dispatch(1099);assert(game.level_id==prior);
 click(800,590);text_paint();assert(select_page==2&&has_text(L"页 3 / 3")&&ui.hit(410,270)==0&&ui.hit(845,460)==0);dispatch(1149);assert(game.level_id==prior);dispatch(401);assert(select_page==2);dispatch(400);paint();assert(ui.hit(410,590)==400&&ui.hit(800,590)==401);dispatch(400);paint();
 // Real game/menu/save path: completing1 immediately unlocks2 and persists.
 dispatch(8);dispatch(1);assert(game.pots.level.columns==15&&!game.motion_ms&&!effects.moving);int initial_elapsed=effects.elapsed;wndproc((HWND)77,0x100,27,0);simulate_step(17);assert(effects.elapsed==initial_elapsed&&!game.motion_ms);wndproc((HWND)77,0x100,27,0);game.state=GameManager::WIN;simulate_step(17);SaveManager after1;after1.init();assert(after1.data.unlocked==2);
 // Left click on scenery when no moves triggers falling pots; repeated clicks/Space keep origin snapshot.
 game.pots.reset(game.pots.level);game.pots.added=1;game.state=GameManager::PLAY;game.anim=game.motion_ms=0;paint();click(700,300);assert(game.rain_count==1&&effects.raining&&effects.moving&&game.pots.has_moves());assert(effects.previous[12][0].type<0);click(700,300);wndproc((HWND)77,0x100,32,0);assert(effects.previous[12][0].type<0);
 int ms=game.motion_ms;wndproc((HWND)77,0x100,27,0);simulate_step(17);assert(game.motion_ms==ms);wndproc((HWND)77,0x100,27,0);for(int i=0;i<50;i++)simulate_step(17);assert(!game.motion_ms&&!effects.moving);
 // Verify physical scene clicks with another valid group present in every fixture.
 auto fixture=[](){game.pots.reset(game.pots.level);game.pots.added=1;game.anim=game.motion_ms=game.wave_clock=game.rain_count=0;game.scores.score=0;effects=EffectManager();game.pots.grid[14][0]={0,0,++game.pots.serial};game.pots.grid[14][1]={0,1,++game.pots.serial};game.pots.grid[12][0]={1,0,++game.pots.serial};paint();assert(game.pots.has_moves());};
 fixture();click(1189,641);assert(game.anim>0&&game.rain_count==0);for(int i=0;i<50;i++)simulate_step(17);assert(game.scores.score==20&&game.pots.count()==1);
 fixture();click(1073,641);assert(!game.anim&&game.rain_count==1&&game.pots.grid[12][0].type==1&&game.pots.grid[14][0].type==0&&game.scores.score==0);
 fixture();click(700,300);assert(game.rain_count==1&&effects.raining);fixture();click(320,250);assert(game.rain_count==1&&effects.raining);
 fixture();click(700,20);assert(game.rain_count==0&&game.anim==0);click(120,655);assert(game.rain_count==0&&game.pots.added==2&&!effects.raining&&effects.moving);int motion=game.motion_ms;click(1189,641);assert(game.rain_count==0&&game.anim==0&&game.motion_ms==motion);
 fixture();game.wave_clock=game.wave_interval()-17;simulate_step(17);assert(game.pots.added==2&&effects.moving&&!effects.raining&&effects.previous[14][0].uid==1);int ax,ay;effects.position(game.pots.grid[14][0],14,0,BoardLayout(15),ax,ay);assert(ax>1168&&ax<1218);simulate_step(game.pots.level.move_ms);assert(!effects.moving&&!game.motion_ms);
 fixture();game.pots.added=game.pots.level.waves;paint();click(120,655);assert(game.rain_count==0&&game.anim==0);
 // Reach20 via completion records, then fail, click Retry, close/reload save and click20 again.
 for(int id=2;id<20;id++)saves.complete(id,id*100);dispatch(1019);assert(game.level_id==20);while(game.state==GameManager::PLAY)simulate_step(17);assert(game.state==GameManager::FAIL);paint();click(470,270);assert(game.level_id==20&&game.state==GameManager::PLAY&&game.pots.added==1);
 assert(saves.save());saves=SaveManager();saves.init();assert(saves.data.unlocked==20);dispatch(2);paint();assert(ui.hit(845,310)==1019);click(845,310);assert(game.level_id==20&&game.state==GameManager::PLAY);
 game.state=GameManager::WIN;simulate_step(17);SaveManager after20;after20.init();assert(after20.data.unlocked==21&&after20.data.completed[19]);
 // A previous20-stage save can now continue at21 without resetting progress.
 dispatch(8);dispatch(1);assert(game.level_id==21&&game.state==GameManager::PLAY);dispatch(2);paint();assert(ui.hit(845,460)==0);
 // Unlock50 by completing49, fail and click Retry50, reload and select50, then complete campaign.
 for(int id=21;id<=49;id++)saves.complete(id,id*100);SaveManager after49;after49.init();assert(after49.data.unlocked==50&&after49.data.completed[48]);
 dispatch(1049);assert(game.level_id==50&&game.pots.level.target_score>0);while(game.state==GameManager::PLAY)simulate_step(17);assert(game.state==GameManager::FAIL);paint();click(470,270);assert(game.level_id==50&&game.state==GameManager::PLAY);
 saves.save();saves=SaveManager();saves.init();dispatch(2);paint();assert(ui.count==52&&ui.hit(845,460)==1049);click(845,460);assert(game.level_id==50&&game.state==GameManager::PLAY);
 game.pots.added=game.pots.level.waves;game.scores.score=game.pots.level.target_score;for(int i=0;i<300&&game.state==GameManager::PLAY;i++)simulate_step(17);assert(game.state==GameManager::WIN);paint();assert(ui.hit(470,270)==5);
 SaveManager finished50;finished50.init();assert(finished50.data.unlocked==51&&finished50.data.completed[49]&&finished50.data.best[49]==game.pots.level.target_score);
 // Completing the previous50-stage version immediately resumes the new51, via Next and restart.
 click(470,270);assert(game.level_id==51&&game.state==GameManager::PLAY);
 saves.data.music=40;saves.data.sfx=20;saves.data.fullscreen=true;saves.data.width=1920;saves.data.height=1080;assert(saves.save());saves=SaveManager();saves.init();dispatch(8);dispatch(1);assert(game.level_id==51&&game.state==GameManager::PLAY);
 dispatch(2);text_paint();assert(select_page==1&&has_text(L"共 150 关")&&has_text(L"页 2 / 3")&&ui.hit(410,270)==1050&&ui.hit(460,270)==0);prior=game.level_id;dispatch(1051);assert(game.level_id==prior);dispatch(401);assert(select_page==2);dispatch(400);dispatch(400);assert(select_page==0);
 // A legacy completed80 save resumes81 with its historical scores and settings intact.
 for(int id=51;id<=80;id++)saves.complete(id,id*100);saves=SaveManager();saves.init();dispatch(8);dispatch(1);assert(game.level_id==81&&saves.data.unlocked==81&&saves.data.completed[79]&&saves.data.best[79]==8000);prior=game.level_id;dispatch(1081);assert(game.level_id==prior);
 // Reach100, fail, click Retry, restart the save, and select100 on page2.
 for(int id=81;id<=99;id++)saves.complete(id,id*100);SaveManager after99;after99.init();assert(after99.data.unlocked==100&&after99.data.completed[98]);
 dispatch(1099);assert(game.level_id==100&&game.pots.level.id==100&&game.pots.level.target_score>0);while(game.state==GameManager::PLAY)simulate_step(17);assert(game.state==GameManager::FAIL);paint();click(470,270);assert(game.level_id==100&&game.state==GameManager::PLAY&&game.pots.added==1);
 assert(saves.save());saves=SaveManager();saves.init();assert(saves.data.unlocked==100);dispatch(2);text_paint();assert(select_page==1&&ui.count==53&&ui.hit(845,460)==1099&&has_text(L"100")&&seen100);click(845,460);assert(game.level_id==100&&game.state==GameManager::PLAY);
 game.pots.added=game.pots.level.waves;game.scores.score=game.pots.level.target_score;for(int i=0;i<300&&game.state==GameManager::PLAY;i++)simulate_step(17);assert(game.state==GameManager::WIN);paint();assert(ui.hit(470,270)==5);
 SaveManager finished100;finished100.init();assert(finished100.data.unlocked==101&&finished100.data.completed[99]&&finished100.data.best[99]==game.pots.level.target_score&&finished100.data.best[79]==8000&&finished100.data.best[49]==finished50.data.best[49]);
 // The previous100-stage campaign now continues101 through Next and save restart.
 click(470,270);assert(game.level_id==101&&game.state==GameManager::PLAY);assert(saves.save());saves=SaveManager();saves.init();dispatch(8);dispatch(1);assert(game.level_id==101&&game.state==GameManager::PLAY);
 dispatch(2);text_paint();assert(select_page==2&&has_text(L"页 3 / 3")&&ui.hit(410,270)==1100&&ui.hit(460,270)==0);prior=game.level_id;dispatch(1101);assert(game.level_id==prior);
 // Unlock150, fail, click Retry, restart and physically choose150 on page3.
 for(int id=101;id<=149;id++)saves.complete(id,id*100);SaveManager after149;after149.init();assert(after149.data.unlocked==150&&after149.data.completed[148]);
 dispatch(1149);assert(game.level_id==150&&game.pots.level.id==150&&game.pots.level.target_score>0);while(game.state==GameManager::PLAY)simulate_step(17);assert(game.state==GameManager::FAIL);paint();click(470,270);assert(game.level_id==150&&game.state==GameManager::PLAY&&game.pots.added==1);
 assert(saves.save());saves=SaveManager();saves.init();assert(saves.data.unlocked==150);dispatch(2);text_paint();assert(select_page==2&&ui.count==52&&ui.hit(845,460)==1149&&has_text(L"150")&&seen150);click(845,460);assert(game.level_id==150&&game.state==GameManager::PLAY);
 game.pots.added=game.pots.level.waves;game.scores.score=game.pots.level.target_score;for(int i=0;i<300&&game.state==GameManager::PLAY;i++)simulate_step(17);assert(game.state==GameManager::WIN);paint();for(int i=0;i<ui.count;i++)assert(ui.buttons[i].action!=5);dispatch(5);assert(game.level_id==150&&game.state==GameManager::WIN);
 SaveManager finished150;finished150.init();assert(finished150.data.unlocked==151&&finished150.data.completed[149]&&finished150.data.best[149]==game.pots.level.target_score&&finished150.data.best[99]==finished100.data.best[99]&&finished150.data.best[79]==8000&&finished150.data.music==40&&finished150.data.sfx==20&&finished150.data.fullscreen&&finished150.data.width==1920&&finished150.data.height==1080);dispatch(8);dispatch(1);assert(game.level_id==150&&game.state==GameManager::PLAY);
 // All150 authored buttons load correct JSON across3 pages; middle page uses53 slots.
 for(int id=1;id<=150;id++){dispatch(2);int page=(id-1)/50;while(select_page<page)dispatch(401);while(select_page>page)dispatch(400);paint();int slot=(id-1)%50,x=410+(slot%10)*49,y=270+(slot/10)*49;assert(ui.count==(page==1?53:52)&&select_page==page&&ui.hit(x,y)==999+id);click(x,y);assert(game.level_id==id&&game.state==GameManager::PLAY&&game.pots.level.id==id);}
 // A stale151 is excluded; its declared count exposes page4 without engine changes.
 auto virtual151=std::wstring(L"C:\\test\\levels\\level_151.json");auto manifest=std::wstring(L"C:\\test\\levels\\campaign.json");auto original_manifest=files[manifest];files[virtual151]=files[L"C:\\test\\levels\\level_150.json"];load_levels();assert(game.levels.count==150);
 files[manifest]="{\"level_count\":151}";load_levels();assert(game.levels.count==151);dispatch(2);dispatch(401);text_paint();assert(select_page==3&&has_text(L"页 4 / 4")&&ui.hit(410,270)==1150);click(410,270);assert(game.level_id==151&&game.state==GameManager::PLAY);
 files.erase(virtual151);files[manifest]=original_manifest;load_levels();assert(game.levels.count==150);dispatch(2);paint();assert(select_page==2);dispatch(8);dispatch(1);assert(game.level_id==150);prior=game.level_id;dispatch(1150);assert(game.level_id==prior);
 // Visible score rules, live hover preview and goal/settlement status must use
 // the same calculation as the actual attack, including the new combo stages.
 dispatch(1000);assert(game.level_id==1);game.motion_ms=0;effects=EffectManager();game.pots.reset(game.pots.level);game.pots.added=1;int rx=game.pots.level.columns-1;
 for(int y=0;y<4;y++)game.pots.grid[rx][y]={0,0,++game.pots.serial};
 int px=468+rx*50+25;wndproc((HWND)77,0x200,0,(641ll<<16)|px);text_paint();
 assert(has_text(L"150关进阶版 v1.1")&&has_text(L"场地 15 列")&&has_text(L"大组合有额外奖励")&&has_text(L"当前组合 4 罐 · 120 分")&&has_text(L"还差 20000 分"));
 // Optional BMPs keep the whole58x49 source when scaled to the15-column cells.
 assets.pots[0]=(HANDLE)9;paint();assets.pots[0]=0;
 click(px,641);for(int i=0;i<50;i++)simulate_step(17);assert(game.scores.score==120);text_paint();assert(has_text(L"还差 19880 分"));
 game.scores.score=game.pots.level.target_score;text_paint();assert(has_text(L"已达成，仍需完成进列"));simulate_step(17);assert(game.state==GameManager::PLAY);
 game.pots.added=game.pots.level.waves;game.end_clock=0;text_paint();assert(has_text(L"进列完成 · 结算 3 秒"));
 wndproc((HWND)77,0x100,27,0);int end=game.end_clock;simulate_step(1000);assert(game.end_clock==end);wndproc((HWND)77,0x100,27,0);
 // Existingv0.3 serialized progress/settings remains unchanged when opening new game.
 assert(after20.data.completed[0]&&after20.data.best[18]==1900);
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
 for(int i=1;i<=150;i++){char local[80];sprintf(local,"ASuePotGame/levels/level_%02d.json",i);std::ifstream input(local);assert(input);std::stringstream bytes;bytes<<input.rdbuf();wchar_t suffix[30];std::swprintf(suffix,30,L"level_%02d.json",i);files[std::wstring(L"C:\\test\\levels\\")+suffix]=bytes.str();}

 // Preserve historical 150-stage behavior as a separate regression scenario.
 files[L"C:\\test\\levels\\campaign.json"]="{\"level_count\":150}";

 // Windows 路径字符串按运行代码输出匹配。

 try{asue_entry();}catch(Exit e){assert(e.code==0);}
 assert(integration_done&&blits>=2&&bitmaps==3&&messages==4&&media==0&&screen_presents>=2);

 bitmap_failure=true;shown=false;graphics_ready=false;frames_painted=0;bool failed=false;try{asue_entry();}catch(Exit e){assert(e.code==1);failed=true;}assert(failed&&!shown);
 printf("PASS: hidden window -> screen-compatible bitmap -> menu built -> ShowWindow -> first WM_PAINT blit -> timer -> click Start -> pause; no media calls during startup. graphics allocation failure exits before showing blank window. 500 stable paints reuse buffers; visible window receives one final blit per paint, no direct clear; menu/pause timer does not invalidate. Actual150 JSON levels; valid group click attacks, isolated pot/empty cell/background click rains despite other matches, HUD/NEXT excluded, animation guards; level1 unlock2; locked buttons; level20 failure -> Retry click -> save reload -> level20 selection click -> completion unlock21; old save continues21; completing49 unlocks50; level50 failure/retry/reload/selection; new first stage matches old50 baseline; all150 buttons across3 pages load, middle page53 buttons; completed50 resumes51 and completed80 resumes81 with progress/settings; level100 failure/retry/restart/selection and completed100 resumes101; level150 failure/retry/restart/selection, completion stores151 and Continue clamps150; stale151 excluded, declared151 extends to a fourth page without core edits; three-digit100/150 labels centered. Original linear slide and immediate initial column. Mock Win32 only.\n");
}
