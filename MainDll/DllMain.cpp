#include "DllMain.h"
#include "GamePlant.h"
#include "pvzclass.h"
#include "Common.h"
#include "Hacker.h"
#include "Graphics.h"
#include "App.h"
#include "resource.h"
#include "Font.h"
#include "GameZombie.h"
#include "NumberParticle.h"
#include "SettingDialog.h"
#include <vector>
#include <thread>
#include "Board.h"
#include <time.h>
#include "SexyStruct.h"
#include "LawnStruct.h"

volatile lpLawnDialog g_LawnDialog = 0x0;

const int DRAW_PATCH_ADDR = 0x5390E1;
const int GET_GRA_PATCH_ADDR = 0x538F62;
const int ZOMBIE_DIE_PATCH_ADDR = 0x530510;
const int ZOMBIE_SPAWN_WAVES_ADDR = 0x409881;
const int BOARD_END_DRAW_ADDR = 0x41AE4E;
const int BOARD_DRAW_BACKDROP_END = 0x41665B;
const int SEED_CHOOSE_SCREEN_CTEATE_BUTTON = 0x4835DF;
const int RESTART_BUTTON_END = 0x45D339;
const int PUT_LAWN_ITEMS = 0x43A6D6;
const int ZOMBIE_DROP_HEAD = 0x529A30;
const int PLANT_DIE = 0x4679B0;

volatile bool g_is_playing = false;
volatile void* g_lp_map = nullptr;
volatile bool g_show_detailed = true;
volatile lpGraphics SrcG = 0;
ServerInfo g_info;

//得分
volatile ScoreInteger g_Score;
std::vector<TextParticle*> g_particles;
Client g_cl = Client();
Server g_ser = Server();
volatile bool g_is_client = false;
SettingsDialog g_settingdlg = SettingsDialog();

Sexy::ButtonListener m_btnltr;
bool m_is_F3_down = false;
bool m_is_F4_down = false;
bool m_is_F7_down = false;
bool m_is_game_start = false;

Font* m_font1 = nullptr;
ULONGLONG m_tick_cache = 0;

unsigned int m_zombie_spawn_plus_num_address = 0;
bool m_zombie_plus_count = 0;

std::vector<Lawn::_Zombie*> m_dead_zombies;

//游戏的特殊按钮
Sexy::PButton m_tomb_spawn_button = 0x0;

enum GameButtonID {
	BUTTON_TOMB = 150,
	BUTTON_READY = 282,
};

static void __stdcall ButtonOn(int the_id) {
	//_hp_Show_Tip("Tomb On");
	switch (the_id)
	{
	case BUTTON_TOMB: 
	{
		if (!g_is_playing)
			break;
		if (PVZ::GetBoard()->Sun >= 225) {
			PVZ::GetBoard()->Sun -= 225;
			int r_x = rand() % 9;
			int r_y = rand() % 5;
			App::GameShowTip("Grave Stone Party!!!");
			PlayerSendTombSpawnMessage(r_x, r_y);
		}
		else {
			App::GameShowTip("You Don't Have Enough Sun");
		}
		//Board::SpawnTombInRect(0,0,9,5,PVZ::GetBoard().get());
		break;
	}
	default:
		break;
	}
}

