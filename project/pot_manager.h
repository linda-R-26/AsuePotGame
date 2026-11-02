#pragma once
#include "pot.h"
#include "level_data.h"
struct PotManager {
 Pot grid[16][12]; unsigned rng=1; LevelData level; int added=0, serial=0;
 unsigned next_random(){rng=rng*1664525u+1013904223u;return rng;}
 int random_type(int exclude=-1){int total=0;for(int i=0;i<4;i++)if(i!=exclude)total+=level.weights[i];if(!total)return exclude>=0?exclude:0;int pick=(next_random()>>16)%total;for(int i=0;i<4;i++){if(i==exclude)continue;pick-=level.weights[i];if(pick<0)return i;}return 3;}
 Pot make_pot(int type){return {type,(int)((next_random()>>8)%5==0),++serial};}
 void reset(LevelData l){level=l;added=0;serial=0;rng=l.seed;for(int x=0;x<16;x++)for(int y=0;y<12;y++)grid[x][y]={-1,0};}
 bool add_column(){
  if(added>=level.waves)return true;
  for(int y=0;y<level.rows;y++)if(grid[0][y].type>=0)return false;
  for(int x=0;x<level.columns-1;x++)for(int y=0;y<level.rows;y++)grid[x][y]=grid[x+1][y];
  // 前三关保留v0.3随机序列，便于用户比较操作手感。
  bool legacy=level.id<=3&&level.wave_min_rows==level.rows&&level.wave_max_rows==level.rows&&!level.repeat_percent&&!level.pattern_mode;
  for(int i=0;i<4;i++)if(level.weights[i]!=25)legacy=false;
  int height=legacy?level.rows:level.wave_min_rows+(next_random()>>16)%(level.wave_max_rows-level.wave_min_rows+1);
  for(int y=0;y<level.rows;y++){
   Pot p={-1,0};if(y<height){
    if(legacy){unsigned n=next_random();grid[level.columns-1][y]={(int)((n>>16)%4),(int)((n>>8)%5==0),++serial};continue;}
    int type=random_type();
    if(y&&level.pattern_mode==2&&type==grid[level.columns-1][y-1].type)type=random_type(type);
    else if(y&&level.repeat_percent&&(int)((next_random()>>16)%100)<level.repeat_percent)type=grid[level.columns-1][y-1].type;
    if(level.pattern_mode==1&&y%2)type=grid[level.columns-1][y-1].type;
    p=make_pot(type);
   }grid[level.columns-1][y]=p;
  }
  added++;return true;
 }
 int group(int x,int y,int out[][2]) const {
  if(x<0||x>=level.columns||y<0||y>=level.rows||grid[x][y].type<0)return 0;
  bool seen[16][12]={};int count=1,head=0;out[0][0]=x;out[0][1]=y;seen[x][y]=true;
  const int dx[4]={-1,1,0,0},dy[4]={0,0,-1,1};
  while(head<count){int a=out[head][0],b=out[head++][1];for(int i=0;i<4;i++){
   int c=a+dx[i],d=b+dy[i];if(c<0||c>=level.columns||d<0||d>=level.rows||seen[c][d])continue;
   if(grid[c][d].type!=grid[x][y].type)continue;
   if(!level.group_by_color&&grid[c][d].variant!=grid[x][y].variant)continue;
   seen[c][d]=true;out[count][0]=c;out[count++][1]=d;
  }}return count;
 }
 bool has_moves()const{int cells[192][2];for(int x=0;x<level.columns;x++)for(int y=0;y<level.rows;y++)if(grid[x][y].type>=0&&group(x,y,cells)>=level.min_group)return true;return false;}
 int height(int x)const{int h=0;for(int y=0;y<level.rows;y++)if(grid[x][y].type>=0)h=y+1;return h;}
 int rain_space()const{int n=0;for(int x=0;x<level.columns;x++)n+=level.rows-height(x);return n;}
 int rain_left()const{int left=level.columns;for(int x=0;x<level.columns;x++)if(height(x)){left=x;break;}if(left==level.columns)return level.columns>3?level.columns-3:0;int room=0;for(int x=left;x<level.columns;x++)room+=level.rows-height(x);return room<level.rain_min&&left>0?left-1:left;}
 int add_rain(){
  if(!level.rain_enabled||(level.rain_trigger_mode==0&&has_moves())||!rain_space())return 0;
  int amount=level.rain_min+(next_random()>>16)%(level.rain_max-level.rain_min+1),made=0;
  // 辅助是否启用由配置决定。无组合时保留救场；已有组合误点则随机落罐。
  // 落罐覆盖当前陶罐区，不把孤立新罐随机扔到远处左侧失败边界。
  int candidates[16],count=0,left=rain_left();
  for(int x=left;x<level.columns;x++){int h=height(x);int need=h?level.min_group-1:level.min_group;if(level.rows-h>=need&&amount>=need)candidates[count++]=x;}
  bool assist=level.rain_match_mode==0||(level.rain_match_mode==1&&!has_moves());
  if(count&&assist){int x=candidates[(next_random()>>16)%count],h=height(x),need=h?level.min_group-1:level.min_group;
   Pot match=h?grid[x][h-1]:make_pot(random_type());
   for(int i=0;i<need;i++){Pot p=make_pot(match.type);if(!level.group_by_color)p.variant=match.variant;grid[x][h+i]=p;made++;}
  }
  while(made<amount){count=0;for(int x=left;x<level.columns;x++)if(height(x)<level.rows)candidates[count++]=x;if(!count)break;int x=candidates[(next_random()>>16)%count];grid[x][height(x)]=make_pot(random_type());made++;}
  return made;
 }
 int hit(int x,int y){int cells[192][2];int n=group(x,y,cells);if(n<level.min_group)return 0;
  for(int i=0;i<n;i++)grid[cells[i][0]][cells[i][1]].type=-1;
  for(int c=0;c<level.columns;c++){int write=0;for(int r=0;r<level.rows;r++)if(grid[c][r].type>=0)grid[c][write++]=grid[c][r];while(write<level.rows)grid[c][write++]={-1,0};}
  if(level.compact_empty_columns){int write=level.columns-1;for(int c=level.columns-1;c>=0;c--){bool occupied=false;for(int r=0;r<level.rows;r++)occupied|=grid[c][r].type>=0;if(occupied){for(int r=0;r<level.rows;r++)grid[write][r]=grid[c][r];write--;}}while(write>=0){for(int r=0;r<level.rows;r++)grid[write][r]={-1,0};write--;}}
  return n;
 }
 int count()const{int n=0;for(int x=0;x<level.columns;x++)for(int y=0;y<level.rows;y++)n+=grid[x][y].type>=0;return n;}
};
