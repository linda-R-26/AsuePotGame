#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include "../project/core.h"
#include "../project/level_loader.h"
#include "balance_player.h"
#include "../project/effect_manager.h"
static void load(GameManager&g){g.levels.count=150;for(int id=1;id<=150;id++){char path[80];sprintf(path,"ASuePotGame/levels/level_%02d.json",id);std::ifstream f(path);assert(f);std::stringstream s;s<<f.rdbuf();g.levels.data[id-1]=parse_level(s.str().c_str(),id);}}
static void check_grid(const PotManager&p){int ids[192],n=0;for(int x=0;x<p.level.columns;x++){bool empty=false;for(int y=0;y<p.level.rows;y++){Pot a=p.grid[x][y];if(a.type<0){empty=true;continue;}assert(!empty&&a.type<4&&a.uid>0);for(int i=0;i<n;i++)assert(ids[i]!=a.uid);ids[n++]=a.uid;}}assert(n==p.count()&&n<=150);}
static bool best_move(GameManager&g){int cells[192][2],best=0,bx=-1,by=-1;for(int x=0;x<g.pots.level.columns;x++)for(int y=0;y<g.pots.level.rows;y++){int n=g.pots.group(x,y,cells);if(n>best){best=n;bx=x;by=y;}}if(best>=g.pots.level.min_group){g.attack(bx,by);return true;}return false;}
int main(){std::setbuf(stdout,nullptr);
 PotManager p;LevelData l;l.columns=13;p.reset(l);p.grid[12][0]={0,0,1};p.grid[12][1]={0,1,2};p.grid[12][2]={1,0,3};p.grid[11][0]={0,0,4};int group[192][2];assert(p.group(12,0,group)==3);assert(p.hit(12,0)==3);assert(p.grid[12][0].type==1&&p.grid[12][0].uid==3);assert(p.hit(12,0)==0);
 p.reset(l);for(int i=0;i<13;i++)assert(p.add_column());assert(p.count()==130);p.level.waves=20;assert(!p.add_column());
 for(int mode=0;mode<2;mode++){l.group_by_color=!mode;p.reset(l);assert(!p.has_moves());int n=p.add_rain();assert(n>=l.rain_min&&n<=l.rain_max&&p.has_moves());check_grid(p);assert(p.add_rain()>0);check_grid(p);p.level.rain_trigger_mode=0;assert(!p.add_rain());p.level.rain_trigger_mode=1;
  p.reset(l);p.grid[12][0]={1,1,++p.serial};assert(!p.has_moves());assert(p.add_rain()&&p.has_moves());check_grid(p);
 }
 p.reset(l);for(int x=0;x<l.columns;x++)for(int y=0;y<l.rows;y++)p.grid[x][y]={(x+y)%4,0,++p.serial};assert(!p.has_moves()&&p.rain_space()==0&&!p.add_rain());
 l.rows=3;l.columns=2;l.wave_min_rows=l.wave_max_rows=3;l.rain_min=l.rain_max=7;p.reset(l);assert(p.add_rain()==6);check_grid(p);l.rain_enabled=false;p.reset(l);assert(!p.add_rain());
 GameManager g;load(g);g.start(1);assert(g.pots.added==1);g.state=GameManager::PAUSE;int a=g.pots.added;g.tick(10000);assert(g.pots.added==a);
 // Click decision depends on the clicked cell, even when another valid group exists.
 auto fixture=[&](){g.start(1);g.motion_ms=0;g.pots.reset(g.pots.level);g.pots.added=1;g.pots.grid[12][0]={0,0,++g.pots.serial};g.pots.grid[12][1]={0,1,++g.pots.serial};g.pots.grid[10][0]={1,0,++g.pots.serial};assert(g.pots.has_moves());};
 fixture();assert(g.scene_click(12,0)==-1&&g.anim>0&&g.rain_count==0);g.tick(17);assert(g.scores.score==0);g.tick(140);assert(g.removed==2);
 fixture();assert(g.scene_click(10,0)>0&&g.anim==0&&g.rain_count==1&&g.scores.score==0&&g.pots.added==1);assert(g.pots.grid[10][0].type==1&&g.pots.grid[12][0].type==0);check_grid(g.pots);int c=g.pots.count();assert(g.scene_click(0,0)==0&&g.pots.count()==c);
 fixture();assert(g.scene_click(0,0)>0&&g.rain_count==1);fixture();assert(g.scene_click(-1,-1)>0&&g.rain_count==1);
 fixture();g.pots.level.rain_trigger_mode=0;assert(g.scene_click(0,0)==0);g.pots.level.rain_trigger_mode=1;g.state=GameManager::PAUSE;assert(g.scene_click(0,0)==0);
 g.start(1);g.motion_ms=0;g.pots.reset(g.pots.level);g.pots.added=1;g.wave_clock=g.wave_interval()-100;assert(g.rain()>0&&g.scores.score==0&&g.pots.added==1&&g.wave_clock==g.wave_interval()-100);int count=g.pots.count();assert(!g.rain());g.next();g.attack(0,0);assert(!g.anim&&g.pots.count()==count);g.state=GameManager::PAUSE;int motion=g.motion_ms;g.tick(1000);assert(g.motion_ms==motion);g.state=GameManager::PLAY;g.tick(17);assert(g.pots.added==1);
 // Goal is disabled by default, fails below threshold and passes exactly at threshold.
 g.start(1);g.pots.added=g.pots.level.waves;g.pots.level.target_score=20;g.scores.score=19;g.tick(g.pots.level.end_delay_ms);assert(g.state==GameManager::FAIL&&g.failure==GameManager::SCORE_TARGET);
 g.start(1);g.pots.added=g.pots.level.waves;g.pots.level.target_score=20;g.scores.score=20;g.tick(g.pots.level.end_delay_ms);assert(g.state==GameManager::WIN);
 g.start(1);g.pots.added=g.pots.level.waves;g.pots.level.target_score=0;g.tick(g.pots.level.end_delay_ms);assert(g.state==GameManager::WIN);assert(LevelData().target_score==0);
 // Rescue-only pairing is random while another group exists, assisted when no
 // moves exist. It must not change the requested click-to-rain trigger.
 l=LevelData();l.rain_match_mode=1;p.reset(l);assert(p.add_rain()>0&&p.has_moves());
 p.reset(l);p.grid[12][0]={0,0,++p.serial};p.grid[12][1]={0,0,++p.serial};PotManager random_rain=p;random_rain.level.rain_match_mode=2;
 assert(p.add_rain()==random_rain.add_rain()&&p.rng==random_rain.rng);for(int x=0;x<13;x++)for(int y=0;y<10;y++)assert(p.grid[x][y].type==random_rain.grid[x][y].type&&p.grid[x][y].uid==random_rain.grid[x][y].uid);
 // Automatic insertion carries animation overrun; manual NEXT starts a fresh
 // interval. Pausing never consumes either clock and input stays animation-safe.
 g.start(11);g.pots.level.interval_ms=1000;g.pots.level.burst_every=0;g.pots.level.keep_wave_overflow=true;g.wave_clock=990;g.motion_ms=100;
 g.tick(50);assert(g.pots.added==1&&g.wave_clock==1040);g.tick(50);assert(g.pots.added==2&&g.wave_clock==90);int clock=g.wave_clock;g.state=GameManager::PAUSE;g.tick(1000);assert(g.wave_clock==clock);g.state=GameManager::PLAY;g.tick(g.pots.level.move_ms);g.wave_clock=800;g.next();assert(g.pots.added==3&&g.wave_clock==0);
 ScoreManager scoring;assert(scoring.calculate(2,10,1)==20&&scoring.calculate(3,10,1)==60&&scoring.calculate(4,10,1)==120);
 LevelData legacy=parse_level("{}",7);assert(legacy.target_score==0&&legacy.rain_match_mode==0&&!legacy.keep_wave_overflow);
 for(int id=1;id<=150;id++){auto d=g.levels.get(id);assert(d.columns==15&&d.target_score>0&&d.score_mode==1&&d.min_group==2&&d.rain_trigger_mode==1);}
 // Fifteen full columns hold150 distinct pots. The16th column may not overwrite
 // them, and a150-pot flood-fill can be eliminated without overrunning storage.
 l=LevelData();l.waves=16;p.reset(l);for(int i=0;i<15;i++)assert(p.add_column());assert(p.count()==150&&!p.add_column());check_grid(p);
 for(int x=0;x<15;x++)for(int y=0;y<10;y++)p.grid[x][y].type=(x+y)%4;
 assert(!p.add_rain()&&p.count()==150);for(int x=0;x<15;x++)for(int y=0;y<10;y++)p.grid[x][y].type=0;
 assert(p.group(14,9,group)==150&&p.hit(14,9)==150&&p.count()==0);check_grid(p);
 assert(parse_level("{\"columns\":15,\"rain_max_count\":200}",1).rain_max==150);
 assert(parse_level("{\"columns\":99}",1).columns==15&&parse_level("{\"columns\":1}",1).columns==2);
 // Shared layout keeps every cell and its click target on the logical canvas.
 for(int cols=2;cols<=15;cols++){BoardLayout board(cols);assert(board.right()<=1222);for(int x=0;x<cols;x++)for(int y=0;y<10;y++){int a,b;assert(board.cell(board.x(x)+board.cell_width/2,board.y(y)+24,10,a,b)&&a==x&&b==y);}int a,b;assert(!board.cell(board.right(),641,10,a,b));}
 BoardLayout board(15);assert(board.cell_width==50&&board.x(14)==1168&&board.right()==1218);
 // New and old pots use the original linear slide, with exactly one-cell spacing.
 EffectManager fx;p.reset(l);p.add_column();Pot old=p.grid[14][0];fx.capture(p);p.add_column();Pot fresh=p.grid[14][0];fx.begin(150);int px,py;
 fx.position(fresh,14,0,board,px,py);assert(px==1218&&py==617);fx.position(old,13,0,board,px,py);assert(px==1168);
 fx.tick(75);fx.position(fresh,14,0,board,px,py);assert(px==1193);fx.position(old,13,0,board,px,py);assert(px==1143);
 for(int time=0;time<=150;time+=3){fx.elapsed=time;fx.moving=time<150;int ox,oy,nx,ny;fx.position(old,13,0,board,ox,oy);fx.position(fresh,14,0,board,nx,ny);assert(nx-ox==50&&nx>=1168&&nx<=1218);}
 fx.tick(150);fx.position(fresh,14,0,board,px,py);assert(!fx.moving&&px==1168&&py==617);
 // Rain and gravity retain their animations. First column is immediately usable.
 fx=EffectManager();fx.begin(650,true);fx.position(fresh,14,0,board,px,py);assert(py<0);fx.tick(650);fx.position(fresh,14,0,board,px,py);assert(py==617);
 g.start(1);assert(!g.motion_ms&&g.pots.added==1);assert(g.scene_click(-1,-1)>0&&g.motion_ms==g.pots.level.rain_ms);
 // Changing the stage ID must not alter the imported stage50 RNG or timing.
 std::ifstream baseline_file("ASuePotGame/tests/fixtures/v08_level50.json");assert(baseline_file);std::stringstream bytes;bytes<<baseline_file.rdbuf();
 GameManager reference;reference.levels.data[49]=parse_level(bytes.str().c_str(),50);reference.start(50);g.start(1);
 BalancePlayer old_player(100,3),new_player(100,3);int compared=0;
 while(g.state==GameManager::PLAY&&compared++<200000){
  old_player.act(reference,17);new_player.act(g,17);reference.tick(17);g.tick(17);
  assert(g.state==reference.state&&g.failure==reference.failure&&g.scores.score==reference.scores.score&&g.wave_clock==reference.wave_clock&&g.pots.added==reference.pots.added&&g.pots.rng==reference.pots.rng);
  for(int x=0;x<15;x++)for(int y=0;y<10;y++){Pot a=g.pots.grid[x][y],b=reference.pots.grid[x][y];assert(a.type==b.type&&a.uid==b.uid&&a.variant==b.variant);}
 }
 assert(compared<200000&&g.state==GameManager::WIN);printf("PASS: new level1 reproduces old level50 step-by-step, score=%d, %d ticks\n",g.scores.score,compared);
 long rain_total=0;
 for(int id=1;id<=150;id++){bool solved=false;for(int attempt=0;attempt<6&&!solved;attempt++){g.start(id);BalancePlayer player(attempt<2?100:attempt<4?150:250,attempt%2?4:3);int steps=0;while(g.state==GameManager::PLAY&&steps++<200000){player.act(g,17);g.tick(17);check_grid(g.pots);}assert(steps<200000);solved=g.state==GameManager::WIN;if(solved){printf("Level %02d: state=%d columns=%d/%d steps=%d rain=%d score=%d target=%d policy=%d delay=%d\n",id,g.state,g.pots.added,g.pots.level.waves,steps,g.rain_count,g.scores.score,g.pots.level.target_score,player.policy,player.delay);rain_total+=g.rain_count;}}assert(solved);}
 g.start(20);while(g.state==GameManager::PLAY)g.tick(17);assert(g.state==GameManager::FAIL);g.start(g.level_id);assert(g.level_id==20&&g.state==GameManager::PLAY&&g.pots.added==1);
 for(int id=1;id<=150;id++){g.start(id);int steps=0;while(g.state==GameManager::PLAY&&steps++<200000)g.tick(17);assert(g.state==GameManager::FAIL);assert(g.failure==(g.pots.level.waves>g.pots.level.columns?GameManager::OVERFLOW:GameManager::SCORE_TARGET));g.start(g.level_id);assert(g.level_id==id&&g.state==GameManager::PLAY&&g.pots.added==1);}
 long ticks=0,restarts=0;g.start(1);for(int ms=0;ms<30*60*1000;ms+=17){if((restarts%3)!=1&&!g.anim&&!g.motion_ms){if(!best_move(g))g.rain();}g.tick(17);ticks++;if(g.state!=GameManager::PLAY)g.start((++restarts%150)+1);check_grid(g.pots);}
 printf("PASS: 150 actual JSON levels solved; rain=%ld; clusters/gravity/pause/rain guards/full board; level20 failure/retry; simulated30min=%ld ticks %ld restarts; fixed game storage=%zu bytes\n",rain_total,ticks,restarts,sizeof(g));
}