static void KeyUpdateThread() {
	while (true)
	{
		auto kd = GetAsyncKeyState(VK_F3);
		if ((kd & 0x8000) && !m_is_F3_down) {
			if (g_show_detailed)
				g_show_detailed = false;
			else
				g_show_detailed = true;
			m_is_F3_down = true;

			if (g_settingdlg.mStruct.mSpawnModeOpen && g_is_playing) {
				if (g_show_detailed) {
					if (m_tomb_spawn_button == 0x0) {
						auto li = Sexy::MakeListener(&m_btnltr);
						m_tomb_spawn_button = Sexy::MakeButton(Draw::ToString("Tomb"), li, BUTTON_TOMB);
						Sexy::ResizeButton(m_tomb_spawn_button, 0, 170, 117, 50);
						Sexy::AddToManager(m_tomb_spawn_button);
					}
				}
				else {
					if (m_tomb_spawn_button) {
						Sexy::RemoveFromManager(m_tomb_spawn_button);
						Sexy::FreeWidget(m_tomb_spawn_button);
						m_tomb_spawn_button = 0x0;
					}
				}
			}
			
			Sleep(300);
		}
		else {
			m_is_F3_down = false;//SettingsDialog
		}

		kd = GetAsyncKeyState(VK_F4);
		if ((kd & 0x8000) && !m_is_F4_down) {
			m_is_F4_down = true;
			g_settingdlg.mIsClient = g_is_client;
			g_settingdlg.Show();
			if(!g_is_client)
				g_ser.All_SendSettingsStruct();
			Sleep(300);
		}
		else
			m_is_F4_down = false;

		kd = GetAsyncKeyState(VK_F7);
		if ((kd & 0x8000) && !m_is_F4_down) {
			m_is_F7_down = true;
			lpBoard bd = Board::TryGetBoard();
			if (bd) {
				Board::SpawnCardAt(0,0,5);
			}
			Sleep(300);
		}
		else
			m_is_F7_down = false;
		Sleep(50);
	}
}

static void ParticleUpdate(){
	if(m_tick_cache == -1)
		m_tick_cache = GetTickCount64();
	ULONGLONG tick = GetTickCount64();
	size_t s = g_particles.size();
	for (size_t i = 0; i < s; i++)
	{
		g_particles[i]->Update(tick - m_tick_cache);
		if (!g_particles[i]->GetExist()) {
			g_particles.erase(g_particles.begin() + i);
			i -= 1;
			s -= 1;
		}
	}
	m_tick_cache = tick;
}

static void ParticleDraw(lpGraphics gra) {
	for (auto & x : g_particles)
	{
		x->Present(gra,m_font1);
	};
}

static void GetGraphics(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
) {
	SrcG = _eax;
}

#include <ddraw.h>
#include "stb_image_write.h"


static void BoardEndDraw(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
) {
	unsigned long* bits = nullptr;
	int width, height, pitch = 0;
	if (App::Is3DAccelerated()) {
		LPDIRECTDRAWSURFACE suf = (LPDIRECTDRAWSURFACE)PVZ::Memory::ReadPointer(0x6a9ec0,0x36C,0x68);
		DDSURFACEDESC desc = { sizeof(DDSURFACEDESC) };
		suf->Lock(NULL, &desc, DDLOCK_SURFACEMEMORYPTR, NULL);
		bits = (unsigned long*)desc.lpSurface;
		width = desc.dwWidth;
		height = desc.dwHeight;
		pitch = desc.lPitch;

		char* bits2 = new char[height * width * 3];

		size_t numPixels = height * pitch / 4;

		for (size_t i = 0; i < numPixels; ++i) {
			uint32_t pixel = (uint32_t)(*(bits + i));
			uint8_t* dst = (uint8_t*)bits2 + i * 3;
			dst[0] = pixel >> 16;
			dst[1] = pixel >> 8;
			dst[2] = pixel;
		}
		delete[] bits2;
		suf->Unlock(NULL);
	}

}

int m_game_tip_pos_x = 10;
int m_game_tip_pos_y = 20;
int m_game_tip_offset_y = 0;

