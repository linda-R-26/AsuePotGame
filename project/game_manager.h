#pragma once
#include "pot_manager.h"
#include "score_manager.h"
#include "level_manager.h"
struct GameManager {
 enum State { MENU,PLAY,PAUSE,FAIL,WIN,SELECT,SETTINGS } state=MENU;
 enum Failure { NO_FAILURE,OVERFLOW,SCORE_TARGET } failure=NO_FAILURE;
 PotManager pots;ScoreManager scores;LevelManager levels;int level_id=1;int wave_clock=0,anim=0,end_clock=0,motion_ms=0,rain_count=0;int target_x=-1,target_y=-1;bool impacted=false;int removed=0;
 void start(int id){level_id=id;levels.current=id;pots.reset(levels.get(id));scores.score=0;wave_clock=anim=end_clock=motion_ms=rain_count=0;target_x=target_y=-1;removed=0;impacted=false;failure=NO_FAILURE;state=PLAY;pots.add_column();}
 void attack(int x,int y){if(state!=PLAY||anim||motion_ms||x<0||x>=pots.level.columns||y<0||y>=pots.level.rows||pots.grid[x][y].type<0)return;target_x=x;target_y=y;anim=pots.level.attack_duration_ms;impacted=false;}
 void next(bool automatic=false){if(state!=PLAY||anim||motion_ms)return;if(pots.added>=pots.level.waves)return;
  // 自动进列保留动作占用的超时时间，防止反复落罐变相放慢传送带。
  // 手动NEXT仍从新列重新计时；旧配置默认保留原有行为。
  int overdue=wave_clock-wave_interval();wave_clock=automatic&&pots.level.keep_wave_overflow&&overdue>0?overdue:0;
  if(!pots.add_column()){state=FAIL;failure=OVERFLOW;}else motion_ms=pots.level.move_ms;
 }
 int wave_interval()const{const LevelData&l=pots.level;return l.burst_every&&pots.added%l.burst_every==0?l.burst_interval_ms:l.interval_ms;}
 int rain(){if(state!=PLAY||anim||motion_ms)return 0;int n=pots.add_rain();if(n){motion_ms=pots.level.rain_ms;rain_count++;}return n;}
 // 场景左键：点中可消除组发槌；孤立罐、空格、背景都补罐。
 // 返回-1表示发槌，正数为落罐数，0为动画中/暂停/无空间等未执行情况。
 int scene_click(int x,int y){if(state!=PLAY||anim||motion_ms)return 0;int cells[192][2];if(pots.group(x,y,cells)>=pots.level.min_group){attack(x,y);return -1;}return rain();}
 void tick(int ms){if(state!=PLAY)return;const LevelData &l=pots.level;
  if(motion_ms){motion_ms-=ms;if(motion_ms<0)motion_ms=0;}
  if(anim){anim-=ms;if(!impacted&&anim<=l.attack_duration_ms-l.attack_delay_ms){removed=pots.hit(target_x,target_y);scores.add(removed?scores.calculate(removed,l.points,l.score_mode):-l.invalid_penalty);impacted=true;if(removed)motion_ms=l.fall_ms;}if(anim<0)anim=0;}
  if(pots.added<l.waves){wave_clock+=ms;if(wave_clock>=wave_interval()&&!anim&&!motion_ms)next(true);}
  else {end_clock+=ms;if(end_clock>=l.end_delay_ms&&!anim&&!motion_ms){if(scores.score<l.target_score){state=FAIL;failure=SCORE_TARGET;}else state=WIN;}}
 }
};
