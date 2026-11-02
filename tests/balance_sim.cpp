#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include "balance_player.h"
#include "../project/level_loader.h"
int main(int argc,char**argv){
 const char*dir=argc>1?argv[1]:"ASuePotGame/levels";
 int wait_ms=argc>2?atoi(argv[2]):150,policy=argc>3?atoi(argv[3]):3;
 unsigned seed=argc>4?(unsigned)atoi(argv[4]):1;
 int jitter=argc>5?atoi(argv[5]):0;
 assert(wait_ms>=0&&wait_ms<=5000&&policy>=0&&policy<=4&&jitter>=0&&jitter<=5000);
 printf("level,state,failure,score,target,seconds,rains,hits,pots_removed,max_group,added\n");
 for(int id=1;id<=256;id++){
  char path[512];snprintf(path,sizeof(path),"%s/level_%02d.json",dir,id);
  std::ifstream f(path);if(!f)break;std::stringstream s;s<<f.rdbuf();
  GameManager g;g.levels.data[id-1]=parse_level(s.str().c_str(),id);g.start(id);
  BalancePlayer player(wait_ms,policy,seed,jitter);int steps=0;
  while(g.state==GameManager::PLAY&&steps++<200000){player.act(g,17);g.tick(17);}
  assert(steps<200000);
  printf("%d,%d,%d,%d,%d,%.3f,%d,%d,%d,%d,%d\n",id,g.state,g.failure,g.scores.score,g.pots.level.target_score,steps*.017,g.rain_count,player.hits,player.pots_removed,player.max_group,g.pots.added);
 }
}
