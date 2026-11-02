#pragma once
#include "util.h"
struct UIManager {
 struct Button{RECT rect;int action;};Button buttons[64];int count=0;HDC dc=0;HANDLE font=0,small=0,big=0;double sx=1,sy=1;int ox=0,oy=0;
 void init(){font=__imp_CreateFontW(-23,0,0,0,600,0,0,0,1,0,0,4,0,L"Microsoft YaHei");small=__imp_CreateFontW(-16,0,0,0,400,0,0,0,1,0,0,4,0,L"Microsoft YaHei");big=__imp_CreateFontW(-40,0,0,0,700,0,0,0,1,0,0,4,0,L"Microsoft YaHei");}
 static U32 rgb(int r,int g,int b){return r|(g<<8)|(b<<16);}
 void rect(int x,int y,int w,int h,U32 color,int round=0){HANDLE brush=__imp_CreateSolidBrush(color),pen=__imp_CreatePen(0,1,color);HANDLE ob=__imp_SelectObject(dc,brush),op=__imp_SelectObject(dc,pen);if(round)__imp_RoundRect(dc,x,y,x+w,y+h,round,round);else __imp_Rectangle(dc,x,y,x+w,y+h);__imp_SelectObject(dc,ob);__imp_SelectObject(dc,op);__imp_DeleteObject(brush);__imp_DeleteObject(pen);}
 void oval(int x,int y,int w,int h,U32 color){HANDLE brush=__imp_CreateSolidBrush(color),pen=__imp_CreatePen(0,1,color);HANDLE ob=__imp_SelectObject(dc,brush),op=__imp_SelectObject(dc,pen);__imp_Ellipse(dc,x,y,x+w,y+h);__imp_SelectObject(dc,ob);__imp_SelectObject(dc,op);__imp_DeleteObject(brush);__imp_DeleteObject(pen);}
 void line(int x,int y,int a,int b,U32 color,int width=2){HANDLE pen=__imp_CreatePen(0,width,color),old=__imp_SelectObject(dc,pen);__imp_MoveToEx(dc,x,y,0);__imp_LineTo(dc,a,b);__imp_SelectObject(dc,old);__imp_DeleteObject(pen);}
 void text(int x,int y,LPCWSTR s,U32 c,HANDLE f=0){__imp_SelectObject(dc,f?f:font);__imp_SetBkMode(dc,1);__imp_SetTextColor(dc,c);__imp_TextOutW(dc,x,y,s,wlen(s));}
 void button(int x,int y,int w,int h,LPCWSTR s,int action,bool enabled=true){rect(x,y,w,h,enabled?rgb(134,76,51):rgb(204,190,164),14);int inset=w<=60&&h<44?(w-wlen(s)*9)/2:16;if(inset<4)inset=4;text(x+inset,y+9,s,enabled?rgb(255,245,218):rgb(111,100,86),h<44?small:font);if(enabled&&count<64)buttons[count++]={{x,y,x+w,y+h},action};}
 int hit(int x,int y){for(int i=count-1;i>=0;i--)if(x>=buttons[i].rect.left&&x<buttons[i].rect.right&&y>=buttons[i].rect.top&&y<buttons[i].rect.bottom)return buttons[i].action;return 0;}
 void destroy(){__imp_DeleteObject(font);__imp_DeleteObject(small);__imp_DeleteObject(big);}
};
