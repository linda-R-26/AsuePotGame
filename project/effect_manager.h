#pragma once
#include "pot_manager.h"
#include "board_layout.h"
// 固定容量，动画和特效不会产生无限节点。
struct EffectManager {
 Pot previous[16][12];int elapsed=10000,popup_ms=0,popup_score=0,popup_x=0,popup_y=0;int duration=220;bool moving=false,raining=false;
 EffectManager(){for(int x=0;x<16;x++)for(int y=0;y<12;y++)previous[x][y]={-1,0,0};}
 void capture(PotManager&p){for(int x=0;x<16;x++)for(int y=0;y<12;y++)previous[x][y]=p.grid[x][y];}
 void begin(int ms,bool rain=false){duration=ms>0?ms:1;elapsed=0;moving=true;raining=rain;}
 void tick(int ms){elapsed+=ms;popup_ms-=ms;if(popup_ms<0)popup_ms=0;if(elapsed>=duration)moving=false;}
 bool old_pos(int uid,int&x,int&y)const{for(int a=0;a<16;a++)for(int b=0;b<12;b++)if(previous[a][b].type>=0&&previous[a][b].uid==uid){x=a;y=b;return true;}return false;}
 double progress()const{double t=(double)elapsed/duration;return t<0?0:t>1?1:t;}
 // 恢复v0.6的匀速横向平移：新列从场地右侧一格推出；不回弹、不绘制速度线。
 void position(Pot p,int column,int row,const BoardLayout&board,int&px,int&py)const{
  px=board.x(column);py=board.y(row);if(!moving)return;
  int a=column,b=row;bool existed=old_pos(p.uid,a,b);double t=progress();
  if(!existed&&raining){int top=-BoardLayout::CELL_HEIGHT-(p.uid%3)*BoardLayout::CELL_HEIGHT;py=(int)(top+(py-top)*t*t);return;}
  if(!existed)a=board.columns;
  px=(int)(board.x(a)+(board.x(column)-board.x(a))*t);py=(int)(board.y(b)+(board.y(row)-board.y(b))*t);
 }
};
