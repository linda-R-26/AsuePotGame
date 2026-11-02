#pragma once
// 显示、悬停、点击和特效共用同一布局；15列仍在1280×720画布内。
struct BoardLayout {
 enum { LEFT=468, BASE_Y=617, CELL_HEIGHT=49, AREA_WIDTH=754, SCREEN_WIDTH=1280 };
 int columns,cell_width;
 BoardLayout(int n):columns(n<2?2:n>15?15:n),cell_width(AREA_WIDTH/columns){if(cell_width>58)cell_width=58;}
 int x(int column)const{return LEFT+column*cell_width;}
 int y(int row)const{return BASE_Y-row*CELL_HEIGHT;}
 int right()const{return x(columns);}
 bool cell(int mx,int my,int rows,int&column,int&row)const{
  column=row=-1;
  if(mx<LEFT||mx>=right()||my<y(rows-1)||my>=BASE_Y+CELL_HEIGHT)return false;
  column=(mx-LEFT)/cell_width;row=(BASE_Y+CELL_HEIGHT-1-my)/CELL_HEIGHT;return true;
 }
};
