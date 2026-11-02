#pragma once
struct LevelData {
 int id=1, rows=10, columns=15, waves=13, seed=171, min_group=2, points=10, invalid_penalty=0;
 int interval_ms=7000, attack_delay_ms=140, attack_duration_ms=420, fall_ms=220, move_ms=220, popup_ms=850, score_mode=0, end_delay_ms=1600;
 bool group_by_color=true, compact_empty_columns=true;
 // 以下为重制版可调规则，不能据此认定原版数值。
 bool rain_enabled=true;int rain_min=4,rain_max=7,rain_ms=650,rain_trigger_mode=1;
 // 0总是辅助配对，1仅全场无组合时救场，2完全随机。触发方式不变。
 int rain_match_mode=0;
 int wave_min_rows=10, wave_max_rows=10, repeat_percent=0, pattern_mode=0;
 int weights[4]={25,25,25,25};
 int burst_every=0,burst_interval_ms=3500,target_score=0;
 bool keep_wave_overflow=false;
};
