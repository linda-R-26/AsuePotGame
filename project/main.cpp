#include "core.h"
#include "ui_manager.h"
#include "settings_manager.h"
#include "audio_manager.h"
#include "player_controller.h"
#include "effect_manager.h"
#include "asset_manager.h"
#include "level_loader.h"
extern "C" { int _fltused=0; }
extern "C" void* memset(void*d,int v,unsigned long long n){auto*p=(unsigned char*)d;for(U64 i=0;i<n;i++)p[i]=(unsigned char)v;return d;}
extern "C" void* memcpy(void*d,const void*s,unsigned long long n){auto*p=(unsigned char*)d;auto*q=(const unsigned char*)s;for(U64 i=0;i<n;i++)p[i]=q[i];return d;}
GameManager game;SaveManager saves;UIManager ui;SettingsManager settings;AudioManager audio;PlayerController player;EffectManager effects;AssetManager assets;
HWND window;HDC buffer;HDC present_buffer=0;HANDLE present_bitmap=0,present_old_bitmap=0;int present_width=0,present_height=0;HANDLE bitmap,old_bitmap;wchar_t root[520];bool running=true;int mouse_x=0,mouse_y=0;GameManager::State settings_return=GameManager::MENU;const int bx=BoardLayout::LEFT,by=BoardLayout::BASE_Y,ch=BoardLayout::CELL_HEIGHT;bool did_save_win=false;int select_page=0;bool graphics_ready=false;int frames_painted=0;U64 timer_last=0,timer_accum=0;int timer_frame=0;
U32 C(int r,int g,int b){return UIManager::rgb(r,g,b);}
void load_levels(){
 game.levels.count=0;
 // 清单限定本版关数，目录残留的额外关卡不自动混入；无清单兼容旧工程。
 wchar_t manifest[520];wcopy(manifest,root);wcat(manifest,L"levels\\campaign.json");char config[1024]={};read_file(manifest,config,1024);
 int limit=clamp(value(config,"\"level_count\"",256),1,256);
 for(int i=0;i<limit;i++){wchar_t p[520];wcopy(p,root);wcat(p,L"levels\\level_");wnumber(p,i+1,2);wcat(p,L".json");char b[4096]={};if(!read_file(p,b,4096))break;game.levels.data[i]=parse_level(b,i+1);game.levels.count++;}
}
void start(int id){if(id<1||id>game.levels.count||id>saves.data.unlocked)return;game.start(id);did_save_win=false;effects=EffectManager();}
// 点击与悬停共用坐标换算；网格外均交给空白场景点击处理。
bool board_cell(int mx,int my,int&x,int&y){
 return BoardLayout(game.pots.level.columns).cell(mx,my,game.pots.level.rows,x,y);
}

