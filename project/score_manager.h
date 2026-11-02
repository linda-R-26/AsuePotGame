#pragma once
struct ScoreManager { int score=0; int calculate(int n,int points,int mode){return mode==1?n*(n-1)*points:mode==2?n*n*points:n*points;} void add(int n){score+=n; if(score<0)score=0;} };
