#pragma once
#include "resource.h"

struct SettingStruct {
	bool mSpawnModeOpen;
	bool mFaster;
	bool mMoreZombie;
	bool mUseScore;
	bool mCoDefenceModeOpen;
	bool mUseCrater;
	unsigned int mConnetPlayersMax;
};

class SettingsDialog {
public:
	SettingStruct mStruct;
	bool mIsClient = false;
public:
	SettingsDialog();
	~SettingsDialog();


	void Show();
};