void draw_pot(int x,int y,Pot p,int width){
 int height=49*width/58;y+=49-height;
 if(assets.draw(ui,assets.pots[p.type],x,y,width,height,58,49))return;
 auto oval=[&](int a,int b,int w,int h,U32 color){ui.oval(x+a*width/58,y+b*height/49,w*width/58,h*height/49,color);};
 auto rect=[&](int a,int b,int w,int h,U32 color,int round){ui.rect(x+a*width/58,y+b*height/49,w*width/58,h*height/49,color,round);};
 auto line=[&](int a,int b,int c,int d,U32 color,int weight=2){ui.line(x+a*width/58,y+b*height/49,x+c*width/58,y+d*height/49,color,weight);};
 U32 colors[4]={C(73,158,165),C(223,234,231),C(211,173,113),C(184,104,37)},color=colors[p.type];
 if(p.type==1){oval(5,4,48,41,C(73,56,39));oval(8,5,42,36,color);oval(15,10,15,10,C(248,251,233));}
 else{oval(6,10,47,31,C(73,56,39));oval(9,11,41,27,color);rect(16,2,28,15,C(73,56,39),4);rect(19,4,22,14,color,3);line(16,38,43,38,C(72,61,43),3);oval(13,17,12,9,C(244,206,150));}
 if(p.variant){line(15,25,44,25,C(87,91,66));oval(26,23,7,7,C(95,94,60));}
}
void background(){if(assets.draw(ui,assets.background,0,0,1280,720))return;ui.rect(0,0,1280,720,C(198,150,84));ui.rect(0,110,1280,400,C(168,206,191));ui.oval(300,180,680,430,C(166,188,122));ui.rect(0,425,1280,295,C(182,139,76));ui.rect(840,350,230,170,C(161,130,82),20);for(int i=0;i<9;i++)ui.oval(818+i*29,338,55,24,C(103,103,84));ui.oval(250,375,240,200,C(169,105,44));ui.oval(298,439,105,95,C(90,60,39));ui.oval(690,340,320,210,C(177,111,45));ui.oval(785,429,105,95,C(97,62,38));ui.rect(0,65,1280,66,C(110,67,37));for(int x=30;x<1280;x+=300)ui.rect(x,110,18,320,C(120,75,42));for(int y=652;y<685;y+=14){ui.line(0,y,1280,y,C(84,65,39),4);for(int x=0;x<1280;x+=28)ui.line(x,y-4,x+8,y+5,C(106,79,37),2);}ui.rect(0,0,1280,66,C(51,33,26));}
void draw_player(){if(assets.draw(ui,assets.characters[player.state],100,350,190,240))return;int lift=player.state==PlayerController::Attack?-34:player.state==PlayerController::Hit?15:0;ui.oval(90,610,172,25,C(138,90,43));ui.rect(100,575,150,83,C(131,71,39),4);ui.rect(106,579,142,12,C(164,91,45));ui.rect(145,515,26,60,C(42,39,99));ui.rect(179,515,26,60,C(42,39,99));ui.oval(130,567,52,18,C(72,38,31));ui.oval(176,567,50,18,C(72,38,31));ui.rect(136,450,70,97,C(66,54,137),18);ui.rect(151,461,47,77,C(240,230,204),8);ui.oval(130,353,90,103,C(97,42,31));ui.oval(135,361,82,80,C(247,214,174));ui.oval(137,350,80,33,C(104,46,33));ui.rect(136,350,77,17,C(246,240,215),12);ui.oval(138,373,17,45,C(121,57,37));ui.oval(162,396,10,17,C(40,33,58));ui.oval(197,396,10,17,C(40,33,58));if(player.state==PlayerController::Win)ui.text(155,411,L"♡",C(186,60,90));else ui.line(177,428,190,428,C(128,65,54),2);ui.line(151,470,205,482+lift,C(249,215,177),16);ui.line(208,483+lift,235,428+lift,C(91,44,28),8);ui.rect(218,407+lift,45,28,C(190,113,40),7);ui.rect(251,409+lift,10,23,C(225,153,62),5);ui.oval(157,599,31,31,C(139,33,29));ui.oval(162,601,23,22,C(237,57,63));}
void render(){ui.dc=buffer;ui.count=0;background();player.update(game);draw_player();ui.text(28,18,L"阿Sue · 陶罐工坊",C(255,219,119));ui.text(1010,24,L"150关进阶版 v1.1",C(212,181,139),ui.small);BoardLayout board(game.pots.level.columns);int cw=board.cell_width;
 if(game.state==GameManager::PLAY||game.state==GameManager::PAUSE||game.state==GameManager::FAIL||game.state==GameManager::WIN||(game.state==GameManager::SETTINGS&&settings_return==GameManager::PAUSE)){
 wchar_t title[150]=L"LEVEL ";wnumber(title,game.level_id,2);wcat(title,L"     SCORE ");wnumber(title,game.scores.score,6);wcat(title,L"     进列 ");wnumber(title,game.pots.added,2);wcat(title,L" / ");wnumber(title,game.pots.level.waves,2);ui.text(360,20,title,C(255,231,155));
 if(game.pots.level.target_score>0){wchar_t goal[100]=L"本关目标 ";wnumber(goal,game.pots.level.target_score);wcat(goal,L" 分 · ");if(game.scores.score>=game.pots.level.target_score)wcat(goal,L"已达成，仍需完成进列");else{wcat(goal,L"还差 ");wnumber(goal,game.pots.level.target_score-game.scores.score);wcat(goal,L" 分");}ui.rect(bx,134,580,30,C(247,230,180),8);ui.text(bx+12,140,goal,game.scores.score>=game.pots.level.target_score?C(51,113,66):C(119,68,42),ui.small);}
 wchar_t capacity[32]=L"场地 ";wnumber(capacity,game.pots.level.columns);wcat(capacity,L" 列");ui.rect(bx+590,134,164,30,C(247,230,180),8);ui.text(bx+608,140,capacity,C(119,68,42),ui.small);
 ui.rect(bx-4,by+45,cw*game.pots.level.columns+8,15,C(163,170,153));ui.line(bx-10,130,bx-10,by+45,C(151,65,45),2);
 int group[192][2],mx,my;int n=board_cell(mouse_x,mouse_y,mx,my)?game.pots.group(mx,my,group):0;
 ui.rect(28,147,390,148,C(247,230,180),12);ui.text(43,162,game.pots.level.score_mode==1?L"本关：大组合有额外奖励":L"本关：普通计分",C(119,68,42),ui.small);
 wchar_t rule[100]={};for(int k=2;k<=4;k++){if(k>2)wcat(rule,L" · ");wnumber(rule,k);wcat(rule,L"罐 ");wnumber(rule,game.scores.calculate(k,game.pots.level.points,game.pots.level.score_mode));wcat(rule,L"分");}ui.text(43,193,rule,C(113,62,36),ui.small);
 wchar_t preview[100]=L"当前组合 ";if(n>=game.pots.level.min_group){wnumber(preview,n);wcat(preview,L" 罐 · ");wnumber(preview,game.scores.calculate(n,game.pots.level.points,game.pots.level.score_mode));wcat(preview,L" 分");}else wcopy(preview,L"观察连片，优先清出左侧空间");ui.text(43,224,preview,C(51,113,66),ui.small);
 ui.text(43,255,game.pots.level.rain_match_mode==1?L"有组合时补罐：颜色随机，占用空间":L"补罐会增加陶罐，注意场地空间",C(119,68,42),ui.small);
 for(int x=0;x<game.pots.level.columns;x++)for(int y=0;y<game.pots.level.rows;y++){
  Pot p=game.pots.grid[x][y];if(p.type<0)continue;int px,py;effects.position(p,x,y,board,px,py);
  if(py>=70&&px<1280&&px+cw>0)draw_pot(px,py,p,cw);
 }
 if(n>=game.pots.level.min_group&&game.state==GameManager::PLAY&&!game.anim&&!game.motion_ms)for(int i=0;i<n;i++){int x=bx+group[i][0]*cw,y=by-group[i][1]*ch;ui.line(x+6*cw/58,y+43,x+49*cw/58,y+43,C(255,228,125),3);}
 if(effects.popup_ms){wchar_t b[30]=L"+";wnumber(b,effects.popup_score);ui.text(effects.popup_x,effects.popup_y-(game.pots.level.popup_ms-effects.popup_ms)/35,b,C(255,248,182));for(int i=0;i<6;i++)ui.oval(effects.popup_x+i*7,effects.popup_y+i%3*5,4,4,C(255,242,186));}
 if(game.anim){double t=1-(double)game.anim/game.pots.level.attack_duration_ms;int tx=bx+game.target_x*cw+cw/2,ty=by-game.target_y*ch+20;int hx=(int)(240+(tx-240)*t),hy=(int)(430+(ty-430)*t);ui.rect(hx,hy,24,18,C(210,129,49),4);ui.line(hx+10,hy+15,hx+3,hy+32,C(81,45,29),5);}
 ui.button(108,640,130,40,L"NEXT →",100,game.state==GameManager::PLAY&&!game.anim&&!game.motion_ms&&game.pots.added<game.pots.level.waves);ui.text(310,688,L"点组合砸罐 · 点孤立罐或空白补罐 · NEXT 进列 · ESC 暂停",C(70,48,33),ui.small);if(game.state==GameManager::PLAY){int seconds=(game.wave_interval()-game.wave_clock+999)/1000;wchar_t b[80]=L"下一列 ";if(game.pots.added>=game.pots.level.waves){wcopy(b,L"进列完成 · 结算 ");seconds=(game.pots.level.end_delay_ms-game.end_clock+999)/1000;}wnumber(b,seconds<0?0:seconds);wcat(b,L" 秒");if(game.pots.added<game.pots.level.waves&&game.wave_interval()<game.pots.level.interval_ms)wcat(b,L"（快速）");ui.text(292,644,b,C(75,45,29),ui.small);ui.button(1120,674,112,34,L"暂停",101);
 if(!game.anim&&!game.motion_ms&&game.pots.level.rain_enabled&&!game.pots.has_moves()){ui.rect(478,90,650,36,C(246,224,162),10);ui.text(493,99,game.pots.rain_space()?L"点不可消除的陶罐或场景空白：随机陶罐从天上落下":L"场地已满：腾出左侧空间，否则下一列将导致失败",C(113,62,36),ui.small);}
 }
 }
 auto state=game.state;if(state==GameManager::PLAY){if(saves.error)ui.text(350,90,L"存档写入失败，请检查 AppData 权限",C(176,35,35));return;}
 ui.rect(345,140,590,475,C(248,232,197),24);
 if(state==GameManager::MENU){ui.text(418,183,L"陶罐工坊",C(94,52,38),ui.big);wchar_t info[80]=L"依据录像重建 · LEVEL 01—";wnumber(info,game.levels.count,2);ui.text(418,241,info,C(139,97,65),ui.small);ui.button(455,285,365,54,L"开始游戏 / 继续已解锁关卡",1);ui.button(455,355,365,54,L"选择关卡",2);ui.button(455,425,365,54,L"设置",3);ui.button(455,495,365,54,L"退出游戏",4);}
 if(state==GameManager::PAUSE||state==GameManager::FAIL||state==GameManager::WIN){ui.text(420,180,state==GameManager::FAIL?L"挑战失败":state==GameManager::WIN?L"挑战完成":L"游戏已暂停",C(94,52,38),ui.big);if(state==GameManager::FAIL)ui.text(420,224,game.failure==GameManager::SCORE_TARGET?L"未达到本关目标分数，请重新挑战":L"左侧堆满，无法继续加入新列",C(151,64,40),ui.small);int y=254;if(state==GameManager::WIN&&game.level_id<game.levels.count){ui.button(440,y,385,48,L"下一关",5);y+=60;}if(state==GameManager::PAUSE){ui.button(440,y,385,48,L"继续游戏",6);y+=60;}ui.button(440,y,385,48,L"重新挑战本关",7);y+=60;if(state==GameManager::PAUSE){ui.button(440,y,385,48,L"设置",3);y+=60;}ui.button(440,y,385,48,L"选择关卡",2);y+=60;ui.button(440,y,385,48,L"返回主菜单",8);if(state==GameManager::WIN&&game.level_id==game.levels.count)ui.text(405,570,L"全部关卡挑战完成！可重玩已解锁关卡刷新最高分。",C(121,79,46),ui.small);}
 if(state==GameManager::SELECT){select_page=clamp(select_page,0,(game.levels.count>0?game.levels.count-1:0)/50);ui.text(385,159,L"选择关卡",C(94,52,38),ui.big);wchar_t info[80]=L"共 ";wnumber(info,game.levels.count);wcat(info,L" 关；灰色关卡尚未解锁 · 页 ");wnumber(info,select_page+1);wcat(info,L" / ");wnumber(info,(game.levels.count+49)/50);ui.text(385,213,info,C(139,97,65),ui.small);for(int slot=0;slot<50;slot++){int i=select_page*50+slot;if(i>=game.levels.count)break;wchar_t b[10]={};wnumber(b,i+1,2);int x=390+(slot%10)*49,y=253+(slot/10)*49;bool enabled=i<game.levels.count&&i<saves.data.unlocked;ui.button(x,y,44,40,b,1000+i,enabled);}if(select_page>0)ui.button(385,582,100,28,L"上一页",400);if((select_page+1)*50<game.levels.count)ui.button(785,582,100,28,L"下一页",401);ui.button(460,529,360,48,L"返回主菜单",8);}
 if(state==GameManager::SETTINGS){ui.text(405,173,L"游戏设置",C(94,52,38),ui.big);wchar_t m[50]=L"背景音乐 ";wnumber(m,saves.data.music);wchar_t s[50]=L"音效 ";wnumber(s,saves.data.sfx);ui.text(410,256,m,C(106,69,44));ui.button(685,245,72,44,L"−",300);ui.button(770,245,72,44,L"+",301);ui.text(410,319,s,C(106,69,44));ui.button(685,308,72,44,L"−",302);ui.button(770,308,72,44,L"+",303);ui.button(410,374,430,48,saves.data.fullscreen?L"全屏：开 → 切换窗口":L"全屏：关 → 切换全屏",304);ui.button(410,437,430,48,saves.data.width==1280?L"分辨率：1280 × 720":L"分辨率：1920 × 1080",305);ui.button(410,512,430,48,L"保存并返回",306);}
}
void dispatch(int action){if(action==1)start(saves.data.unlocked>game.levels.count?game.levels.count:saves.data.unlocked);else if(action==2){game.state=GameManager::SELECT;select_page=clamp((game.level_id-1)/50,0,(game.levels.count>0?game.levels.count-1:0)/50);}else if(action==3){settings_return=game.state;game.state=GameManager::SETTINGS;}else if(action==4)running=false;else if(action==5)start(game.level_id+1);else if(action==6)game.state=GameManager::PLAY;else if(action==7)start(game.level_id);else if(action==8)game.state=GameManager::MENU;else if(action==100&&game.state==GameManager::PLAY&&!game.anim&&!game.motion_ms&&game.pots.added<game.pots.level.waves){effects.capture(game.pots);int a=game.pots.added;game.next();if(game.pots.added!=a)effects.begin(game.pots.level.move_ms);}else if(action==101)game.state=GameManager::PAUSE;else if(action>=1000&&action<1256)start(action-999);else if(action==400)select_page=clamp(select_page-1,0,(game.levels.count>0?game.levels.count-1:0)/50);else if(action==401)select_page=clamp(select_page+1,0,(game.levels.count>0?game.levels.count-1:0)/50);else if(action>=300&&action<=306){auto&d=saves.data;if(action==300)d.music=clamp(d.music-10,0,100);if(action==301)d.music=clamp(d.music+10,0,100);if(action==302)d.sfx=clamp(d.sfx-10,0,100);if(action==303)d.sfx=clamp(d.sfx+10,0,100);if(action==304){d.fullscreen=!d.fullscreen;settings.apply();}if(action==305){d.width=d.width==1280?1920:1280;d.height=d.width==1280?720:1080;settings.apply();}audio.volumes(d.music,d.sfx);if(action==300||action==301)audio.start_music();saves.save();if(action==306)game.state=settings_return;}}