int circle_x = 0;
static void DrawUITop(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
) 
{
	if (!SrcG) 
		return;
	SexyColor White = { 255,0,0,255 };//它曾经是白色
	Gra::SetOutlined(true);
	

	m_game_tip_offset_y = 40;

	if (g_show_detailed)
		m_game_tip_offset_y = 80;

	if (g_show_detailed) {
		Gra::DrawStringPlus(SrcG, m_game_tip_pos_x, m_game_tip_pos_y, "PVZHelper Working", White, m_font1);
		if (!g_is_client) {
			char ptStr[40] = { 0 };
			snprintf(ptStr, sizeof(ptStr), "Port: %d, Now Guests: %d", g_ser.mPort, g_ser.mClientMap.size());
			Gra::DrawStringPlus(SrcG, m_game_tip_pos_x, m_game_tip_pos_y + 20, "You are: Host", White, m_font1);
			Gra::DrawStringPlus(SrcG, m_game_tip_pos_x, m_game_tip_pos_y + 40, ptStr, White, m_font1);
			int ctr = 0;
			for (auto& x : g_ser.mClientStates)
			{
				char ptStr2[40];
				snprintf(ptStr2, sizeof(ptStr2), "Guest: %s(%d) Score: %d", x.second.mUserName.c_str(), x.first, x.second.mScore);
				Gra::DrawStringPlus(SrcG, m_game_tip_pos_x, m_game_tip_pos_y + 60 + ctr*20, ptStr2, White, m_font1);
				m_game_tip_offset_y += 20;
				ctr++;
			}

		}
		else {
			char ptStr[40] = { 0 };
			snprintf(ptStr, sizeof(ptStr), "Port: %d,Token: %d", g_cl.mPort, g_cl.mToken);
			Gra::DrawStringPlus(SrcG, m_game_tip_pos_x, m_game_tip_pos_y + 20, "You are: Guest", White, m_font1);
			Gra::DrawStringPlus(SrcG, m_game_tip_pos_x, m_game_tip_pos_y + 40, ptStr, White, m_font1);
		}
		if (g_is_playing) {
			Gra::DrawStringPlus(SrcG, m_game_tip_pos_x, m_game_tip_offset_y + m_game_tip_pos_y, "Gaming", White, m_font1);
			char scStr[20] = { 0 };
			snprintf(scStr, sizeof(scStr), "TotalScore: %d", g_Score);
			Gra::DrawStringPlus(SrcG, m_game_tip_pos_x, m_game_tip_offset_y + m_game_tip_pos_y + 30, scStr, White, m_font1);
		}
	}

	ParticleUpdate();
	ParticleDraw(SrcG);
}

static void ZombieDie(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
) {
	if (!g_settingdlg.mStruct.mUseScore)
		return;
	auto zombie_itr = std::find(m_dead_zombies.begin(), m_dead_zombies.end(), (Lawn::_Zombie*)_ecx);
	if (zombie_itr != m_dead_zombies.end()) {
		m_dead_zombies.erase(zombie_itr);
		return;
	}
	

	auto Board = PVZ::GetBoard();
	_hp_Check(Board);
	GameZombie a_zombie = GameZombie(_ecx);
	_hp_Check(a_zombie.IsOnBoard());

	if (g_settingdlg.mStruct.mCoDefenceModeOpen) {
		if (a_zombie.X < circle_x)
			return;
	}

	ScoreInteger socre_delta = GetDeltaScore(&a_zombie);;
	g_Score += socre_delta;
	char s_str[10] = {0};
	snprintf(s_str,10,"%d",socre_delta);
	auto p = SpawnNumParticleAt(a_zombie.X, a_zombie.Y, s_str);
	g_particles.push_back(p);

}

static void DrawBoardBackgroundTop(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
)
{
	lpGraphics t_g = *(lpGraphics*)(_ebp + 12);
	if (g_settingdlg.mStruct.mCoDefenceModeOpen) {
		int bkType = 0;
		__asm {
			mov eax, 0x6a9ec0
			mov eax, [eax]
			mov eax, [eax + 0x768]
			mov eax, [eax + 0x554C]
			mov bkType, eax
		}
		int row = (bkType == 2 || bkType == 3) ? 6 : 5;

		circle_x = Board::GridXtoPixelX(4, 0);
		for (size_t i = 0; i < row; i++)
		{
			Gra::DrawImageScaledF(t_g, circle_x, Board::GridYtoPixelY(4, i), 0.45f, 0.45f, *(lpImage*)0x6A7A04);
		}
	}
}

#include "SexyStruct.h"
static void SeedChooseScreenAddButton(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
){
	if (g_is_client) {
		Sexy::_GameButton* ready_button = nullptr;
		__asm {
			mov eax, _ebp
			mov eax, [eax + 0x88]
			mov ready_button, eax
		}
		_ASSERT(ready_button);
		if (ready_button)
			ready_button->mLabel = "Ready";
	}
}

