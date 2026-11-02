#pragma once
#include "util.h"
struct AudioManager {
 wchar_t root[520]={};int music=70,sfx=80;bool opened=false;
 void prepare(const wchar_t*p,int m,int s){wcopy(root,p);music=m;sfx=s;opened=false;}
 void start_music(){if(opened||!music)return;wchar_t cmd[700]=L"open \"";wcat(cmd,root);wcat(cmd,L"assets\\music.wav\" type mpegvideo alias bg");if(!__imp_mciSendStringW(cmd,0,0,0)){opened=true;__imp_mciSendStringW(L"play bg repeat",0,0,0);}}
 void volumes(int m,int s){music=m;sfx=s;wchar_t cmd[100]=L"setaudio bg volume to ";wnumber(cmd,m*10);if(opened)__imp_mciSendStringW(cmd,0,0,0);}
 void hit(){if(!sfx)return;wchar_t p[520];wcopy(p,root);wcat(p,L"assets\\hit_");wnumber(p,sfx);wcat(p,L".wav");__imp_PlaySoundW(p,0,0x20001);}
 void stop(){__imp_PlaySoundW(0,0,0);if(opened)__imp_mciSendStringW(L"close bg",0,0,0);opened=false;}
};