// 首帧不依赖计时、音频或空闲循环；创建彩色缓冲区后才显示窗口。
void startup_log(const char*message){
 wchar_t path[520]={};wcopy(path,root);wcat(path,L"startup_log.txt");
 HANDLE f=__imp_CreateFileW(path,0x00000004,1,0,4,0,0);
 if(f!=(HANDLE)-1){U32 written=0;__imp_WriteFile(f,message,slen(message),&written,0);__imp_FlushFileBuffers(f);__imp_CloseHandle(f);}
}
void simulate_step(int step){
 if(game.state==GameManager::PLAY&&((game.anim<=step&&game.motion_ms<=step&&game.pots.added<game.pots.level.waves&&game.wave_clock+step>=game.wave_interval())||(game.anim&&!game.impacted&&game.anim-step<=game.pots.level.attack_duration_ms-game.pots.level.attack_delay_ms)))effects.capture(game.pots);
 int before=game.pots.added;bool impact=game.impacted;game.tick(step);
 if(before!=game.pots.added||(!impact&&game.impacted)){
  effects.begin(before!=game.pots.added?game.pots.level.move_ms:game.pots.level.fall_ms);
  if(!impact&&game.impacted&&game.removed){effects.popup_ms=game.pots.level.popup_ms;effects.popup_score=game.scores.calculate(game.removed,game.pots.level.points,game.pots.level.score_mode);effects.popup_x=BoardLayout(game.pots.level.columns).x(game.target_x);effects.popup_y=by-game.target_y*ch;audio.hit();}
 }
 if(game.state==GameManager::PLAY)effects.tick(step);
 if(game.state==GameManager::WIN&&!did_save_win){saves.complete(game.level_id,game.scores.score);did_save_win=true;}
}
void on_timer(){
 auto previous_state=game.state;U64 now=__imp_GetTickCount64();U64 elapsed=now-timer_last;timer_last=now;if(elapsed>250)elapsed=250;timer_accum+=elapsed*60;
 while(timer_accum>=1000){simulate_step((timer_frame++%3==0)?16:17);timer_accum-=1000;}
 if(game.state==GameManager::PLAY||previous_state==GameManager::PLAY)__imp_InvalidateRect(window,0,0);
}
// 两级缓冲：逻辑场景 -> 完整客户区（含黑边）-> 一次BitBlt显示。
// 可见窗口禁止先清屏再绘制，缓冲区只在尺寸变化时重建。
bool prepare_present_buffer(HDC screen,int width,int height){
 if(present_buffer&&present_width==width&&present_height==height)return true;
 HDC next=__imp_CreateCompatibleDC(screen);HANDLE next_bitmap=next?__imp_CreateCompatibleBitmap(screen,width,height):0;
 if(!next||!next_bitmap){if(next)__imp_DeleteDC(next);return false;}
 HANDLE next_old=__imp_SelectObject(next,next_bitmap);
 if(!next_old||next_old==(HANDLE)-1){__imp_DeleteObject(next_bitmap);__imp_DeleteDC(next);return false;}
 if(present_buffer){__imp_SelectObject(present_buffer,present_old_bitmap);__imp_DeleteObject(present_bitmap);__imp_DeleteDC(present_buffer);}
 present_buffer=next;present_bitmap=next_bitmap;present_old_bitmap=next_old;present_width=width;present_height=height;return true;
}
I64 procedure(HWND hwnd,U32 msg,U64 wp,I64 lp){
 if(msg==0x10){running=false;__imp_PostQuitMessage(0);return 0;}
 if(msg==0x5){if(graphics_ready)__imp_InvalidateRect(hwnd,0,0);return 0;}
 if(msg==0x14)return 1; // WM_ERASEBKGND：由完整双缓冲画面覆盖。
 if(msg==0x113&&wp==1){if(graphics_ready)on_timer();return 0;}
 if(msg==0x100){
  if(wp==27){if(game.state==GameManager::PLAY)game.state=GameManager::PAUSE;else if(game.state==GameManager::PAUSE)game.state=GameManager::PLAY;else if(game.state==GameManager::SETTINGS)dispatch(306);else game.state=GameManager::MENU;}
  if(wp==32&&game.state==GameManager::PLAY)dispatch(100);__imp_InvalidateRect(hwnd,0,0);return 0;
 }
 if(msg==0x200||msg==0x201){
  if(!graphics_ready||ui.sx<=0||ui.sy<=0)return 0;
  int x=(short)(lp&65535),y=(short)((lp>>16)&65535);mouse_x=(int)((x-ui.ox)/ui.sx);mouse_y=(int)((y-ui.oy)/ui.sy);
  if(msg==0x200&&game.state==GameManager::PLAY)__imp_InvalidateRect(hwnd,0,0);
  if(msg==0x201){int a=ui.hit(mouse_x,mouse_y);if(a)dispatch(a);else if(game.state==GameManager::PLAY&&mouse_x>=0&&mouse_x<1280&&mouse_y>=66&&mouse_y<674&&!(mouse_x>=108&&mouse_x<238&&mouse_y>=640&&mouse_y<680)){
 if(!game.anim&&!game.motion_ms){int cx,cy;board_cell(mouse_x,mouse_y,cx,cy);effects.capture(game.pots);if(game.scene_click(cx,cy)>0)effects.begin(game.pots.level.rain_ms,true);}
 }__imp_InvalidateRect(hwnd,0,0);}return 0;
 }
 if(msg==0xf){
  PAINTSTRUCT ps={};HDC dc=__imp_BeginPaint(hwnd,&ps);RECT r={};__imp_GetClientRect(hwnd,&r);int w=r.right,h=r.bottom;
  if(dc&&graphics_ready&&w>0&&h>0){
   render(); // WM_PAINT 自己生成完整菜单；不读未完成的上一帧。
   double scale=(double)w/1280;if((double)h/720<scale)scale=(double)h/720;ui.sx=ui.sy=scale;ui.ox=(w-(int)(1280*scale))/2;ui.oy=(h-(int)(720*scale))/2;
   if(!prepare_present_buffer(dc,w,h)){startup_log("ERROR: presentation buffer allocation\r\n");__imp_EndPaint(hwnd,&ps);return 0;}
   HDC previous=ui.dc;ui.dc=present_buffer;ui.rect(0,0,w,h,C(35,26,21));ui.dc=previous;
   int composed=__imp_StretchBlt(present_buffer,ui.ox,ui.oy,(int)(1280*scale),(int)(720*scale),buffer,0,0,1280,720,0x00cc0020);
   int ok=composed?__imp_BitBlt(dc,0,0,w,h,present_buffer,0,0,0x00cc0020):0;
   if(!frames_painted)startup_log(ok?"first frame copied OK\r\n":"ERROR: StretchBlt failed\r\n");frames_painted++;
  }
  __imp_EndPaint(hwnd,&ps);return 0;
 }
 return __imp_DefWindowProcW(hwnd,msg,wp,lp);
}
extern "C" void entry(){
 game=GameManager();saves=SaveManager();ui=UIManager();audio=AudioManager();effects=EffectManager();player=PlayerController();assets=AssetManager();running=true;graphics_ready=false;frames_painted=0;timer_accum=0;timer_frame=0;
 __imp_GetModuleFileNameW(0,root,500);int n=wlen(root);while(n&&root[n-1]!=L'\\')n--;root[n]=0;
 startup_log("\r\nASuePotGame v1.1 startup\r\n");saves.init();load_levels();
 if(!game.levels.count){__imp_MessageBoxW(0,L"未找到 levels\\level_01.json。请先解压整个文件夹，再启动 EXE。",L"缺少关卡配置",16);__imp_ExitProcess(1);return;}
 HANDLE instance=__imp_GetModuleHandleW(0);WNDCLASSW cls={};cls.proc=procedure;cls.instance=instance;cls.cursor=__imp_LoadCursorW(0,(LPCWSTR)32512);cls.name=L"ASuePotGameWindow";
 if(!__imp_RegisterClassW(&cls)){startup_log("ERROR: RegisterClassW\r\n");__imp_ExitProcess(1);return;}
 window=__imp_CreateWindowExW(0,cls.name,L"阿Sue砸陶罐 · 150关进阶版 v1.1",0x00cf0000,60,40,1296,759,0,0,instance,0);
 if(!window){startup_log("ERROR: CreateWindowExW\r\n");__imp_ExitProcess(1);return;}
 startup_log("window created (hidden)\r\n");
 HDC dc=__imp_GetDC(window);buffer=dc?__imp_CreateCompatibleDC(dc):0;bitmap=dc?__imp_CreateCompatibleBitmap(dc,1280,720):0;
 if(!dc||!buffer||!bitmap){startup_log("ERROR: graphics allocation\r\n");__imp_MessageBoxW(window,L"无法创建游戏画布。请把 startup_log.txt 发给我。",L"绘制初始化失败",16);__imp_ExitProcess(1);return;}
 old_bitmap=__imp_SelectObject(buffer,bitmap);__imp_ReleaseDC(window,dc);
 if(!old_bitmap||old_bitmap==(HANDLE)-1){startup_log("ERROR: SelectObject bitmap\r\n");__imp_ExitProcess(1);return;}
 ui.init();assets.init(root);settings.window=window;settings.save=&saves;settings.apply();
 // 音频是可选子系统：启动不调用 MCI 打开媒体，不能阻止首帧显示。
 audio.prepare(root,saves.data.music,saves.data.sfx);
 graphics_ready=true;render();startup_log("graphics ready; initial menu rendered\r\n");
 __imp_ShowWindow(window,5);__imp_InvalidateRect(window,0,0);__imp_UpdateWindow(window);
 timer_last=__imp_GetTickCount64();
 if(!__imp_SetTimer(window,1,16,0)){startup_log("ERROR: SetTimer\r\n");__imp_MessageBoxW(window,L"无法建立游戏计时器。请把 startup_log.txt 发给我。",L"启动失败",16);__imp_ExitProcess(1);return;}
 startup_log("message loop ready\r\n");MSG msg={};
 while(running){int result=__imp_GetMessageW(&msg,0,0,0);if(result<=0)break;__imp_TranslateMessage(&msg);__imp_DispatchMessageW(&msg);}
 __imp_KillTimer(window,1);saves.save();audio.stop();assets.destroy();ui.destroy();if(present_buffer){__imp_SelectObject(present_buffer,present_old_bitmap);__imp_DeleteObject(present_bitmap);__imp_DeleteDC(present_buffer);present_buffer=0;}__imp_SelectObject(buffer,old_bitmap);__imp_DeleteObject(bitmap);__imp_DeleteDC(buffer);startup_log("clean exit\r\n");__imp_ExitProcess(0);
}