static void PutLawnItems(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
) {
	if (g_settingdlg.mStruct.mCoDefenceModeOpen)
	{
		int bkType = 0;
		__asm {
			mov eax, 0x6a9ec0
			mov eax, [eax]
			mov eax, [eax + 0x768]
			mov eax, [eax + 0x554C]
			mov bkType, eax
		}
		size_t row = (bkType == 2 || bkType == 3) ? 6 : 5;
		if (!g_settingdlg.mStruct.mUseCrater) {
			if (g_is_client) {
				for (size_t i = 0; i < row; i++)
				{
					for (size_t j = 5; j < 9; j++)
					{
						if (Board::CanTombPlaceAt(j, i))
							Board::SpawnTomb(j, i);
					}
				}
			}
			else {
				for (size_t i = 0; i < row; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						if (Board::CanTombPlaceAt(j, i))
							Board::SpawnTomb(j, i);
					}
				}
			}
		}
		else {
			if (g_is_client) {
				for (size_t i = 0; i < row; i++)
				{
					for (size_t j = 5; j < 9; j++)
					{
						Board::SpawnCrater(j, i);
					}
				}
			}
			else {
				for (size_t i = 0; i < row; i++)
				{
					for (size_t j = 0; j < 4; j++)
					{
						Board::SpawnCrater(j, i);
					}
				}
			}
		}

		//CanPlantAt
		if (!g_is_client && (bkType == 4 || bkType == 5)) {
			for (size_t i = 0; i < row; i++)
			{
				for (size_t j = 5; j < 8; j++)
				{
					Board::SpawnPlant(j, i, PlantType::PlantType::FlowerPot);
				}
			}
		}
	}
}

static void ZombieDropHead(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
) {
	if (!g_settingdlg.mStruct.mUseScore)
		return;
	GameZombie the_zombie = GameZombie(_ebx);
	if (!the_zombie.IsOnBoard())
		return;
	m_dead_zombies.push_back((Lawn::_Zombie*)the_zombie.GetBaseAddress());
	ScoreInteger socre_delta = GetDeltaScore(&the_zombie);

	g_Score += socre_delta;
	char s_str[10] = { 0 };
	snprintf(s_str, 10, "%d", socre_delta);
	auto p = SpawnNumParticleAt(the_zombie.X, the_zombie.Y, s_str);
	g_particles.push_back(p);
}

static void PlantDie(
	_Reg _edi,
	_Reg _esi,
	_Reg _ebp,
	_Reg _esp,
	_Reg _ebx,
	_Reg _edx,
	_Reg _ecx,
	_Reg _eax
) {
	bool is_plant_on_board = false;
	lpPlant plant = *(lpPlant*)(_esp + 0x24);
	__asm {
		mov eax, plant
		mov al,byte ptr [eax+0x144]
		mov is_plant_on_board,al
	}
	if (is_plant_on_board) {
		if (!g_is_client && g_settingdlg.mStruct.mCoDefenceModeOpen) {
			g_ser.All_SendASeedPack(*(int*)(plant + 0x24));
		}
	}
}



