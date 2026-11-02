#pragma once
#include "../project/core.h"
#include "../project/util.h"
struct SaveManager {
 SaveData data; wchar_t path[520]={},temp[520]={},backup[520]={};bool error=false,recovered=false;
 unsigned hash(const char*s){unsigned h=2166136261u;while(*s){h^=(unsigned char)*s++;h*=16777619u;}return h&0x7fffffff;}
 void payload(char*b,const SaveData&d){b[0]=0;append(b,"{\"version\":1,\"unlocked\":");number(b,d.unlocked);append(b,",\"music\":");number(b,d.music);append(b,",\"sfx\":");number(b,d.sfx);append(b,",\"fullscreen\":");number(b,d.fullscreen);append(b,",\"width\":");number(b,d.width);append(b,",\"height\":");number(b,d.height);for(int i=0;i<256;i++){append(b,",\"best_");number(b,i+1);append(b,"\":");number(b,d.best[i]);append(b,",\"done_");number(b,i+1);append(b,"\":");number(b,d.completed[i]);}append(b,"}");}
 bool load(const wchar_t*p){char b[32768]={};if(!read_file(p,b,32768))return false;int checksum=value(b,"\"checksum\"",-1);if(checksum<0||value(b,"\"version\"",-1)!=1)return false;SaveData d;d.unlocked=value(b,"\"unlocked\"",0);d.music=value(b,"\"music\"",-1);d.sfx=value(b,"\"sfx\"",-1);int fs=value(b,"\"fullscreen\"",-1);d.fullscreen=fs;d.width=value(b,"\"width\"",0);d.height=value(b,"\"height\"",0);
 if(d.unlocked<1||d.unlocked>256||d.music<0||d.music>100||d.sfx<0||d.sfx>100||fs<0||fs>1||!((d.width==1280&&d.height==720)||(d.width==1920&&d.height==1080)))return false;
 for(int i=0;i<256;i++){char k[40]="\"best_";number(k,i+1);append(k,"\"");d.best[i]=value(b,k,-1);char t[40]="\"done_";number(t,i+1);append(t,"\"");int done=value(b,t,-1);if(d.best[i]<0||done<0||done>1)return false;d.completed[i]=done;}
 char canon[32768]={};payload(canon,d);if((int)hash(canon)!=checksum)return false;data=d;return true;}
 void init(){__imp_GetEnvironmentVariableW(L"APPDATA",path,400);if(!path[0]){error=true;return;}wcat(path,L"\\ASuePotGame");__imp_CreateDirectoryW(path,0);wcat(path,L"\\save.json");wcopy(temp,path);wcat(temp,L".tmp");wcopy(backup,path);wcat(backup,L".bak");if(!load(path)&&load(backup))recovered=true;}
 bool save(){if(!path[0]){error=true;return false;}char b[32768]={};payload(b,data);unsigned h=hash(b);b[slen(b)-1]=0;append(b,",\"checksum\":");number(b,h);append(b,"}");HANDLE f=__imp_CreateFileW(temp,0x40000000,0,0,2,0,0);if(f==(HANDLE)-1){error=true;return false;}U32 n=0;bool ok=__imp_WriteFile(f,b,slen(b),&n,0)&&n==(U32)slen(b);ok=__imp_FlushFileBuffers(f)&&ok;__imp_CloseHandle(f);SaveData keep=data;if(load(path))__imp_CopyFileW(path,backup,0);data=keep;if(ok)ok=__imp_MoveFileExW(temp,path,9)!=0;error=!ok;return ok;}
 void complete(int level,int score){if(level<1||level>256)return;data.completed[level-1]=true;if(score>data.best[level-1])data.best[level-1]=score;if(level<256&&data.unlocked<level+1)data.unlocked=level+1;save();}
};
