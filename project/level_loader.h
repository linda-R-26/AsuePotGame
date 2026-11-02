#pragma once
#include "level_data.h"
#include "util.h"
// UI和自动测试共用同一个JSON参数读取器；数据只用整数和0/1开关。
inline LevelData parse_level(const char*b,int id){
 LevelData l;l.id=id;
 l.rows=clamp(value(b,"\"rows\"",10),1,10);l.columns=clamp(value(b,"\"columns\"",15),2,15);
 l.waves=clamp(value(b,"\"wave_count\"",13+3*(id-1)),1,10000);l.seed=value(b,"\"seed\"",id==3?246:171+(id-1)*37);
 l.min_group=clamp(value(b,"\"min_group\"",2),1,l.rows*l.columns);l.points=clamp(value(b,"\"points_per_pot\"",10),0,10000);
 l.invalid_penalty=clamp(value(b,"\"invalid_penalty\"",0),0,10000);
 l.interval_ms=clamp(value(b,"\"next_wave_delay_ms\"",7000),1000,60000);
 l.attack_delay_ms=clamp(value(b,"\"attack_delay_ms\"",140),0,1500);l.attack_duration_ms=clamp(value(b,"\"attack_duration_ms\"",420),l.attack_delay_ms+1,3000);
 l.fall_ms=clamp(value(b,"\"pot_fall_duration_ms\"",220),1,3000);l.move_ms=clamp(value(b,"\"pot_move_duration_ms\"",220),1,3000);
 l.popup_ms=clamp(value(b,"\"score_popup_duration_ms\"",850),100,5000);l.score_mode=clamp(value(b,"\"score_mode\"",0),0,2);
 l.end_delay_ms=clamp(value(b,"\"level_end_delay_ms\"",1600),1,5000);
 l.group_by_color=value(b,"\"group_by_color\"",1)!=0;l.compact_empty_columns=value(b,"\"compact_empty_columns\"",1)!=0;
 l.rain_enabled=value(b,"\"rain_enabled\"",1)!=0;l.rain_min=clamp(value(b,"\"rain_min_count\"",4),1,l.rows*l.columns);
 l.rain_max=clamp(value(b,"\"rain_max_count\"",7),l.rain_min,l.rows*l.columns);l.rain_ms=clamp(value(b,"\"rain_duration_ms\"",650),100,3000);
 l.rain_trigger_mode=clamp(value(b,"\"rain_trigger_mode\"",1),0,1);
 l.rain_match_mode=clamp(value(b,"\"rain_match_mode\"",0),0,2);
 l.wave_min_rows=clamp(value(b,"\"wave_min_rows\"",l.rows),1,l.rows);l.wave_max_rows=clamp(value(b,"\"wave_max_rows\"",l.rows),l.wave_min_rows,l.rows);
 l.repeat_percent=clamp(value(b,"\"vertical_repeat_percent\"",0),0,100);l.pattern_mode=clamp(value(b,"\"pattern_mode\"",0),0,2);
 l.weights[0]=clamp(value(b,"\"weight_blue\"",25),0,1000);l.weights[1]=clamp(value(b,"\"weight_white\"",25),0,1000);
 l.weights[2]=clamp(value(b,"\"weight_beige\"",25),0,1000);l.weights[3]=clamp(value(b,"\"weight_brown\"",25),0,1000);
 if(!l.weights[0]&&!l.weights[1]&&!l.weights[2]&&!l.weights[3])for(int i=0;i<4;i++)l.weights[i]=25;
 l.burst_every=clamp(value(b,"\"burst_every_columns\"",0),0,10000);l.burst_interval_ms=clamp(value(b,"\"burst_interval_ms\"",l.interval_ms),1000,60000);
 l.target_score=clamp(value(b,"\"target_score\"",0),0,1000000);
 l.keep_wave_overflow=value(b,"\"keep_wave_overflow\"",0)!=0;
 return l;
}
