#pragma once
#include "level_data.h"
struct LevelManager { LevelData data[256]; int current=1,count=3; LevelManager(){for(int i=0;i<3;i++){data[i].id=i+1;data[i].waves=13+3*i;data[i].seed=i==2?246:171+i*37;}} LevelData get(int id){return data[id-1];} };
