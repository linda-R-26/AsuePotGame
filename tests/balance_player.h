#pragma once
#include "../project/core.h"
// A visible-board player for balancing. It never reads the next RNG state,
// future columns or rain. The delay is extra thinking time after animations.
struct BalanceMove { int x,y,n; };
inline int balance_moves(const PotManager&p,BalanceMove out[192]){
 bool seen[16][12]={};int count=0,cells[192][2];
 for(int x=0;x<p.level.columns;x++)for(int y=0;y<p.level.rows;y++){
  if(seen[x][y]||p.grid[x][y].type<0)continue;
  int n=p.group(x,y,cells);for(int i=0;i<n;i++)seen[cells[i][0]][cells[i][1]]=true;
  if(n>=p.level.min_group)out[count++]={x,y,n};
 }return count;
}
struct BalancePlayer {
 int delay,policy,jitter,idle=0,thinking,hits=0,pots_removed=0,max_group=0;
 unsigned rng;
 BalancePlayer(int d,int p,unsigned seed=1,int j=0):delay(d),policy(p),jitter(j),thinking(d),rng(seed){}
 unsigned random(){rng=rng*1664525u+1013904223u;return rng>>16;}
 void act(GameManager&g,int ms){
  if(g.state!=GameManager::PLAY)return;
  if(g.anim||g.motion_ms){idle=0;return;}
  idle+=ms;if(idle<thinking)return;
  BalanceMove m[192];int count=balance_moves(g.pots,m);
  if(count){
   int pick=0;
   if(policy==1)pick=random()%count;
   else if(policy!=2)for(int i=1;i<count;i++)if(m[i].n>m[pick].n)pick=i;
   // Policies 3/4 inspect one elimination ahead. 4 also rewards clearing
   // columns, especially when only two or fewer spaces remain.
   if(policy==3||policy==4){
    int best=-1,occupied=0;for(int x=0;x<g.pots.level.columns;x++)occupied+=g.pots.height(x)>0;
    for(int i=0;i<count;i++){
     PotManager p=g.pots;p.hit(m[i].x,m[i].y);BalanceMove next[192];int nc=balance_moves(p,next),nn=0;
     for(int j=0;j<nc;j++)if(next[j].n>nn)nn=next[j].n;
     int v=g.scores.calculate(m[i].n,p.level.points,p.level.score_mode)+g.scores.calculate(nn,p.level.points,p.level.score_mode)*3/4;
     if(policy==4){int after=0;for(int x=0;x<p.level.columns;x++)after+=p.height(x)>0;
      int pressure=p.level.columns-occupied<=2?120:15;v+=(occupied-after)*p.level.points*pressure;
     }else if(p.height(0)==0)v+=p.level.points*5;
     if(v>best){best=v;pick=i;}
    }
   }
   g.scene_click(m[pick].x,m[pick].y);hits++;pots_removed+=m[pick].n;
   if(m[pick].n>max_group)max_group=m[pick].n;
  }else if(g.pots.added<g.pots.level.waves)g.scene_click(-1,-1);
  idle=0;thinking=delay+(jitter?(int)(random()%(jitter+1)):0);
 }
};
