#pragma once
#include "win_api.h"
inline int slen(const char*s){int n=0;while(s[n])n++;return n;}
inline int wlen(const wchar_t*s){int n=0;while(s[n])n++;return n;}
inline void wcopy(wchar_t*d,const wchar_t*s){int i=0;while((d[i]=s[i]))i++;}
inline void wcat(wchar_t*d,const wchar_t*s){wcopy(d+wlen(d),s);}
inline void append(char*d,const char*s){int i=slen(d),j=0;while((d[i++]=s[j++])){}}
inline void number(char*d,int n){char t[24];int k=0;if(n<0){append(d,"-");n=-n;}do{t[k++]=(char)('0'+n%10);n/=10;}while(n);char s[24];for(int i=0;i<k;i++)s[i]=t[k-i-1];s[k]=0;append(d,s);}
inline void wnumber(wchar_t*d,int n,int digits=0){char b[24]={};number(b,n);int k=slen(b),i=wlen(d);while(k<digits){d[i++]=L'0';digits--;}for(int j=0;j<k;j++)d[i++]=b[j];d[i]=0;}
inline int value(const char*s,const char*key,int def){int n=slen(key);for(int i=0;s[i];i++){int j=0;while(j<n&&s[i+j]&&s[i+j]==key[j])j++;if(j!=n)continue;int p=i+n;while(s[p]&&s[p]!=':')p++;if(!s[p])return def;p++;while(s[p]==' '||s[p]=='\n')p++;int sign=1;if(s[p]=='-'){sign=-1;p++;}if(s[p]<'0'||s[p]>'9')return def;I64 r=0;while(s[p]>='0'&&s[p]<='9'){r=r*10+s[p++]-'0';if(r>2147483647)return def;}return r*sign;}return def;}
inline bool read_file(const wchar_t*p,char*b,int cap){HANDLE h=__imp_CreateFileW(p,0x80000000,1,0,3,0,0);if(h==(HANDLE)-1)return false;U32 n=0;bool ok=__imp_ReadFile(h,b,cap-1,&n,0)!=0;__imp_CloseHandle(h);b[n]=0;return ok&&n<(U32)(cap-1);}
inline int clamp(int n,int a,int b){return n<a?a:n>b?b:n;}
