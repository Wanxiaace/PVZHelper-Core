#pragma once
#ifndef __HELPER_DLL_MAIN__
#define __HELPER_DLL_MAIN__

#include "Common.h"
#include "SocketInterface.h"

extern volatile ScoreInteger g_Score;
extern Client g_cl;
extern Server g_ser;
extern ServerInfo g_info;
extern volatile lpLawnDialog g_LawnDialog;
extern SettingsDialog g_settingdlg;
extern volatile bool g_is_client;
extern volatile void* g_lp_map;
extern volatile bool g_show_detailed;
extern volatile bool g_is_playing;

void InitAll();
void DoMain();
void QuitAll();
void TryToCheckOut();

void InitSocket(bool IsClient,Client cl,Server ser);

#endif // !__HELPER_DLL_MAIN__