void InitAll()
{
	srand((unsigned int)time(0));
	m_font1 = LoadFontFromMemory((const char*)IDR_TTF2);
	DWORD pid = GetCurrentProcessId();
	PVZ::InitPVZ(pid);
	App::InitApp();
	InitPlantSystem();
	InitZombieSystem();
	EnableBackgroundRunning();
	PVZ::Memory::immediateExecute = true;

	m_game_tip_pos_x = GetPrivateProfileIntA("PVZHelper","GameTipX",0, "C:\\HPConfig\\HPconfig.ini");
	m_game_tip_pos_y = GetPrivateProfileIntA("PVZHelper","GameTipY",200, "C:\\HPConfig\\HPconfig.ini");

	Hook(GET_GRA_PATCH_ADDR, 7, GetGraphics);
	Hook(DRAW_PATCH_ADDR, 5, DrawUITop);
	Hook(ZOMBIE_DIE_PATCH_ADDR, 6, ZombieDie);
	Hook(BOARD_DRAW_BACKDROP_END, 6, DrawBoardBackgroundTop);
	Hook(SEED_CHOOSE_SCREEN_CTEATE_BUTTON, 5, SeedChooseScreenAddButton);
	Hook(PUT_LAWN_ITEMS, 6, PutLawnItems);
	Hook(ZOMBIE_DROP_HEAD, 6, ZombieDropHead);
	Hook(PLANT_DIE, 5, PlantDie);
	DWORD old_protext = 0;
	//修改属性为可读可写可执行
	VirtualProtect((LPVOID)0x44F587,4,PAGE_EXECUTE_READWRITE,&old_protext);
	PVZ::Memory::WriteMemory<char>(0x44F587, 0xEB);

	auto addr = HookAtWithoutBehaviour(RESTART_BUTTON_END, 6);

	char mulCode_restart[] = {
		CMP_EUX(REG_ESI,2),
		JNE,10,
		PUSHDWORD(0x45D5E6),
		RET,
	};
	PVZ::Memory::WriteArray(addr, mulCode_restart, sizeof(mulCode_restart));

	addr = HookAtWithoutBehaviour(ZOMBIE_SPAWN_WAVES_ADDR, 6);

	char mulCode[] = {
		IMUL_EUX_EVX_DWORD(REG_ECX,REG_ECX,1),
		MOV_PTR_ESP_ADD_V_EUX(REG_ECX,0x24),
		MOV_EAX(0),
	};
	m_zombie_spawn_plus_num_address = addr + 2;
	PVZ::Memory::WriteArray(addr, mulCode,sizeof(mulCode));

	if (g_is_client) {
		g_LawnDialog = App::Safe_DoDialog(0,"Tip","Wait for the Host to Enter a Game");
		char userName[20] = {0};
		PVZ::GetSaveData()->GetPVZUserName(userName);
		std::string n_str = userName;
		g_cl.ClientSendUserNames(n_str);
	}
	else {
		//暂时注释
		//auto i = App::Safe_DoDialog(3, "Tip", "Enter a Level you like and all\n your guests will join in it","OK");
	}

}

void DoMain()
{
	m_btnltr.DepressListener = (DWORD)ButtonOn;

	InitAll();
	new std::thread(KeyUpdateThread);
	auto App = PVZ::GetPVZApp();
	while (true)
	{
		if (m_zombie_plus_count != g_settingdlg.mStruct.mMoreZombie) {
			m_zombie_plus_count = g_settingdlg.mStruct.mMoreZombie;
			if (g_settingdlg.mStruct.mMoreZombie)
				PVZ::Memory::WriteMemory<char>(m_zombie_spawn_plus_num_address,3);
			else
				PVZ::Memory::WriteMemory<char>(m_zombie_spawn_plus_num_address, 1);
		}
		static bool is_open_game = false;
		bool is_end_cutscene = false;
		auto sptBoard = PVZ::GetBoard();
		if (sptBoard) {
			if (!is_open_game && !g_is_client) {
				is_open_game = true;
				g_Score = 0;
				if (!g_is_client) {
					g_ser.All_SendEnterGameMessage(PVZ::GetPVZApp()->LevelId, sptBoard->AdventureLevel);
				}
			}
			int state = 0;
			__asm {
				mov eax,0x6a9ec0
				mov eax,[eax]
				mov eax,[eax+0x7FC]
				mov state,eax
			}
			if (state == 3) {
				is_end_cutscene = true;
			}
		}
		else {
			is_open_game = false;
		}
		if (is_end_cutscene &&
			(App->LevelId != PVZLevel::Zen_Garden) &&
			(App->LevelId != PVZLevel::Treeof_Wisdom)
			) {
			if (!g_is_playing) {

				if (g_settingdlg.mStruct.mFaster) {
					PVZ::Memory::WriteMemory<bool>(0x6A9EAB,true);
					PVZ::Memory::WriteMemory<int>(0x4526D3,2);
				}
				if (g_settingdlg.mStruct.mSpawnModeOpen && g_show_detailed) {
					auto li = Sexy::MakeListener(&m_btnltr);
					//Sexy::MakeButton(Draw::ToString("Tomb"), li, BUTTON_TOMB);
					m_tomb_spawn_button = Sexy::MakeButton(Draw::ToString("Tomb"), li, BUTTON_TOMB);
					Sexy::ResizeButton(m_tomb_spawn_button, 0, 170, 117, 50);
					Sexy::AddToManager(m_tomb_spawn_button);
				}
				m_is_game_start = false;
				g_is_playing = true;
				
			}
			g_cl.ClientSendScore(g_Score);
			if (!g_is_client && g_settingdlg.mStruct.mCoDefenceModeOpen) {
				auto zoms = GetAllZombiesFixed(PVZ::GetBoard().get());
				for (auto & x : zoms)
				{
					int a_zombie_hp, a_zombie_hpMax = 0;
					x->GetBodyHp(&a_zombie_hp, &a_zombie_hpMax);
					if (x->X < circle_x && a_zombie_hpMax < 29000) {
						g_ser.All_SendAZombie((void*)x->GetBaseAddress());
						x->Remove();
					}
				}
			}
		}
		else {
			if (g_is_playing == true) {
				if (m_tomb_spawn_button) {
					Sexy::RemoveFromManager(m_tomb_spawn_button);
					Sexy::FreeWidget(m_tomb_spawn_button);
					m_tomb_spawn_button = 0x0;
				}
				if (g_settingdlg.mStruct.mFaster) {
					PVZ::Memory::WriteMemory<bool>(0x6A9EAB, false);
				}
				TryToCheckOut();
			}
			g_is_playing = false;
		}
		
		Sleep(20);
	}

}

