#include "DllMain.h"
#include <CommCtrl.h>
#include "SettingDialog.h"
#include "App.h"
#include "pvzclass.h"

SettingsDialog* g_DlgMain = nullptr;

INT_PTR CALLBACK SettingsDialogCallBack(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		CheckDlgButton(hDlg, IDC_CHECK1, g_DlgMain->mStruct.mSpawnModeOpen);
		CheckDlgButton(hDlg, IDC_CHECK2, g_DlgMain->mStruct.mFaster);
		CheckDlgButton(hDlg, IDC_CHECK3, g_DlgMain->mStruct.mMoreZombie);
		CheckDlgButton(hDlg, IDC_CHECK4, g_DlgMain->mStruct.mUseScore);
		CheckDlgButton(hDlg, IDC_CHECK5, g_DlgMain->mStruct.mCoDefenceModeOpen);
		CheckDlgButton(hDlg, IDC_CHECK6, g_DlgMain->mStruct.mUseCrater);

		HWND hSlider = GetDlgItem(hDlg, IDC_SLIDER1);
		SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELONG(2, 5));
		SendMessage(hSlider, TBM_SETPOS, 1,g_DlgMain->mStruct.mConnetPlayersMax);
		char info[20] = { 0 };
		snprintf(info, 20, "当前:%d人", g_DlgMain->mStruct.mConnetPlayersMax);
		SetDlgItemTextA(hDlg, TEXT_PEOPLE, info);

		if(g_DlgMain->mStruct.mCoDefenceModeOpen)
			EnableWindow(GetDlgItem(hDlg, IDC_SLIDER1), FALSE);

		if (g_DlgMain->mIsClient) {
			SetDlgItemText(hDlg, TEXT_GREET, TEXT("你好: 成员(游戏模式由房主选择)"));
			HWND hCheckBox1 = GetDlgItem(hDlg, IDC_CHECK1);
			EnableWindow(hCheckBox1, FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_SLIDER1), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK2), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK3), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK4), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK5), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK6), FALSE);
		}
		else {
			SetDlgItemText(hDlg, TEXT_GREET, TEXT("你好: 房主"));
		}

		if (g_is_playing) {
			HWND hCheckBox1 = GetDlgItem(hDlg, IDC_CHECK1);
			EnableWindow(hCheckBox1, FALSE);

			EnableWindow(GetDlgItem(hDlg, IDC_SLIDER1), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK2), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK3), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK4), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK5), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK6), FALSE);
			EnableWindow(GetDlgItem(hDlg, TEXT_GAME_IN), FALSE);
		}
		return TRUE;
	}
	case WM_CLOSE:
		EndDialog(hDlg,IDCANCEL);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDC_CHECK1:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_DlgMain->mStruct.mSpawnModeOpen = IsDlgButtonChecked(hDlg, IDC_CHECK1);
				if (g_DlgMain->mStruct.mSpawnModeOpen)
					_hp_Show_Tip("召唤模式开启，关卡中，可以花费225阳光向除了自己的场地召唤墓碑");
			}
			return TRUE;
		case IDC_CHECK2:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_DlgMain->mStruct.mFaster = IsDlgButtonChecked(hDlg, IDC_CHECK2);
			}
			return TRUE;
		case IDC_CHECK3:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_DlgMain->mStruct.mMoreZombie = IsDlgButtonChecked(hDlg, IDC_CHECK3);
			}
			return TRUE;
		case IDC_CHECK4:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_DlgMain->mStruct.mUseScore = IsDlgButtonChecked(hDlg, IDC_CHECK4);
			}
			return TRUE;
		case IDC_CHECK5:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_DlgMain->mStruct.mCoDefenceModeOpen = IsDlgButtonChecked(hDlg, IDC_CHECK5);
				g_DlgMain->mStruct.mConnetPlayersMax = 2;
			}
			if (g_DlgMain->mStruct.mCoDefenceModeOpen)
				EnableWindow(GetDlgItem(hDlg, IDC_SLIDER1), FALSE);
			else
				EnableWindow(GetDlgItem(hDlg, IDC_SLIDER1), TRUE);
			{
				char info[20] = { 0 };
				snprintf(info, 20, "当前:%d人", g_DlgMain->mStruct.mConnetPlayersMax);
				SetDlgItemTextA(hDlg, TEXT_PEOPLE, info);
				HWND hSlider = GetDlgItem(hDlg, IDC_SLIDER1);
				SendMessage(hSlider, TBM_SETPOS, 1, g_DlgMain->mStruct.mConnetPlayersMax);
			}
			return TRUE;
		case IDC_CHECK6:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_DlgMain->mStruct.mUseCrater = IsDlgButtonChecked(hDlg, IDC_CHECK6);
			}
			return TRUE;
		default:
			break;
		}
		break;
	case WM_HSCROLL:
	{
		int ctrlID = (int)GetWindowLongPtr((HWND)lParam, GWL_ID);
		if (ctrlID == IDC_SLIDER1) {
			int sliderPos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
			g_DlgMain->mStruct.mConnetPlayersMax = sliderPos;
			char info[20] = { 0 };
			snprintf(info, 20, "当前:%d人", sliderPos);
			SetDlgItemTextA(hDlg, TEXT_PEOPLE, info);
		}
	}
		break;
	default:
		break;
	}
	return FALSE;
}

SettingsDialog::SettingsDialog()
{
	mStruct.mSpawnModeOpen = true;
	mStruct.mConnetPlayersMax = 2;
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::Show()
{
	g_DlgMain = this;
	HINSTANCE hInst = GetModuleHandleA("MainDll.dll");

	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1),NULL, SettingsDialogCallBack);
}
