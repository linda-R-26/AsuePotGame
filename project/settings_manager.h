#pragma once
#include "../save_system/save_manager.h"
struct SettingsManager {
 HWND window=0;SaveManager*save=0;
 void apply(){auto&d=save->data;if(d.fullscreen){__imp_SetWindowLongPtrW(window,-16,0x80000000);__imp_SetWindowPos(window,0,0,0,__imp_GetSystemMetrics(0),__imp_GetSystemMetrics(1),0x20);}else{__imp_SetWindowLongPtrW(window,-16,0x00cf0000);__imp_SetWindowPos(window,0,60,40,d.width+16,d.height+39,0x20);} }
};