void QuitAll()
{
	PVZ::QuitPVZ();
}

void TryToCheckOut()
{
	if (!g_is_client && g_ser.mClientStates.size() == 0)
		return;
	if (g_is_client) {
		g_cl.mIsFinishCheckout = false;
		g_cl.ClientSendScore(g_Score);
		g_cl.ClientSendFinishGame();
		auto i = App::Safe_DoDialog(0, "Tip", "Wait for other Players");
		while (g_cl.mIsFinishCheckout == false)
		{
			Sleep(100);
		}
		App::Safe_KillDialog(i);
		g_LawnDialog = App::Safe_DoDialog(0, "Tip", "Wait for the Host to Enter another Game");

	}
	else {
		lpLawnDialog i;
		if (g_settingdlg.mStruct.mCoDefenceModeOpen) {
			i = App::Safe_DoDialog(0, "Front Yard Win", "Wait for the Back Yard");
		}else
			i = App::Safe_DoDialog(0,"Tip","Wait for other Players");
		while (true)
		{
			bool IsOkay = true;
			for (auto& x : g_ser.mClientStates) {
				if (!x.second.mFinishGame)
					IsOkay = false;
			}
			if (IsOkay) {
				for (auto& x : g_ser.mClientStates) {
					x.second.mFinishGame = false;
				}
				break;
			}
			Sleep(100);
		}
		App::Safe_KillDialog(i);
		g_ser.All_SendEnterCheckOut();

		if (!g_settingdlg.mStruct.mUseScore)
			return;
		int max_tk = -1;
		ClientState maxc = {0};
		maxc.mScore = 0;

		char sName[20] = {0};
		PVZ::GetSaveData()->GetPVZUserName(sName);
		std::string sText = std::string(sName) +": " + std::to_string(g_Score) + "\n";
		
		for (auto & x :g_ser.mClientStates)
		{
			sText += x.second.mUserName + "(" + std::to_string(x.first) + ")" + ": " + std::to_string(x.second.mScore) + "\n";
			if (x.second.mScore > maxc.mScore) {
				maxc = x.second;
				max_tk = x.first;
			}
		}
		if (g_Score > maxc.mScore) {
			sText += "Max: " + std::string(sName) + ": " + std::to_string(g_Score);
		}
		else {
			sText += "Max: " + maxc.mUserName + "(" + std::to_string(max_tk) + ")" + ": " + std::to_string(maxc.mScore);
		}
		i = App::Safe_DoDialog(3, "Summary", sText, "I Know");
	}
}

void InitSocket(bool IsClient, Client cl, Server ser)
{
	g_is_client = IsClient;
	if (g_is_client) {
		g_cl = cl;
	}
	else {
		g_ser = ser;
	}

}
