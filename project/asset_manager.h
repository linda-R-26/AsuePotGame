#pragma once
#include "ui_manager.h"
struct AssetManager {
 HANDLE pots[4]={},characters[5]={},background=0;HDC dc=0;
 HANDLE load(const wchar_t*root,const wchar_t*name){wchar_t path[520];wcopy(path,root);wcat(path,L"assets\\");wcat(path,name);return __imp_LoadImageW(0,path,0,0,0,0x10|0x2000);}
 void init(const wchar_t*root){dc=__imp_CreateCompatibleDC(0);const wchar_t*pn[4]={L"pot_blue.bmp",L"pot_white.bmp",L"pot_beige.bmp",L"pot_brown.bmp"};const wchar_t*cn[5]={L"character_idle.bmp",L"character_attack.bmp",L"character_hit.bmp",L"character_fail.bmp",L"character_win.bmp"};for(int i=0;i<4;i++)pots[i]=load(root,pn[i]);for(int i=0;i<5;i++)characters[i]=load(root,cn[i]);background=load(root,L"background.bmp");}
 bool draw(UIManager&ui,HANDLE bitmap,int x,int y,int w,int h,int source_w=0,int source_h=0){if(!bitmap)return false;HANDLE old=__imp_SelectObject(dc,bitmap);__imp_TransparentBlt(ui.dc,x,y,w,h,dc,0,0,source_w?source_w:w,source_h?source_h:h,UIManager::rgb(255,0,255));__imp_SelectObject(dc,old);return true;}
 void destroy(){for(int i=0;i<4;i++)if(pots[i])__imp_DeleteObject(pots[i]);for(int i=0;i<5;i++)if(characters[i])__imp_DeleteObject(characters[i]);if(background)__imp_DeleteObject(background);__imp_DeleteDC(dc);}
};
