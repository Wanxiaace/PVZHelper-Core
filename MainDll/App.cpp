#include "App.h"
#include "Hacker.h"

//#include <mutex>
//std::mutex mtx;
bool m_is_wait = false;

volatile bool m_is_okay = false;

void App::PreNewGame(unsigned int mode,int level)
{
	unsigned int app;
	if (mode == PVZLevel::PVZLevel::Adventure) {
		app = PVZ::Memory::ReadPointer(0x6A9EC0, 0x82C) + 0x24;
		PVZ::Memory::WriteMemory<int>(app, level);
	}
	
	lpFunc func1 = 0x44F9E0;//LawnApp::KillGameSelector
	lpFunc func2 = 0x44F560;//LawnApp::PreNewGame
	
	unsigned int ads = PVZ_BASE;

	__asm {
		mov esi,ads
		call func1

		push 0
		push mode
		mov esi, ads
		call func2
	}
}

//延迟执行，减少崩溃风险
void App::Safe_PreNewGame(unsigned int mode, int level)
{
	WaitPvZ();
	PreNewGame(mode, level);
	EndWait();
}

static void DoAfter(
	_Reg EDI,
	_Reg ESI,
	_Reg EBP,
	_Reg ESP,
	_Reg EBX,
	_Reg EDX,
	_Reg ECX,
	_Reg EAX
) {
	if (m_is_wait)
	{
		m_is_okay = true;
		while (m_is_wait) {
			m_is_okay = true;
		};
	}
}

void App::InitApp()
{
	Hook(0x55200C,7, DoAfter);
}

void App::WaitPvZ()
{
	m_is_okay = false;
	m_is_wait = true;
	while (!m_is_okay);//崩溃处
}

//与WaitPvZ连用，操作夹在中间
void App::EndWait()
{
	m_is_wait = false;
}

lpLawnDialog App::DoDialog(int button_mode,
	SexyString header_name,
	SexyString lines_name,
	SexyString button1_name,
	SexyString button2_name,
	int dialog_id)
{
	//MessageBoxA(NULL, NULL, NULL, NULL);
	lpSexyString lp_footer = Draw::ToString(button1_name.c_str());
	lpSexyString lp_lines = Draw::ToString(lines_name.c_str());
	lpSexyString lp_header = Draw::ToString(header_name.c_str());
	lpFunc func = 0x450770;
	lpLawnDialog result = 0x0;

	__asm {
		push button_mode
		push lp_footer
		push lp_lines
		push lp_header
		push 1
		push dialog_id
		mov ecx,0x6a9ec0
		mov ecx,[ecx]
		call func
		mov result,eax
	}

	return result;
}

lpLawnDialog App::Safe_DoDialog(int g_BtnMode, SexyString g_HeaderName, SexyString g_LinesName, SexyString g_Btn1Name, SexyString g_Btn2Name, int g_DialogId)
{
	WaitPvZ();
	lpLawnDialog ans = DoDialog(
		g_BtnMode,
		g_HeaderName,
		g_LinesName,
		g_Btn1Name,
		g_Btn2Name,
		g_DialogId);
	EndWait();
	return ans;
}

void App::GameShowTip(SexyString str)
{
	lpFunc Func = 0x40CA10;
	int style = 0x7;
	auto psi = Draw::ToString(str.c_str());
	auto bd = PVZ::GetBoard().get()->BaseAddress;

	__asm {
		mov ebx,-1
		mov ecx,style
		mov eax, psi
		mov edx, eax
		mov edi, bd
		call Func
	}
}

int App::Safe_LawnMessageBox(
	int g_BtnMode, 
	const char* g_HeaderName, 
	const char* g_LinesName, 
	const char* g_Btn1Name, 
	const char* g_Btn2Name,
	int g_DialogId
)
{
	WaitPvZ();
	lpFunc Func = 0x4504B0;

	unsigned int ads = PVZ_BASE;
	int re = -1;
	__asm {
		push g_BtnMode
		push g_Btn2Name
		push g_Btn1Name
		push g_LinesName
		push g_HeaderName
		push g_DialogId
		push ads
		call Func
		mov re,eax
	}
	EndWait();
	return re;
}

bool App::Safe_KillDialog(lpLawnDialog tar)
{
	_hp_Check_WithResult(tar,false);
	WaitPvZ();
	unsigned int ads = PVZ_BASE;
	lpFunc Func = 0x5462F0;
	bool re = false;
	__asm {
		push tar
		mov ecx,ads
		call Func
		mov re,al
	}
	EndWait();
	return re;
}

bool App::Is3DAccelerated()
{
	lpFunc Func = 0x554F90;
	bool re = false;
	__asm {
		mov eax, 0x6a9ec0
		mov eax, [eax]
		call Func
		mov re,al
	}
	return re;
}

void PVZDelete(void* _Block)
{
	lpFunc Func = 0x61C19A;
	__asm {
		push _Block
		call Func
		add esp,0x4
	}
}

unsigned int PVZNew(int _Size)
{
	unsigned int re = 0;
	lpFunc FuncNew = 0x61C130;
	__asm {
		push _Size
		call FuncNew
		mov re, eax
		add esp, 0x4
	}
	return re;
}

