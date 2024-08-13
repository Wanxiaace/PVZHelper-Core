#pragma once
#include "Common.h"
#include "pvzclass.h"

namespace App {
	void PreNewGame(unsigned int mode, int level = 0);
	void Safe_PreNewGame(unsigned int mode, int level = 0);
	void InitApp();
	void WaitPvZ();
	void EndWait();
	lpLawnDialog DoDialog(int button_mode,
		SexyString header_name,
		SexyString lines_name,
		SexyString button1_name = "OK",
		SexyString button2_name = "Cancel",
		int dialog_id = 0xE8);

	lpLawnDialog Safe_DoDialog(int g_BtnMode,
		SexyString g_HeaderName,
		SexyString g_LinesName,
		SexyString g_Btn1Name = "OK",
		SexyString g_Btn2Name = "Cancel",
		int g_DialogId = 0xE8);
	void GameShowTip(SexyString str);

	int Safe_LawnMessageBox(
		int g_BtnMode,
		const char* g_HeaderName,
		const char* g_LinesName,
		const char* g_Btn1Name = "OK",
		const char* g_Btn2Name = "Cancel",
		int g_DialogId = 0xC4);
	bool Safe_KillDialog(lpLawnDialog tar);
	bool Is3DAccelerated();

}
void PVZDelete(void* _Block);
unsigned int PVZNew(int _Size);